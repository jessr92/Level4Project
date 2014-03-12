#include <iostream>
#include <fstream>
#include <vector>
#include "read_files.h"
#include "shared_details.h"
#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS
//#ifdef __APPLE__
#include "cl.hpp"
//#else
//#include <CL/cl.hpp>
//#endif
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

void executeGPUImplementation(const std::vector<word_t> *collection,
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
#ifdef DEVACC
        platforms[0].getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
        // Wim's Intel Xeon Phi is the second one.
        std::cout << "Device name: " << devices[1].getInfo<CL_DEVICE_NAME>() << std::endl;
#else
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
        std::cout << "Device name: " << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;

#endif
        // Create a context for the devices
        cl::Context context(devices);
        // Create a command queue for the first device
#ifdef DEVACC
        cl::CommandQueue queue = cl::CommandQueue(context, devices[1]);
#else
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);
#endif
        // Create the memory buffers
        int collectionSize = sizeof(word_t) * collection->size();
        cl::Buffer d_collection = cl::Buffer(context, CL_MEM_READ_ONLY, collectionSize);
        int profileSize = sizeof(word_t) * profile->size();
        cl::Buffer d_profile = cl::Buffer(context, CL_MEM_READ_ONLY, profileSize);
        int docAddressesSize = sizeof(unsigned int) * docAddresses->size();
        cl::Buffer d_docAddresses = cl::Buffer(context, CL_MEM_READ_ONLY, docAddressesSize);
#ifdef BLOOM_FILTER
        int bloomFilterSize = sizeof(word_t) * bloomFilter->size();
        cl::Buffer d_bloomFilter = cl::Buffer(context, CL_MEM_READ_ONLY, bloomFilterSize);
#endif
        int scoresSize = sizeof(scores) * docAddresses->at(0);
        cl::Buffer d_scores = cl::Buffer(context, CL_MEM_WRITE_ONLY, scoresSize);
        // Read the program source
        std::ifstream sourceFile(KERNEL_FILE);
        std::string sourceCode(std::istreambuf_iterator < char > (sourceFile), (std::istreambuf_iterator < char > ()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
        // Make program from the source code
        cl::Program program = cl::Program(context, source);
        // Build the program for the devices
#ifdef BLOOM_FILTER
        program.build(devices, "-DBLOOM_FILTER");
#else
        program.build(devices);
#endif
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
        queue.enqueueWriteBuffer(d_profile, CL_TRUE, 0, profileSize, tempProfile);
        // Copy the input data to the input buffers using the command queue
        mark_time();
        for (int i = 0; i < REPETITIONS; i++)
        {
            queue.enqueueWriteBuffer(d_collection, CL_TRUE, 0, collectionSize, tempCollection);
            queue.enqueueWriteBuffer(d_docAddresses, CL_TRUE, 0, docAddressesSize, tempDocAddresses);
            queue.enqueueWriteBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
#ifdef BLOOM_FILTER
            queue.enqueueWriteBuffer(d_bloomFilter, CL_TRUE, 0, bloomFilterSize, tempBloomFilter);
#endif
            // Execute the kernel
            int globalSize = (docAddresses->at(0) / DOCS_PER_THREAD) + 1;
#ifdef DEVACC
            int localSize = 4;
            if (globalSize % localSize != 0)
            {
                globalSize += localSize - (globalSize % localSize);
            }
            cl::NDRange global(globalSize);
            cl::NDRange local(localSize);
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
#else
            cl::NDRange global(globalSize);
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
#endif
            queue.enqueueReadBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
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
    executeGPUImplementation(collection, profile, bloomFilter, docAddresses);
    delete bloomFilter;
    delete collection;
    delete profile;
    delete docAddresses;
    return 0;
}
