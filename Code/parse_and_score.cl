#include "shared_details.h"

__kernel void parse_and_score(__global const uchar *documents,
                              __global const ulong4 *profile,
                              __global const ulong *positions,
                              __global ulong *scores)
{
    uint document = get_global_id(0) + 1;
    scores[document - 1] = documents[positions[document]];
}
