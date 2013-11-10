#include "shared_details.h"

uchar checkBloomFilter(uint term, __global const uchar *bloomFilter);
uchar checkBloomFilterUnrolled(uint term, __global const uchar *bloomFilter);

__kernel void scoreCollectionBloom(__global const ulong *collection,
                                   __global const ulong4 *profile,
                                   __global const uint *docAddresses,
                                   __global ulong *scores,
                                   __global const uchar *bloomFilter)
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
#ifdef UNROLLED
        uchar isHit = checkBloomFilterUnrolled(term, bloomFilter);
#else
        uchar isHit = checkBloomFilter(term, bloomFilter);
#endif
        if (isHit)
        {
            // Rest = bits representing the actual term from the collection
            uint rest = (term >> 4) & PROF_REST_LENGTH;
            // Profile address is the 22 most significant bits.
            // Left shift not required in OpenCL Kernel since we're using vectors.
            // The ulong4 part has already been sorted by declaring profile in the
            // kernel as ulong4.
            uint profileAddress = ((term >> 42) & PROF_MASK);
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

uchar checkBloomFilter(uint term, __global const uchar *bloomFilter)
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
        uint addr = (term >> (64 - ADDR_BITS * (i + 1))) & ADDR_MASK;
        // The byte is stored at the address without the three least
        // significant bits.
        uchar byte = bloomFilter[addr >> 3];
        // 0x7 ensures byte is right shifted at most 7 places.
        // 0x1 ensures only the last bit can be 1.
        uchar bit = (byte >> (addr & 0x7)) & 0x1;
        // isHit is 1 only if both itself and the bloom filter bit at 1.
        isHit = isHit & bit;
    }
    return isHit;
}

uchar checkBloomFilterUnrolled(uint term, __global const uchar *bloomFilter)
{
    // Initialise result to 1 (found), this becomes and stays 0 if any of the
    // corresponding bloom filter location bits is 0.
    uchar isHit = 1;
    // i = 0 results in addr right shifting by 49 places, leaving 15 bits.
    // i = 1 results in addr right shifting by 34 places, leaving 30 bits.
    // i = 2 results in addr right shifting by 19 places, leaving 45 bits.
    // i = 3 results in addr right shifting by  4 places, leaving 60 bits.
    uint i = 0, addr = 0;
    uchar byte = 0, bit = 0;
    // Obtain the correct ADDR_BITS bits of term that represent the address
    // ADDR_MASK ensures only the last ADDR_BITS can be 1.
    addr = (term >> (64 - (ADDR_BITS * (++i)) ) & ADDR_MASK);
    // The byte is stored at the address without the three least
    // significant bits.
    byte = bloomFilter[addr >> 3];
    // 0x7 ensures byte is right shifted at most 7 places.
    // 0x1 ensures only the last bit can be 1.
    bit = (byte >> (addr & 0x7)) & 0x1;
    // isHit is 1 only if both itself and the bloom filter bit at 1.
    isHit = isHit & bit;
    addr = (term >> (64 - (ADDR_BITS * (++i)) ) & ADDR_MASK);
    byte = bloomFilter[addr >> 3];
    bit = (byte >> (addr & 0x7)) & 0x1;
    isHit = isHit & bit;
    addr = (term >> (64 - (ADDR_BITS * (++i)) ) & ADDR_MASK);
    byte = bloomFilter[addr >> 3];
    bit = (byte >> (addr & 0x7)) & 0x1;
    isHit = isHit & bit;
    addr = (term >> (64 - (ADDR_BITS * (++i)) ) & ADDR_MASK);
    byte = bloomFilter[addr >> 3];
    bit = (byte >> (addr & 0x7)) & 0x1;
    isHit = isHit & bit;
    return isHit;
}
