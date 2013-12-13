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
    while (fh >> word)
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
        bfh.read((char *)(&w), sizeof(w));
        profile->push_back(w);
    }
    bfh.close();
    return profile;
}

std::vector<unsigned int> *getDocumentAddresses(const std::vector<word_t> *collection)
{
    std::vector<unsigned int> *docAddresses = new std::vector<unsigned int>;
    // Address represents the word in collection the document id resides in.
    word_t address = 0;
    word_t numberOfDocuments = 0;
    // Placeholder at beginning of vector until we know true value.
    docAddresses->push_back(numberOfDocuments);
    // Loop over the entire collection to find the words representing documents
    for (std::vector<word_t>::const_iterator iter = collection->begin(); iter != collection->end(); ++iter)
    {
        word_t word = *iter;
        // Documents have their last four bits as zero (no ngrams so no length)
        if ((word & 0xF) == 0)
        {
            // We've found a document
            ++numberOfDocuments;
            // Add document address to vector
            docAddresses->push_back(address);
        }
        // Iterate the address after each iteration
        ++address;
    }
    // Store the length of the collection (in bytes) at the end of the vector
    // so we know where the collection and thus last document ends.
    docAddresses->push_back(collection->size());
    // Update the 0th element of the vector that was originally a placeholder
    // to the total number of documents in the vector
    docAddresses->at(0) = numberOfDocuments;
    return docAddresses;
}

const std::string *readFile(std::string file)
{
    std::string *documents = new std::string();
    std::ifstream in(file, std::ifstream::binary);
    in.seekg(0, std::ios::end);
    documents->reserve(in.tellg());
    in.seekg(0, std::ios::beg);
    // Read in file to string
    documents->assign((std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>());
    in.close();
    return documents;
}

const std::vector<word_t> *getMarkerPositions(const std::string *documents)
{
    std::vector<word_t> *positions = new std::vector<word_t>;
    // Find all document endings, note their location (assuming valid file)
    word_t pos = documents->find(DOCUMENT_MARKER, 0);
    word_t numberOfDocuments = 0;
    positions->push_back(numberOfDocuments); // Placeholder for actual value.
    while (pos != std::string::npos)
    {
        // Add index of first character for substring match
        positions->push_back(pos);
        numberOfDocuments++;
        pos = documents->find(DOCUMENT_MARKER, pos + 1);
    }
    // Add string length so positions.size() so last document knows where to end
    positions->push_back(documents->size());
    // Update 0th element to contain document count.
    positions->at(0) = numberOfDocuments;
    return positions;
}
