#ifndef __READ_FILES__
#define __READ_FILES__

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <pthread.h>
#include "shared_details.h"

typedef uint64_t word_t;

const std::vector<word_t> *loadParsedCollectionFromFile(std::string collection_file);

const std::vector<word_t> *readProfileFromFile(std::string memcontent_file);

std::vector<unsigned int> *getDocumentAddresses(const std::vector<word_t> *collection);

const std::string *readFile(std::string file);

const std::vector<word_t> *getMarkerPositions(const std::string *documents);

#endif