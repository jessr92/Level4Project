#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"

int main()
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile("bloomfilter.raw");
    std::cout << "bloomfilter.raw: " << bloomFilter->size() << std::endl;
    const std::vector<word_t> *collection = loadParsedCollectionFromFile("collection.raw");
    std::cout << "collection.raw: " << collection->size() << std::endl;
    const std::vector<word_t> *profile = readProfileFromFile("profile.bin");
    std::cout << "profile.bin: " << profile->size() << std::endl;
    const std::vector<unsigned int> *docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    return 0;
}
