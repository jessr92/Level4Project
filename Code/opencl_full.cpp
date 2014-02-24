#include <iostream>
#include <fstream>
#include <string>
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
#include <sstream>

double time_elapsed;
double startt, endt;
double totalt = 0;

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
    totalt += time_elapsed;
}

void executeFullOpenCL(const std::string *documents,
                       const std::vector<word_t> *profile,
                       const std::vector<word_t> *bloomFilter,
                       const std::vector<word_t> *positions)
{
    unsigned long hamCount = 0;
    unsigned long spamCount = 0;
    const char *docs = documents->c_str();
#ifdef DEVCPU
    char *tempDocs = new char[documents->size() + 1];
    tempDocs[documents->size()] = '\0';
    memcpy(tempDocs, docs, documents->size());
#endif
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
#ifdef DEVCPU
        platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
#elif defined(DEVACC)
        platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
#else
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
#endif
#ifdef DEVACC
        // Wim's Intel Xeon Phi is the second one.
        std::cout << "Device name: " << devices[2].getInfo<CL_DEVICE_NAME>() << std::endl;
#else
        std::cout << "Device name: " << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
#endif
        // Create a context for the devices
        cl::Context context(devices);
        // Create a command queue for the first device
#ifdef DEVACC
        cl::CommandQueue queue = cl::CommandQueue(context, devices[2]);
#else
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);
#endif
        // Create the memory buffers
        int docsSize = sizeof(char) * documents->size();
        int profileSize = sizeof(word_t) * profile->size();
        int positionsSize = sizeof(word_t) * positions->size();
        int scoresSize = sizeof(scores) * positions->at(0);
#ifdef DEVCPU
        cl::Buffer d_docs = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, docsSize, tempDocs);
        cl::Buffer d_profile = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, profileSize, tempProfile);
        cl::Buffer d_positions = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, positionsSize, tempPositions);
        cl::Buffer d_scores = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, scoresSize, scores);
#else
        cl::Buffer d_docs = cl::Buffer(context, CL_MEM_READ_ONLY, docsSize);
        cl::Buffer d_profile = cl::Buffer(context, CL_MEM_READ_ONLY, profileSize);
        cl::Buffer d_positions = cl::Buffer(context, CL_MEM_READ_ONLY, positionsSize);
        cl::Buffer d_scores = cl::Buffer(context, CL_MEM_WRITE_ONLY, scoresSize);
#endif
#ifdef BLOOM_FILTER
        int bloomFilterSize = sizeof(word_t) * bloomFilter->size();
#ifdef DEVCPU
        cl::Buffer d_bloomFilter = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, bloomFilterSize, tempBloomFilter);
#else
        cl::Buffer d_bloomFilter = cl::Buffer(context, CL_MEM_READ_ONLY, bloomFilterSize);
#endif
#endif
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
        for (int i = 0; i < REPETITIONS; i++)
        {
#ifdef DEVCPU
            queue.enqueueMapBuffer(d_profile, CL_FALSE, CL_MAP_READ, 0, profileSize);
            queue.enqueueMapBuffer(d_docs, CL_FALSE, CL_MAP_READ, 0, docsSize);
            queue.enqueueMapBuffer(d_positions, CL_FALSE, CL_MAP_READ, 0, positionsSize);
            queue.enqueueMapBuffer(d_scores, CL_FALSE, CL_MAP_WRITE, 0, scoresSize);
#else
            queue.enqueueWriteBuffer(d_profile, CL_TRUE, 0, profileSize, tempProfile);
            // Copy the input data to the input buffers using the command queue
            mark_time();
            queue.enqueueWriteBuffer(d_docs, CL_TRUE, 0, docsSize, docs);
            queue.enqueueWriteBuffer(d_positions, CL_TRUE, 0, positionsSize, tempPositions);
            queue.enqueueWriteBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
            stop_time();
            std::cout << time_elapsed << " seconds to copy data HtoD." << std::endl;
#endif
#ifdef BLOOM_FILTER
#ifdef DEVCPU
            queue.enqueueMapBuffer(d_bloomFilter, CL_FALSE, CL_MAP_READ, 0, bloomFilterSize);
#else
            queue.enqueueWriteBuffer(d_bloomFilter, CL_TRUE, 0, bloomFilterSize, tempBloomFilter);
#endif
#endif
            // Execute the kernel
            mark_time();
#ifdef DEVCPU
            int localSize = 1;
#elif defined(DEVACC)
            int localSize = 4;
#else
            int localSize = 128;
#endif
            int globalSize = positions->at(0);
            if (globalSize % localSize != 0)
            {
                globalSize += localSize - (globalSize % localSize);
            }
            cl::NDRange global(globalSize);
            cl::NDRange local(localSize);
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
#ifndef DEVCPU
            queue.enqueueReadBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
#endif
            queue.finish();
            stop_time();
        }
        std::cout << time_elapsed << " seconds to run kernel and get scores back." << std::endl;
    }
    catch (cl::Error error)
    {
        std::cout << error.what() << "(" << clErrorString(error.err()) << ")" << std::endl;
        return;
    }
    //return;
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
#ifdef HTML_PARSE
    const std::string *docs = readFile(HTML_FILE);
#else
    const std::string *docs = readFile(DOCUMENT_FILE);
#endif
    const std::vector<word_t> *positions;
    mark_time();
    for (int i = 0; i < REPETITIONS; i++)
    {
        positions = getMarkerPositions(docs);
    }
    stop_time();
    std::cout << time_elapsed << " seconds to get marker positions." << std::endl;
    executeFullOpenCL(docs, profile, bloomFilter, positions);
    std::cout << totalt << " seconds to score documents." << std::endl;
    return 0;
}
