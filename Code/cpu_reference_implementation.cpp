#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"

#define VERBOSE

std::vector<unsigned int> *getDocumentAddresses(const std::vector<word_t> *collection)
{
    std::vector<unsigned int> *docAddresses = new std::vector<unsigned int>;
    // Address represents the word in collection the document id resides in.
    unsigned int address = 0;
    unsigned int numberOfDocuments = 0;
    // Placeholder at beginning of vector until we know true value.
    docAddresses->push_back(numberOfDocuments);
    // Loop over the entire collection to find the words representing documents
    for (std::vector<word_t>::const_iterator iter = collection->begin();
            iter != collection->end();
            ++iter)
    {
        word_t word = *iter;
        // Documents have their last four bits as zero (no ngrams so no length)
        if ((word & 0xF) == 0)
        {
            // We've found a document
            ++numberOfDocuments;
            // Add document address to vector
            docAddresses->push_back(address);
#ifdef VERBOSE
            std::cout << "ID " << (word >> 4) << " found at " << address << std::endl;
#endif
        }
        // Iterate the address after each iteration
        ++address;
    }
    // Store the length of the collection (in bytes) at the end of the vector
    // so we know where the collection and thus last document ends.
    docAddresses->push_back(collection->size() * sizeof(word_t)); // NEEDED?
    // Update the 0th element of the vector that was originally a placeholder
    // to the total number of documents in the vector
    docAddresses->at(0) = numberOfDocuments;
    return docAddresses;
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
    return 0;
}
