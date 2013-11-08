#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"
#include "shared_details.h"
#include "OclWrapper.h"

void executeGPUImplementation(const std::vector<word_t> *collection,
                              const std::vector<word_t> *profile,
                              std::vector<unsigned int> *docAddresses,
                              const std::vector<word_t> *bloomFilter)
{
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
    unsigned long *scores = new unsigned long[docAddresses->at(0)];
    for (word_t i = 0; i < docAddresses->at(0); i++)
    {
        scores[i] = 0;
    }
    word_t *tempCollection = new word_t[collection->size()];
    std::copy(collection->begin(), collection->end(), tempCollection);
    unsigned int *tempDocAddresses = new unsigned int[docAddresses->size()];
    std::copy(docAddresses->begin(), docAddresses->end(), tempDocAddresses);
    word_t *tempProfile = new word_t[profile->size()];
    std::copy(profile->begin(), profile->end(), tempProfile);
    // Sizes
    int collectionSize = sizeof(word_t) * collection->size();
    int profileSize = sizeof(word_t) * profile->size();
    int docAddressesSize = sizeof(unsigned int) * docAddresses->size();
    int bloomFilterSize = sizeof(word_t) * bloomFilter->size();
    int scoresSize = sizeof(unsigned long) * docAddresses->at(0);
    // Insert OcLWrapper stuff here
    OclWrapper ocl;
    ocl.initOclWrapper(KERNEL_FILE, KERNEL_NAME, "");
    cl::Buffer d_scores = ocl.makeReadWriteBuffer(scoresSize, scores, CL_MEM_READ_WRITE | CL_MEM_READ_MODE);
    cl::Buffer d_profile = ocl.makeReadBuffer(profileSize, tempProfile, CL_MEM_READ_ONLY | CL_MEM_READ_MODE);
    cl::Buffer d_collection = ocl.makeReadBuffer(collectionSize, tempCollection, CL_MEM_READ_ONLY | CL_MEM_READ_MODE);
    cl::Buffer d_docAddresses = ocl.makeReadBuffer(docAddressesSize, tempDocAddresses, CL_MEM_READ_ONLY | CL_MEM_READ_MODE);
    ocl.writeBuffer(d_scores, scoresSize, scores);
    ocl.writeBuffer(d_profile, profileSize, tempProfile);
    ocl.writeBuffer(d_collection, collectionSize, tempCollection);
    ocl.writeBuffer(d_docAddresses, docAddressesSize, tempDocAddresses);
    ocl.enqueueNDRange(cl::NDRange(docAddresses->at(0)), cl::NDRange(1));
    ocl.runKernel(d_collection, d_profile, d_docAddresses, d_scores).wait();
    ocl.readBuffer(d_scores, scoresSize, scores);
    for (word_t i = 0; i < docAddresses->at(0); ++i)
    {
        scores[i] > THRESHOLD ? ++spamCount : ++hamCount;
        std::cout << (collection->at(docAddresses->at(i + 1)) >> 4) << ":" << scores[i] << std::endl;
    }
    std::cout << "Ham documents: " << hamCount << std::endl;
    std::cout << "Spam documents " << spamCount << std::endl;
    delete [] scores;
    delete [] tempProfile;
}

int main()
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile(BLOOM_FILTER_FILE);
    std::cout << BLOOM_FILTER_FILE << ": " << bloomFilter->size() << std::endl;
    const std::vector<word_t> *collection = loadParsedCollectionFromFile(COLLECTION_FILE);
    std::cout << COLLECTION_FILE << ": " << collection->size() << std::endl;
    const std::vector<word_t> *profile = readProfileFromFile(PROFILE_FILE);
    std::cout << PROFILE_FILE << ": " << profile->size() << std::endl;
    std::vector<unsigned int> *docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    executeGPUImplementation(collection, profile, docAddresses, bloomFilter);
    delete bloomFilter;
    delete collection;
    delete profile;
    delete docAddresses;
    return 0;
}
