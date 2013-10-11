#include <iostream>
#include <fstream>
#include <vector>

typedef uint64_t word_t;

const std::vector<word_t> *loadParsedCollectionFromFile(std::string collection_file);

const std::vector<word_t> *readProfileFromFile(std::string memcontent_file);
