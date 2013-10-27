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
#include "OpenCLUtils.h"

#define THRESHOLD 0

void executeGPUImplementation(const std::vector<word_t> *collection,
                              const std::vector<word_t> *profile,
                              std::vector<unsigned int> *docAddresses,
                              const std::vector<word_t> *bloomFilter)
{
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
    unsigned long *scores = new unsigned long[docAddresses->at(0)];
    for (word_t i = 0; i < docAddresses->at(0); i++)
    {
        scores[i] = 0;
    }
    // No idea why I need this but it'll crash otherwise.
    word_t *tempProfile = new word_t[profile->size()];
    std::copy(profile->begin(), profile->end(), tempProfile);
    try
    {
        // Get the available platforms.
        cl::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        // Get a list of devices on this platform.
        cl::vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
        std::cout << "Device name: " << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "Device max mem alloc size: " << devices[0].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
        // Create a context for the devices
        cl::Context context(devices);
        // Create a command queue for the first device
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);
        // Create the memory buffers
        int collectionSize = sizeof(collection) * collection->size();
        cl::Buffer d_collection = cl::Buffer(context, CL_MEM_READ_ONLY, collectionSize);
        int profileSize = sizeof(profile) * profile->size();
        std::cout << "Profile size: " << profileSize << std::endl;
        cl::Buffer d_profile = cl::Buffer(context, CL_MEM_READ_ONLY, profileSize);
        int docAddressesSize = sizeof(docAddresses) * docAddresses->size();
        cl::Buffer d_docAddresses = cl::Buffer(context, CL_MEM_READ_ONLY, docAddressesSize);
#ifdef BLOOM_FILTER
        int bloomFilterSize = sizeof(bloomFilter) * bloomFiler->size();
        cl::Buffer d_bloomFilter = cl::Buffer(context, CL_MEM_READ_ONLY, bloomFilterSize);
#endif
        int scoresSize = sizeof(scores) * docAddresses->at(0);
        cl::Buffer d_scores = cl::Buffer(context, CL_MEM_WRITE_ONLY, scoresSize);
        // Copy the input data to the input buffers using the command queue
        queue.enqueueWriteBuffer(d_collection, CL_TRUE, 0, collectionSize, collection);
        queue.enqueueWriteBuffer(d_profile, CL_TRUE, 0, profileSize, tempProfile);
        queue.enqueueWriteBuffer(d_docAddresses, CL_TRUE, 0, docAddressesSize, docAddresses);
#ifdef BLOOM_FILTER
        queue.enqueueWriteBuffer(d_bloomFilter, CL_TRUE, 0, bloomFilterSize, bloomFilter);
#endif
        queue.enqueueWriteBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
        // Read the program source
        std::ifstream sourceFile(KERNEL_FILE);
        std::string sourceCode(std::istreambuf_iterator < char > (sourceFile),
                               (std::istreambuf_iterator < char > ()));
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
#ifdef BLOOM_FILTER
        kernel.setArg(3, d_bloomFilter);
        kernel.setArg(4, d_scores);
#else
        kernel.setArg(3, d_scores);
#endif
        // Execute the kernel
        cl::NDRange global(docAddresses->at(0));
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
        // Copy the output data back to the host
        std::cout << "Just about to read scores" << std::endl;
        queue.enqueueReadBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
        std::cout << "Read scores" << std::endl;
    }
    catch (cl::Error error)
    {
        std::cout << error.what() << "(" << clErrorString(error.err()) << ")" << std::endl;
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

int main()
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile(BLOOM_FILTER_FILE);
    std::cout << BLOOM_FILTER_FILE << ": " << bloomFilter->size() << std::endl;
    const std::vector<word_t> *collection = loadParsedCollectionFromFile(COLLECTION_FILE);
    std::cout << COLLECTION_FILE << ": " << collection->size() << std::endl;
    const std::vector<word_t> *profile = readProfileFromFile(PROFILE_FILE);
    std::cout << PROFILE_FILE << ": " << profile->size() << std::endl;
    std::vector<unsigned int> *docAddresses = getDocumentAddresses(collection);
    std::cout << "docAddresses: " << docAddresses->at(0) << std::endl;
    executeGPUImplementation(collection, profile, docAddresses, bloomFilter);
    delete bloomFilter;
    delete collection;
    delete profile;
    delete docAddresses;
    return 0;
}
