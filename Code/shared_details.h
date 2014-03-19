#ifndef __SHARED_DETAILS__
#define __SHARED_DETAILS__

#define __CL_ENABLE_EXCEPTIONS

#define PROF_MASK 0x3FFFFF // 22 bits
#define PROF_WEIGHT 0x3FFFFFF // 26 bits
#define PROF_REST_LENGTH 0x3FFFFFFFFF // 38 bits

#define THRESHOLD 0

#define USEC_PER_SEC 1000000

#define NUM_THREADS 8

#ifndef REPETITIONS
#define REPETITIONS 10
#endif

#define NUM_NGRAMS 3
#define CHAR_SIZE 5
#define LENGTH 0xFUL //15 bits

//#define BLOOM_FILTER

#define BF15

#define DOCS_PER_THREAD 1

#ifndef COLLECTION_FILE
#define COLLECTION_FILE "collection.raw"
#endif
#define PROFILE_FILE "profile.bin"

#ifndef DOCUMENT_FILE
#define DOCUMENT_FILE "TREC_all.txt"
#endif
#define DOCUMENT_MARKER "<DOC>"

#ifndef HTML_FILE
#define HTML_FILE "01.warc"
#endif
#define WARC_MARKER "Content-Type: text/html"

#define SKIPPING 0
#define WRITING 1
#define FLUSHING 2
#define INSIDE_TAG 3

#define TERM_LENGTH 60
#define CHARACTER_SIZE 5

#define KERNEL_FULL_FILE "parse_and_score.cl"

#ifdef BLOOM_FILTER
#define KERNEL_FILE "gpu_kernel_bloom.cl"
#define KERNEL_NAME "scoreCollectionBloom"
#define KERNEL_FULL_NAME "parse_and_score_bloom"
#else
#define KERNEL_FILE "gpu_kernel_no_bloom.cl"
#define KERNEL_NAME "scoreCollectionNoBloom"
#define KERNEL_FULL_NAME "parse_and_score"
#endif

#ifdef BF15
#ifndef BLOOM_FILTER_FILE
#define BLOOM_FILTER_FILE "bloomfilter.raw"
#endif
#define BLOOM_SIZE 514
#define ADDR_BITS 15
#define ADDR_MASK 0x7FFF
#else
#define BLOOM_FILTER_FILE "bloomfilter_4k_17b.raw"
#define BLOOM_SIZE 2050
#define ADDR_BITS 17
#define ADDR_MASK 0x1FFFF
#endif

#endif
