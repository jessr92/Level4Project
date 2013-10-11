#include "read_files.h"

const std::vector<word_t> *loadParsedCollectionFromFile(std::string collection_file)
{
    std::vector<word_t> *collection = new std::vector<word_t>;
    std::ifstream fh(collection_file.c_str());
    if (fh.bad())
    {
        std::cerr << "Error: Could not open " << collection_file << std::endl;
        exit (8);
    }
    fh >> std::hex;
    word_t word;
    while (  fh >> word)
    {
        collection->push_back(word);
    }
    fh.close();
    return collection;
}

const std::vector<word_t> *readProfileFromFile(std::string memcontent_file)
{
    std::vector<word_t> *profile = new std::vector<word_t>;
    std::ifstream bfh(memcontent_file.c_str(), std::ios::in | std::ios::binary);
    if (bfh.bad())
    {
        std::cerr << "Error: Could not open " << memcontent_file << std::endl;
        exit (8);
    }
    word_t w = 0;
    while (!bfh.eof())
    {
        bfh.read((char *)(&w), sizeof(w)) ;
        profile->push_back(w);
    }
    bfh.close ();
    return profile;
}
