#include "shared_details.h"

__kernel void parse_and_score_bloom(__global const uchar *documents,
                                    __global const ulong4 *profile,
                                    __global const ulong *positions,
                                    __global ulong *scores,
                                    __global const ulong *bloomFilter)
{
    uint document = get_global_id(0) + 1;
    scores[document - 1] = documents[positions[document]];
}
