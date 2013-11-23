#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"
#include "shared_details.h"
#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS
#ifdef __APPLE__
#include "cl.hpp"
#else
#include <CL/cl.hpp>
#endif
#include <sys/time.h>
#include "OpenCLUtils.h"

double time_elapsed;
double startt, endt;

inline void mark_time()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    startt = tim.tv_sec + (tim.tv_usec / static_cast<double>(USEC_PER_SEC));
}

inline void stop_time()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    endt = tim.tv_sec + (tim.tv_usec / static_cast<double>(USEC_PER_SEC));
    time_elapsed = endt - startt;
}

void executeCPUImplementation(const std::vector<word_t> *collection,
                              const std::vector<word_t> *profile,
                              const std::vector<word_t> *bloomFilter,
                              std::vector<unsigned int> *docAddresses)
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
#ifdef BLOOM_FILTER
    word_t *tempBloomFilter = new word_t[bloomFilter->size()];
    std::copy(bloomFilter->begin(), bloomFilter->end(), tempBloomFilter);
#endif
    try
    {
        // Get the available platforms.
        cl::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        // Get a list of devices on this platform.
        cl::vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
        std::cout << "Device name: " << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
        // Create a context for the devices
        cl::Context context(devices);
        // Create a command queue for the first device
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);
        // Create the memory buffers
        int collectionSize = sizeof(word_t) * collection->size();
        cl::Buffer d_collection = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, collectionSize, tempCollection);
        int profileSize = sizeof(word_t) * profile->size();
        cl::Buffer d_profile = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, profileSize, tempProfile);
        int docAddressesSize = sizeof(unsigned int) * docAddresses->size();
        cl::Buffer d_docAddresses = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, docAddressesSize, tempDocAddresses);
#ifdef BLOOM_FILTER
        int bloomFilterSize = sizeof(word_t) * bloomFilter->size();
        cl::Buffer d_bloomFilter = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, bloomFilterSize, tempBloomFilter);
#endif
        int scoresSize = sizeof(scores) * docAddresses->at(0);
        cl::Buffer d_scores = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, scoresSize, scores);
        // Read the program source
        std::ifstream sourceFile(KERNEL_FILE);
        std::string sourceCode(std::istreambuf_iterator < char > (sourceFile), (std::istreambuf_iterator < char > ()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
        // Make program from the source code
        cl::Program program = cl::Program(context, source);
        // Build the program for the devices
        program.build(devices);
        // Make kernel
        cl::Kernel kernel(program, KERNEL_NAME);
        // Set the kernel arguments
        kernel.setArg(0, d_collection);
        kernel.setArg(1, d_profile);
        kernel.setArg(2, d_docAddresses);
        kernel.setArg(3, d_scores);
#ifdef BLOOM_FILTER
        kernel.setArg(4, d_bloomFilter);
#endif
        mark_time();
        for (int i = 0; i < REPETITIONS; i++)
        {
            // Copy the input data to the input buffers using the command queue
            queue.enqueueMapBuffer(d_collection, CL_FALSE, CL_MAP_READ, 0, collectionSize);
            queue.enqueueMapBuffer(d_profile, CL_FALSE, CL_MAP_READ, 0, profileSize);
            queue.enqueueMapBuffer(d_docAddresses, CL_FALSE, CL_MAP_READ, 0, docAddressesSize);
            queue.enqueueMapBuffer(d_scores, CL_FALSE, CL_MAP_WRITE, 0, scoresSize);
#ifdef BLOOM_FILTER
            queue.enqueueMapBuffer(d_bloomFilter, CL_FALSE, CL_MAP_READ, 0, bloomFilterSize);
#endif
            // Execute the kernel
            cl::NDRange global((docAddresses->at(0) / DOCS_PER_THREAD) + 1);
            queue.finish();
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
            queue.finish();
        }
        stop_time();
        std::cout << time_elapsed << " seconds to score documents " << REPETITIONS << " times." << std::endl;
    }
    catch (cl::Error error)
    {
        std::cout << error.what() << "(" << clErrorString(error.err()) << ")" << std::endl;
        return;
    }
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

int main(int argc, char *argv[])
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile(BLOOM_FILTER_FILE);
    std::cout << BLOOM_FILTER_FILE << ": " << bloomFilter->size() << std::endl;
    const std::vector<word_t> *collection = loadParsedCollectionFromFile(COLLECTION_FILE);
    std::cout << COLLECTION_FILE << ": " << collection->size() << std::endl;
    const std::vector<word_t> *profile;
    if (argc == 2)
    {
        profile = readProfileFromFile(argv[1]);
        std::cout << argv[1] << ": " << profile->size() << std::endl;
    }
    else
    {
        profile = readProfileFromFile(PROFILE_FILE);
        std::cout << PROFILE_FILE << ": " << profile->size() << std::endl;
    }
    std::vector<unsigned int> *docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    executeCPUImplementation(collection, profile, bloomFilter, docAddresses);
    delete bloomFilter;
    delete collection;
    delete profile;
    delete docAddresses;
    return 0;
}
