#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"

#define PROF_MASK 0x3FFFFF // 22 bits
#define PROF_WEIGHT 0x3FFFFFF // 26 bits
#define PROF_REST_LENGTH 0x3FFFFFFFFF // 38 bits

#define THRESHOLD 0

#define PROF_SZ 0xFFFFFF // 16M 64-bit words
#define PROF_MASK 0x3FFFFF

#define SCORES_STACKSZ 128 // (32*1024)
#define NTH 1
#define BF15
#ifdef BF15
#define ADDR_BITS  15
#define ADDR_MASK 0x7FFF
#define BF_SZ_BITS (32*1024)
#define BF_SZ_UCHAR (512*8)
#define BF_SZ_ULONG 512
#else
#define ADDR_BITS  17
#define ADDR_MASK 0x1FFFF
#define BF_SZ_BITS (4*32*1024)
#define BF_SZ_UCHAR (4*512*8)
#define BF_SZ_ULONG 2048
#endif


struct ulong4
{
    unsigned long s0, s1, s2, s3;
};

// Bloom filter check
unsigned char check_Bloom_filter(unsigned long term, const std::vector<word_t> *bloom_filter_p)
{

    unsigned char isHit = 1;
    for (unsigned int i = 0; i < 4; i++)
    {
        unsigned int addr = (term >> (64 - ADDR_BITS * (i + 1)) ) & ADDR_MASK ; //  this is a 15-bit address
        word_t w = bloom_filter_p->at(addr >> 6); // but we have 64-bit words, i.e. 2**6 bits
        // So which byte is it? bits 3..5
        unsigned int byte_addr = (addr >> 3) & 0x7; // 1 out of 8
        unsigned char byte = (w >> byte_addr) & 0xFF;
        // now assume the 3 LSbs are for the byte
        unsigned char bit = (byte >> (addr & 0x7)) & 0x1;
        isHit = isHit & bit;
    }
    return isHit;
}

