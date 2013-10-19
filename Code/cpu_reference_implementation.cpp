#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"

#define PROF_MASK 0x3FFFFF
#define PROF_WEIGHT 0x3FFFFFF
#define PROF_ADDR_LENGTH 0x3FFFFFFFFF

#define THRESHOLD 0

/*
 * Scores all documents and returns the results. Primarily used as a test of
 * the scoring algorithm as there is no threshold or categorisation of the
 * documents.
 */
unsigned long *singleThreadCPUNoThreshold(const std::vector<word_t> *collection,
        const std::vector<word_t> *profile,
        const std::vector<word_t> *docAddresses,
        unsigned long *scores)
{
    word_t numberOfDocuments = docAddresses->at(0);
    for (word_t document = 1; document <= numberOfDocuments; ++document)
    {
        word_t id = docAddresses->at(document);
        unsigned long score = 0;
        word_t numberOfTerms = docAddresses->at(document + 1) - id;
        for (word_t number = id; number < id + numberOfTerms; ++number)
        {
            // Get number-th term of document from collection.
            word_t term = collection->at(number);
            word_t rest = (term >> 4) & PROF_ADDR_LENGTH;
            // Profile address is the 22 most significant bits.
            // Left shift by 2 since we need to check four profile entries from
            // this address (this is instead of having a ulong4 structure to
            // match the OpenCL API and looking at each element.)
            word_t profileAddress = ((term >> 42) & PROF_MASK) << 2;
            // Get profile entry and add score to total document score
            word_t profileEntry = profile->at(profileAddress);
            if ((((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) == 1) {
                printf("%llu\t\t%llu\t\t0\t\t\t%llu\n", document, number - id, (profileEntry & PROF_WEIGHT));
            }
            score += (((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) * (profileEntry & PROF_WEIGHT);
            profileEntry = profile->at(profileAddress + 1);
            if ((((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) == 1) {
                printf("%llu\t\t%llu\t\t1\t\t\t%llu\n", document, number - id, (profileEntry & PROF_WEIGHT));
            }
            score += (((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) * (profileEntry & PROF_WEIGHT);
            profileEntry = profile->at(profileAddress + 2);
            if ((((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) == 1) {
                printf("%llu\t\t%llu\t\t2\t\t\t%llu\n", document, number - id, (profileEntry & PROF_WEIGHT));
            }
            score += (((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) * (profileEntry & PROF_WEIGHT);
            profileEntry = profile->at(profileAddress + 3);
            if ((((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) == 1) {
                printf("%llu\t\t%llu\t\t3\t\t\t%llu\n", document, number - id, (profileEntry & PROF_WEIGHT));
            }
            score += (((profileEntry >> 26) & PROF_ADDR_LENGTH) == rest) * (profileEntry & PROF_WEIGHT);
        }
        scores[document - 1] = score;
    }
    return scores;
}

void executeReferenceImplementation(const std::vector<word_t> *collection,
                                    const std::vector<word_t> *profile,
                                    const std::vector<word_t> *docAddresses)
{
    unsigned long *scores = new unsigned long[docAddresses->at(0)];
    printf("DocNum\tTermNum\tProfOffset\tScore\n");
    scores = singleThreadCPUNoThreshold(collection, profile, docAddresses, scores);
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
    for (word_t i = 0; i < docAddresses->at(0); ++i)
    {
        scores[i] > THRESHOLD ? ++spamCount : ++hamCount;
        std::cout << "Document " << i + 1 << " has score " << scores[i] << std::endl;
    }
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
    const std::vector<word_t> *docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    executeReferenceImplementation(collection, profile, docAddresses);
    return 0;
}
