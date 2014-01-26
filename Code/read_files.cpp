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

word_t *values;
word_t *threadIds;
std::vector<word_t> **docVectors;
const std::string *doc;
word_t threadCount = NUM_THREADS;

void *calculateMarkerPositions(void *value)
{
    word_t tid = *(word_t *)value;
    word_t pos = (tid == 0) ? 0 : values[tid - 1];
    word_t end = values[tid];
    docVectors[tid] = new std::vector<word_t>();
    // Find all document endings, note their location (assuming valid file)
    pos = doc->find(DOCUMENT_MARKER, pos);
    while (pos < end && pos != std::string::npos)
    {
        // Add index of first character for substring match
        docVectors[tid]->push_back(pos);
        pos = doc->find(DOCUMENT_MARKER, pos + 1);
    }
    return NULL;
}

const std::vector<word_t> *getMarkerPositions(const std::string *documents)
{
    doc = documents;
    threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0)
    {
        threadCount = NUM_THREADS;
    }
    values = new word_t[threadCount];
    threadIds = new word_t[threadCount];
    docVectors = new std::vector<word_t> *[threadCount];
    pthread_t *threads = new pthread_t[threadCount];
    unsigned long docLength = documents->size();
    if (docLength % threadCount != 0)
    {
        docLength += (threadCount - (docLength % threadCount));
    }
    for (word_t i = 0; i < threadCount; i++)
    {
        values[i] = (docLength / threadCount) * (i + 1);
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, calculateMarkerPositions, (void *)&threadIds[i]);
    }
    for (word_t i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }
    std::vector<word_t> *result = new std::vector<word_t>();
    result->push_back(0);
    unsigned long numberOfDocuments = 0;
    for (word_t i = 0; i < threadCount; i++)
    {
        numberOfDocuments += docVectors[i]->size();
        result->insert(result->end(), docVectors[i]->begin(), docVectors[i]->end());
    }
    result->at(0) = numberOfDocuments;
    result->push_back(documents->size());
    return result;
}