void compute_reference(unsigned int nunits,
                       const std::vector<word_t> *collection_p,
                       std::vector<unsigned int> *docaddrs,
                       const std::vector<word_t> *profile_p,
                       const std::vector<word_t> *bloom_filter_p,
                       unsigned long *scores_stack,
                       const unsigned long threshold)
{
    printf("**** Running kernel\n");

    unsigned int ndocs = docaddrs->at(0);
    printf("**** NDocs: %d\n", ndocs);
    unsigned short ncores = nunits;
    unsigned int range = ndocs / ncores; // docaddrs is padded with zeroes to be a multiple of ncores
    printf("**** Range: %d\n", range);
    //- work out actual subrange of array
    for (unsigned short core_id = 0; core_id < nunits; core_id++)
    {
        printf("**** Running thread %d\n", core_id);
        unsigned int start_docids = core_id * range + 1; // address 0 stores ndocs
        unsigned int end_docids = start_docids + range;
        //- loop over array of doc addresses
        //      printf("**** Docs from %d to %d\n",start_docids,end_docids-1);
        for (unsigned int ii = start_docids; ii < end_docids; ii++)
        {
            unsigned int docaddr = docaddrs->at(ii); // this address contains the docid

            word_t docid = collection_p->at(docaddr) >> 4;
            //          printf("**** %d: %lu\n",docaddr,docid);
            unsigned int docscore;
            docscore = 0;
            unsigned int nterms = docaddrs->at(ii + 1) - docaddr;
            unsigned short th_id = 0;
            unsigned int start_terms = th_id * nterms;
            unsigned int stop_terms = start_terms + nterms;
            //- loop over terms per doc, which we know by looking at the next address, split loop over nthreads
            //int th_scores[NTH];
            unsigned int th_score = 0; // maybe real?
            //          printf("**** Terms from %d to %d\n",start_terms,stop_terms-1);
            for (unsigned int jj = start_terms; jj < stop_terms; jj++)
            {
                // here do the actual lookup & compute the score for the document
                // get the term
                unsigned long term = collection_p->at(docaddr + jj);
                //                  printf("**** Term at %d: %x\n",docaddr+jj,term);
                // does it occur in the profile?
                // Bloom filter check
                unsigned int isHit = check_Bloom_filter(term, bloom_filter_p);
                //                  printf("isHit: %d\n",isHit); // OK in TEST: all hits
                if (isHit == 1)
                {
                    // the naive way: take the 22 MSbs, use as address
                    unsigned int prof_entry_addr = ((term >> 42)&PROF_MASK) << 2; // lshift by 2 because 4 64-bit words
                    unsigned int rest = (term >> 4) & 0x3FFFFFFFFF; // 38 bits
                    ulong4 prof_entry;
                    prof_entry.s0 = profile_p->at(prof_entry_addr);
                    prof_entry.s1 = profile_p->at(prof_entry_addr + 1); // should be 0?
                    prof_entry.s2 = profile_p->at(prof_entry_addr + 2); // should be 0?
                    prof_entry.s3 = profile_p->at(prof_entry_addr + 3); // should be 0?
                    for (int ii = 0; ii < 4; ii++)
                    {
                        if (profile_p->at(prof_entry_addr + ii) != 0)
                        {
                            printf("**** Profile entry at %x (%d): %x\n", prof_entry_addr, ii, profile_p->at(prof_entry_addr) );
                            printf("**** (%x==%x)*(%x)\n", ((prof_entry.s0 >> 26) & 0x3FFFFFFFFF), rest, prof_entry.s0 & 0x3FFFFFF);
                        }
                    }

                    ulong4 tscore;
                    tscore.s0 = (((prof_entry.s0 >> 26) & 0x3FFFFFFFFF) == rest) * (prof_entry.s0 & 0x3FFFFFF);
                    tscore.s1 = (((prof_entry.s1 >> 26) & 0x3FFFFFFFFF) == rest) * (prof_entry.s1 & 0x3FFFFFF);
                    tscore.s2 = (((prof_entry.s2 >> 26) & 0x3FFFFFFFFF) == rest) * (prof_entry.s2 & 0x3FFFFFF);
                    tscore.s3 = (((prof_entry.s3 >> 26) & 0x3FFFFFFFFF) == rest) * (prof_entry.s3 & 0x3FFFFFF);
                    th_score += (tscore.s0 + tscore.s1 + tscore.s2 + tscore.s3);
                    //                  th_score+=1; // TEST!
                }
            }
            if (th_score != 0)
            {
                printf("**** Score per thread: %d\n", th_score);
            }
            //th_scores[th_id]=th_score;
            for (unsigned short tth = 0; tth < NTH; tth++)
            {
                if (tth == th_id)
                {
                    docscore += th_score;
                }
            }
            if (docscore != 0)
            {
                printf("**** Score per doc for %lu: %u\n", docid, docscore);
            }
            // So now somehow return the score for this doc, maybe with a threshold
            if (docscore > threshold)
            {
                printf("DocId: %lu Score: %u\n", docid, docscore);
                // Basically use a stack of scores per thread, stored in a single array
                // Means we have a limit on the number of scores per core/ total
                // read stack pointer
                unsigned long sp = scores_stack[core_id * 2 * SCORES_STACKSZ]; //  location 1 is unused to have powers of 2
                // write docid, write score
                scores_stack[core_id * 2 * SCORES_STACKSZ + sp] = docid;
                scores_stack[core_id * 2 * SCORES_STACKSZ + sp + 1] = docscore;
                // write stack pointer
                scores_stack[core_id * 2 * SCORES_STACKSZ] += 2; // note: no overflow protection!
            }
        }

    } // loop over cores
}

/*
 * Scores all documents and returns the results. Primarily used as a test of
 * the scoring algorithm as there is no threshold or categorisation of the
 * documents.
 *
 * Does not return the document ID with scores. This is a highly simplified
 * implementation.
 */
