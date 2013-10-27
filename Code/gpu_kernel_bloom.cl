#include "shared_details.h"

__kernel void scoreCollectionNoBloom(__global const ulong *collection,
                                     __global const ulong4 *profile,
                                     __global const uint *docAddresses,
                                     __global const uchar8 *bloomFilter,
                                     __global ulong *scores)
{

}