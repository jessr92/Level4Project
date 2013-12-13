#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "read_files.h"
#include "shared_details.h"

int main(int argc, char *argv[])
{
    // Original file input as string
    std::string documents;
    // Beginning of document marker
    std::string substring = DOCUMENT_MARKER;
    // Index of first character for substring match
    std::vector<word_t> *positions = new std::vector<word_t>;
    // Work out file size, reserve enough space in string.
    std::ifstream in(DOCUMENT_FILE, std::ifstream::binary);
    in.seekg(0, std::ios::end);
    documents.reserve(in.tellg());
    in.seekg(0, std::ios::beg);
    // Read in file to string
    documents.assign((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
    in.close();
    const char *document_char = documents.c_str();
    // Find all document endings, note their location (assuming valid file)
    word_t pos = documents.find(substring, 0);
    word_t numberOfDocuments = 0;
    positions->push_back(numberOfDocuments); // Placeholder for actual value.
    while (pos != std::string::npos)
    {
        // Add index of first character for substring match
        positions->push_back(pos);
        numberOfDocuments++;
        pos = documents.find(substring, pos + 1);
    }
    // Add string length so positions.size() so last document knows where to end
    positions->push_back(documents.size());
    // Update 0th element to contain document count.
    positions->at(0) = numberOfDocuments;
    for (std::vector<word_t>::const_iterator iter = positions->begin(); iter != positions->end(); ++iter)
    {
        std::cout << *iter << std::endl;
    }
    return 0;
}
