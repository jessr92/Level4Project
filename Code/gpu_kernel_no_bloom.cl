#include "shared_details.h"

void generateNGrams(ulong term, ulong *reg, ulong *ngrams);

__kernel void scoreCollectionNoBloom(__global const ulong *collection,
                                     __global const ulong4 *profile,
                                     __global const uint *docAddresses,
                                     __global ulong *scores)
{
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
        // Since document starts at 1, store the ith document in ith-1 position.
        scores[document - 1] = score;
    }
}

void generateNGrams(ulong term, ulong *reg, ulong *ngrams)
{
    // Shift the terms along, losing the oldest term.
    for (int i = 0; i < NUM_NGRAMS; i++)
    {
        reg[i] = reg[i - 1];
    }
    // Add new term to reg
    reg[0] = term;
    // Build ngrams
    ngrams[0] = reg[0];
    for (uint i = 1; i < NUM_NGRAMS; i++)
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
