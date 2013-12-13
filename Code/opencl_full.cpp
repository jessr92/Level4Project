#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "read_files.h"
#include "shared_details.h"

int main(int argc, char *argv[])
{
    // Read in document file and find out where the documents start.
    const std::string *docs = readFile(DOCUMENT_FILE);
    const std::vector<word_t> *pos = getMarkerPositions(docs);
    for (std::vector<word_t>::const_iterator iter = pos->begin(); iter != pos->end(); ++iter)
    {
        std::cout << *iter << std::endl;
    }
    //const char *doc_char = docs->c_str();
    return 0;
}
