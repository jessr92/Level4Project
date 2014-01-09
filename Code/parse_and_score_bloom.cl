#include "shared_details.h"

uchar checkBloomFilter(ulong term,
                       __local const ulong *bloomFilter);

void generateNGrams(ulong term,
                    ulong *reg,
                    ulong *ngrams);

ulong score_term(ulong term,
                 __global const ulong4 *profile,
                 __local const ulong *localBloomFilter,
                 ulong *reg,
                 ulong *ngrams);

ulong to5BitEncoding(char c);

__kernel void parse_and_score_bloom(__global const char *documents,
                                    __global const ulong4 *profile,
                                    __global const ulong *positions,
                                    __global ulong *scores,
                                    __global const ulong *bloomFilter,
                                    __global int *state)
{
    // Copy bloom filter to local storage
    __local ulong localBloomFilter[BLOOM_SIZE];
    __local int nextState[25];
    if (get_local_id(0) == 0)
    {
        for (uint i = 0; i < BLOOM_SIZE; i++)
        {
            localBloomFilter[i] = bloomFilter[i];
        }
        for (uint i = 0; i < 25; i++)
        {
            nextState[i] = state[i];
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    // Keep a record of terms that make up the ngrams, and the ngrams themselves
    ulong reg[NUM_NGRAMS];
    ulong ngrams[NUM_NGRAMS];
    for (uint i = 0; i < NUM_NGRAMS; i++)
    {
        reg[i] = 0;
        ngrams[i] = 0;
    }
    // Document to parse
    uint document = get_global_id(0) + 1;
    ulong startParse = positions[document];
    ulong endParse = positions[document + 1];
    ulong score = 0;
    ulong termToScore = 0;
    // 0 = Skipping
    // 1 = Writing
    // 2 = Flushing
    // 3 = Inside Tag
    int currentState = SKIPPING;
    int bitn = 0;
    for (ulong pos = startParse; pos < endParse; pos++)
    {
        char c = documents[pos];
        // if isalnum(c)
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        {
            currentState = nextState[5 * currentState];
            if ((bitn < TERM_LENGTH - 4) && (currentState == WRITING))
            {
                termToScore += to5BitEncoding(c) << (bitn + 4);
                bitn += CHARACTER_SIZE;
            }
        }
        // else if isspace(c)
        else if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
        {
            currentState = nextState[(5 * currentState) + 2];
            if ((bitn > 0) && (currentState == FLUSHING))
            {
                termToScore += (bitn / CHARACTER_SIZE);
                score += score_term(termToScore, profile, localBloomFilter, reg, ngrams);
                termToScore = 0;
                bitn = 0;
            }
        }
        else if (c == '<')
        {
            currentState = nextState[(5 * currentState) + 3];
        }
        else if (c == '>')
        {
            currentState = nextState[(5 * currentState) + 4];
        }
        else
        {
            currentState = nextState[(5 * currentState) + 1];
        }
    }
    scores[document - 1] = score;
}

ulong score_term(ulong term,
                 __global const ulong4 *profile,
                 __local const ulong *localBloomFilter,
                 ulong *reg,
                 ulong *ngrams)
{
    // Generate the ngrams now we know of this term.
    generateNGrams(term, reg, ngrams);
    ulong score = 0;
    for (uint i = 0; i < NUM_NGRAMS; i++)
    {
        term = ngrams[i];
        uchar isHit = checkBloomFilter(term, localBloomFilter);
        if (isHit)
        {
            // Rest = bits representing the actual term from the collection
            ulong rest = (term >> 4) & PROF_REST_LENGTH;
            // Profile address is the 22 most significant bits.
            // Left shift not required in OpenCL Kernel since we're using vectors.
            // The ulong4 part has already been sorted by declaring profile in the
            // kernel as ulong4.
            // Hashing the term for better spread across the profile address space
            // ulong h1term = (term >> 32) ^ term;
            // ulong profileAddress = ((h1term >> 10) ^ (h1term & 0x3FFUL)) & PROF_MASK;
            ulong profileAddress = ((term >> 42) & PROF_MASK);
            // Get profile entry and add score to total document score.
            // score = Lowest 26th elements of the profile entry.
            // The upper 38 bits represent the specific term which needs to
            // match rest (the actual term) from the collection.
            ulong4 profileEntry = profile[profileAddress];
            // Only one of these will add something non-zero to the score.
            // The left hand side of the * operator will either be 0 or 1.
            score += (((profileEntry.s0 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s0 & PROF_WEIGHT);
            score += (((profileEntry.s1 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s1 & PROF_WEIGHT);
            score += (((profileEntry.s2 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s2 & PROF_WEIGHT);
            score += (((profileEntry.s3 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s3 & PROF_WEIGHT);
        }
    }
    return score;
}

uchar checkBloomFilter(ulong term, __local const ulong *bloomFilter)
{
    // Initialise result to 1 (found), this becomes and stays 0 if any of the
    // corresponding bloom filter location bits is 0.
    uchar isHit = 1;
    // i = 0 results in addr right shifting by 49 places, leaving 15 bits.
    // i = 1 results in addr right shifting by 34 places, leaving 30 bits.
    // i = 2 results in addr right shifting by 19 places, leaving 45 bits.
    // i = 3 results in addr right shifting by  4 places, leaving 60 bits.
    for (uint i = 0; i < 4; i++)
    {
        // Obtain the correct ADDR_BITS bits of term that represent the address
        // ADDR_MASK ensures only the last ADDR_BITS can be 1.
        ulong addr = (term >> (64 - ADDR_BITS * (i + 1))) & ADDR_MASK;
        // The byte is stored at the address without the six least
        // significant bits.
        ulong byte = bloomFilter[1 + (addr >> 6)];
        // 0x7 ensures byte is right shifted at most 7 places.
        // 0x1 ensures only the last bit can be 1.
        // Get the appropriate bit to determine if there is a hit or not.
        ulong bit = (byte >> (addr & 0x7)) & 0x1;
        // isHit is 1 only if both itself and the bloom filter bit at 1.
        isHit = isHit & bit;
    }
    return isHit;
}

void generateNGrams(ulong term, ulong *reg, ulong *ngrams)
{
    // Shift the terms along, losing the oldest term.
    for (int i = NUM_NGRAMS - 1; i > 0; --i)
    {
        reg[i] = reg[i - 1];
    }
    // Add new term to reg
    reg[0] = term;
    // Build ngrams
    ngrams[0] = reg[0];
    for (uint i = 1; i < NUM_NGRAMS; ++i)
    {
        // Check if the (n-1)th ngram has already taken all 60 bits.
        ulong oldLength = ngrams[i - 1] & LENGTH;
        if (oldLength < 12)
        {
            // Previous ngram without length field
            ulong temp = ngrams[i - 1] >> 4;
            // Generate new ngram
            ngrams[i] = (temp << ((reg[i] & LENGTH) * CHAR_SIZE + 4)) + reg[i];
            // Compute length of ngram
            ulong newLength = (reg[i] & LENGTH) + oldLength;
            // Append next word to the ngram
            newLength = (newLength > 12) ? 12 : newLength;
            // Remove old length and append the new one
            ngrams[i] = ((ngrams[i] >> 4) << 4) + newLength;
        }
    }
}

ulong to5BitEncoding(char c)
{
    switch (c)
    {
    case 'i':
    case 'j':
    case 'I':
    case 'J':
        return 1UL; // i, j, I, J -> 1
    case 'o':
    case 'O':
        return 0UL; // o, O -> 0
    case 'u':
    case 'U':
        return 27UL; // u, U -> V
    }
    ulong v = convert_ulong(c);
    if (c >= 'a' && c <= 'z')
    {
        ulong tcode = v - ((v > 106) ? ((v > 111) ? ((v > 117) ? 4 : 3) : 2) : 0);
        return tcode - 87; // += 10 - 97;
    }
    else if (c >= 'A' && c <= 'Z')
    {
        ulong tcode = v - ((v > 74) ? ((v > 79) ? ((v > 86) ? 4 : 3) : 2) : 0);
        return tcode - 55; // += 10 - 65;
    }
    else if (c >= '0' && c <= '9')
    {
        return v - 48;
    }
    return 0;
}
