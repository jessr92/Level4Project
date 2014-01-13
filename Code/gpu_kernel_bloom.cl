#include "shared_details.h"

uchar checkBloomFilter(ulong term, __local const ulong *bloomFilter);
void generateNGrams(ulong term, ulong *reg, ulong *ngrams);

__kernel void scoreCollectionBloom(__global const ulong *collection,
                                   __global const ulong4 *profile,
                                   __global const uint *docAddresses,
                                   __global ulong *scores,
                                   __global const ulong *bloomFilter)
{
    if (get_global_id(0) >= docAddresses[0])
    {
        return;
    }
    __local ulong localBloomFilter[BLOOM_SIZE];
    if (get_local_id(0) == 0)
    {
        for (uint i = 0; i < BLOOM_SIZE; i++)
        {
            localBloomFilter[i] = bloomFilter[i];
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    ulong reg[NUM_NGRAMS];
    ulong ngrams[NUM_NGRAMS];
    uint document = (get_global_id(0) * DOCS_PER_THREAD) + 1;
    uint endDoc = (document + DOCS_PER_THREAD > docAddresses[0]) ? docAddresses[0] + 1 : document + DOCS_PER_THREAD;
    // Item in docAddresses stores the index of the first term for document
    // so the next element denotes the upper bound (exclusive) for terms
    // in this document.
    for (; document < endDoc; document++)
    {
        for (uint i = 0; i < NUM_NGRAMS; i++)
        {
            reg[i] = 0;
            ngrams[i] = 0;
        }
        uint id = docAddresses[document];
        uint endIndex = docAddresses[document + 1];
        ulong score = 0;
        // Loop over all terms in the document
        for (uint number = id; number < endIndex; ++number)
        {
            // Get number-th term of document from collection.
            ulong term = collection[number];
            // Generate the ngrams now we know of this term.
            generateNGrams(term, reg, ngrams);
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
        }
        // Since document starts at 1, store the ith document in ith-1 position.
        scores[document - 1] = score;
    }
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

