#include <iostream>
#include <fstream>
#include <string>
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

void executeFullOpenCL(const std::string *documents,
                       const std::vector<word_t> *profile,
                       const std::vector<word_t> *bloomFilter,
                       const std::vector<word_t> *positions)
{
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
    const char *docs = documents->c_str();
    unsigned long *scores = new unsigned long[positions->at(0)];
    for (word_t i = 0; i < positions->at(0); i++)
    {
        scores[i] = 0;
    }
    std::cout << std::endl;
    word_t *tempProfile = new word_t[profile->size()];
    std::copy(profile->begin(), profile->end(), tempProfile);
    word_t *tempPositions = new word_t[positions->size()];
    std::copy(positions->begin(), positions->end(), tempPositions);
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
        int docsSize = sizeof(char) * documents->size();
        cl::Buffer d_docs = cl::Buffer(context, CL_MEM_READ_ONLY, docsSize);
        int profileSize = sizeof(word_t) * profile->size();
        cl::Buffer d_profile = cl::Buffer(context, CL_MEM_READ_ONLY, profileSize);
        int positionsSize = sizeof(word_t) * positions->size();
        cl::Buffer d_positions = cl::Buffer(context, CL_MEM_READ_ONLY, positionsSize);
#ifdef BLOOM_FILTER
        int bloomFilterSize = sizeof(word_t) * bloomFilter->size();
        cl::Buffer d_bloomFilter = cl::Buffer(context, CL_MEM_READ_ONLY, bloomFilterSize);
#endif
        int scoresSize = sizeof(scores) * positions->at(0);
        cl::Buffer d_scores = cl::Buffer(context, CL_MEM_WRITE_ONLY, scoresSize);
        // Read the program source
        std::ifstream sourceFile(KERNEL_FULL_FILE);
        std::string sourceCode(std::istreambuf_iterator < char > (sourceFile), (std::istreambuf_iterator < char > ()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
        // Make program from the source code
        cl::Program program = cl::Program(context, source);
        // Build the program for the devices
        program.build(devices);
        // Make kernel
        cl::Kernel kernel(program, KERNEL_FULL_NAME);
        // Set the kernel arguments
        kernel.setArg(0, d_docs);
        kernel.setArg(1, d_profile);
        kernel.setArg(2, d_positions);
        kernel.setArg(3, d_scores);
#ifdef BLOOM_FILTER
        kernel.setArg(4, d_bloomFilter);
#endif
        queue.enqueueWriteBuffer(d_profile, CL_TRUE, 0, profileSize, tempProfile);
        // Copy the input data to the input buffers using the command queue
        queue.enqueueWriteBuffer(d_docs, CL_TRUE, 0, docsSize, docs);
        queue.enqueueWriteBuffer(d_positions, CL_TRUE, 0, positionsSize, tempPositions);
        queue.enqueueWriteBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
#ifdef BLOOM_FILTER
        queue.enqueueWriteBuffer(d_bloomFilter, CL_TRUE, 0, bloomFilterSize, tempBloomFilter);
#endif
        // Execute the kernel
        cl::NDRange global(positions->at(0));
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
        queue.enqueueReadBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
        queue.finish();
    }
    catch (cl::Error error)
    {
        std::cout << error.what() << "(" << clErrorString(error.err()) << ")" << std::endl;
        return;
    }
    for (word_t i = 0; i < positions->at(0); ++i)
    {
        scores[i] > THRESHOLD ? ++spamCount : ++hamCount;
        std::cout << positions->at(i + 1) << ":" << scores[i] << std::endl;
    }
    std::cout << "Ham documents: " << hamCount << std::endl;
    std::cout << "Spam documents " << spamCount << std::endl;
}

int main(int argc, char *argv[])
{
    const std::vector<word_t> *bloomFilter = loadParsedCollectionFromFile(BLOOM_FILTER_FILE);
    std::cout << BLOOM_FILTER_FILE << ": " << bloomFilter->size() << std::endl;
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
    // Read in document file and find out where the documents start.
    const std::string *docs = readFile(DOCUMENT_FILE);
    const std::vector<word_t> *positions = getMarkerPositions(docs);
    executeFullOpenCL(docs, profile, bloomFilter, positions);
    return 0;
}
