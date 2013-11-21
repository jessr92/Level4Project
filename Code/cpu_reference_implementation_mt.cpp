#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include "read_files.h"
#include "shared_details.h"
#include <sys/time.h>
#include "OpenCLUtils.h"

double time_elapsed;
double startt, endt;

const std::vector<word_t> *collection;
const std::vector<word_t> *profile;
std::vector<unsigned int> *docAddresses;
unsigned long *scores;

inline void mark_time()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    startt = tim.tv_sec + (tim.tv_usec / static_cast<double>(USEC_PER_SEC));
}

inline void stop_time()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    endt = tim.tv_sec + (tim.tv_usec / static_cast<double>(USEC_PER_SEC));
    time_elapsed = endt - startt;
}

struct ulong4
{
    unsigned long s0, s1, s2, s3;
};

void *multiThreadCPUNoThresholdOrBloom(void *i)
{
    word_t document = *(word_t *)i;
    word_t numberOfDocuments = docAddresses->at(0);
    word_t end = document + (numberOfDocuments / NUM_THREADS);
    if (end > numberOfDocuments)
    {
        end = numberOfDocuments;
    }
    // Loop over all documents
    for (; document <= end; ++document)
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
    return NULL;
}

void executeReferenceImplementation(const std::vector<word_t> *collection,
                                    const std::vector<word_t> *profile,
                                    std::vector<unsigned int> *docAddresses,
                                    const std::vector<word_t> *bloomFilter)
{
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
    scores = new unsigned long[docAddresses->at(0)];
    for (word_t i = 0; i < docAddresses->at(0); i++)
    {
        scores[i] = 0;
    }
    mark_time();
    std::cout << "Threads" << NUM_THREADS << std::endl;
    pthread_t *threads = new pthread_t[NUM_THREADS];
    word_t *values = new word_t[NUM_THREADS];
    word_t roundedDocs = docAddresses->at(0) + (docAddresses->at(0) % NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        values[i] = ((roundedDocs / NUM_THREADS) * i) + 1;
        pthread_create(&threads[i], NULL, multiThreadCPUNoThresholdOrBloom, (void *)&values[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    stop_time();
    std::cout << time_elapsed << " seconds to score documents." << std::endl;
    for (word_t i = 0; i < docAddresses->at(0); ++i)
    {
        scores[i] > THRESHOLD ? ++spamCount : ++hamCount;
        std::cout << (collection->at(docAddresses->at(i + 1)) >> 4) << ":" << scores[i] << std::endl;
    }
    std::cout << "Ham documents: " << hamCount << std::endl;
    std::cout << "Spam documents " << spamCount << std::endl;
    delete [] scores;
}

int main()
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile(BLOOM_FILTER_FILE);
    std::cout << BLOOM_FILTER_FILE << ": " << bloomFilter->size() << std::endl;
    collection = loadParsedCollectionFromFile(COLLECTION_FILE);
    std::cout << COLLECTION_FILE << ": " << collection->size() << std::endl;
    profile = readProfileFromFile(PROFILE_FILE);
    std::cout << PROFILE_FILE << ": " << profile->size() << std::endl;
    docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    executeReferenceImplementation(collection, profile, docAddresses, bloomFilter);
    delete bloomFilter;
    delete collection;
    delete profile;
    delete docAddresses;
    return 0;
}
