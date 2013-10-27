#include "shared_details.h"

__kernel void scoreCollectionNoBloom(__global const ulong *collection,
                                     __global const ulong4 *profile,
                                     __global const uint *docAddresses,
                                     __global ulong *scores)
{
    int document = get_global_id(0) + 1;
    // Item in docAddresses stores the index of the first term for document
    // so the next element denotes the upper bound (exclusive) for terms
    // in this document.
    uint id = docAddresses[document];
    uint endIndex = docAddresses[document + 1];
    ulong score = 0;
    // Loop over all terms in the document
    for (uint number = id; number < endIndex; ++number)
    {
        // Get number-th term of document from collection.
        uint term = collection[number];
        // Rest = bits representing the actual term from the collection
        uint rest = (term >> 4) & PROF_REST_LENGTH;
        // Profile address is the 22 most significant bits.
        // Left shift by 2 since we need to check four profile entries from
        // this address (this is instead of having a ulong4 structure to
        // match the OpenCL API and looking at each element.)
        uint profileAddress = ((term >> 42) & PROF_MASK) << 2;
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
    // Since document starts at 1, store the ith document in ith-1 position.
    //printf("%d\n", score);
    //scores[document - 1] = score;
    scores[document -1] = document;
}