void singleThreadCPUNoThreshold(const std::vector<word_t> *collection,
                                const std::vector<word_t> *profile,
                                std::vector<unsigned int> *docAddresses,
                                unsigned long *scores)
{
    word_t numberOfDocuments = docAddresses->at(0);
    // Loop over all documents
    for (word_t document = 1; document <= numberOfDocuments; ++document)
    {
        // Item in docAddresses stores the index of the first term for document
        // so the next element denotes the upper bound (exclusive) for terms
        // in this document.
        word_t id = docAddresses->at(document);
        word_t endIndex = docAddresses->at(document + 1);
        unsigned long score = 0;
        // Loop over all terms in the document
        for (word_t number = id; number < endIndex; ++number)
        {
            // Get number-th term of document from collection.
            word_t term = collection->at(number);
            // Rest = bits representing the actual term from the collection
            word_t rest = (term >> 4) & PROF_REST_LENGTH;
            // Profile address is the 22 most significant bits.
            // Left shift by 2 since we need to check four profile entries from
            // this address (this is instead of having a ulong4 structure to
            // match the OpenCL API and looking at each element.)
            word_t profileAddress = ((term >> 42) & PROF_MASK) << 2;
            // Get profile entry and add score to total document score.
            // score = Lowest 26th elements of the profile entry.
            // The upper 38 bits represent the specific term which needs to
            // match rest (the actual term) from the collection.
            ulong4 profileEntry;
            profileEntry.s0 = profile->at(profileAddress);
            profileEntry.s1 = profile->at(profileAddress + 1);
            profileEntry.s2 = profile->at(profileAddress + 2);
            profileEntry.s3 = profile->at(profileAddress + 3);
            // Only one of these will add something non-zero to the score.
            // The left hand side of the * operator will either be 0 or 1.
            score += (((profileEntry.s0 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s0 & PROF_WEIGHT);
            score += (((profileEntry.s1 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s1 & PROF_WEIGHT);
            score += (((profileEntry.s2 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s2 & PROF_WEIGHT);
            score += (((profileEntry.s3 >> 26) & PROF_REST_LENGTH) == rest) * (profileEntry.s3 & PROF_WEIGHT);
        }
        // Since document starts at 1, store the ith document in ith-1 position.
        scores[document - 1] = score;
    }
}

void executeReferenceImplementation(const std::vector<word_t> *collection,
                                    const std::vector<word_t> *profile,
                                    std::vector<unsigned int> *docAddresses,
                                    const std::vector<word_t> *bloomFilter)
{
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
#define GR
#ifdef GR
    unsigned long *scores = new unsigned long[docAddresses->at(0)];
    for (word_t i = 0; i < docAddresses->at(0); i++)
    {
        scores[i] = 0;
    }
    singleThreadCPUNoThreshold(collection, profile, docAddresses, scores);
    for (word_t i = 0; i < docAddresses->at(0); ++i)
    {
        scores[i] > THRESHOLD ? ++spamCount : ++hamCount;
        std::cout << "Document " << i + 1 << " has score " << scores[i] << std::endl;
    }
#else
    unsigned long *scores = new unsigned long[(docAddresses->at(0) + 1) * 2];
    for (word_t i = 0; i < (docAddresses->at(0) + 1) * 2; i++)
    {
        scores[i] = 0;
    }
    scores[0] = 2;
    compute_reference(NTH, collection, docAddresses, profile, bloomFilter, scores, THRESHOLD);
    for (word_t i = 2; i < (docAddresses->at(0) + 1) * 2; i = i + 2)
    {
        scores[i + 1] > THRESHOLD ? ++spamCount : ++hamCount;
        std::cout << "Document " << scores[i] << " has score " << scores[i + 1] << std::endl;
    }
#endif
    std::cout << "Ham documents: " << hamCount << std::endl;
    std::cout << "Spam documents " << spamCount << std::endl;
}

int main()
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile("bloomfilter.raw");
    std::cout << "bloomfilter.raw: " << bloomFilter->size() << std::endl;
    const std::vector<word_t> *collection = loadParsedCollectionFromFile("collection.raw");
    std::cout << "collection.raw: " << collection->size() << std::endl;
    const std::vector<word_t> *profile = readProfileFromFile("profile.bin");
    std::cout << "profile.bin: " << profile->size() << std::endl;
    std::vector<unsigned int> *docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    executeReferenceImplementation(collection, profile, docAddresses, bloomFilter);
    return 0;
}
