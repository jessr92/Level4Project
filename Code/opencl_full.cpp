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

std::string decode(unsigned long term)
{
    std::ostringstream outs;
    char letters[12];
    unsigned termSize = term & 0xFU;
    for (unsigned i = 0; i < termSize; ++i)
    {
        letters[i] = (term >> (i * 5 + 4) & 0x1FUL);
        // is letter?
        if ((letters[i] > 9) && (letters[i] < 10 + 22 + 1 ))  // WV: this is wrong! it's 22 I think
        {
            letters[i] += (letters[i] > 17) ? ((letters[i] > 21) ? ((letters[i] > 27) ? 4 : 3) : 2) : 0;
            letters[i] += 65 - 10;
            // is digit?
        }
        else
        {
            letters[i] = letters[i] + 48;
        }
        outs << letters[i];
    }
    //  std::cout << std::endl;
    //std::cout << "size: " << termSize << std::endl;
    std::string os = outs.str();
    return os;
}

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
    int nextState[25] =
    {
        WRITING,    SKIPPING,   FLUSHING,   INSIDE_TAG, SKIPPING,
        WRITING,    SKIPPING,   FLUSHING,   INSIDE_TAG, SKIPPING,
        WRITING,    SKIPPING,   SKIPPING,   INSIDE_TAG, SKIPPING,
        INSIDE_TAG, INSIDE_TAG, INSIDE_TAG, INSIDE_TAG, SKIPPING,
        WRITING,    SKIPPING,   SKIPPING,   SKIPPING,   SKIPPING
    };
    try
    {
        // Get the available platforms.
        cl::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        // Get a list of devices on this platform.
        cl::vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
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
        int stateSize = sizeof(nextState) * 25;
        cl::Buffer d_state = cl::Buffer(context, CL_MEM_READ_ONLY, stateSize);
        int docLength = positions->at(2) - positions->at(1);
        unsigned long *result = new unsigned long[docLength];
        for (int i = 0; i < docLength; i++)
        {
            result[i] = 0;
        }
        int resultSize = sizeof(result) * docLength;
        cl::Buffer d_result = cl::Buffer(context, CL_MEM_WRITE_ONLY, resultSize);
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
#else
        kernel.setArg(4, d_result);
#endif
        kernel.setArg(5, d_state);
        queue.enqueueWriteBuffer(d_profile, CL_TRUE, 0, profileSize, tempProfile);
        // Copy the input data to the input buffers using the command queue
        mark_time();
        queue.enqueueWriteBuffer(d_docs, CL_TRUE, 0, docsSize, docs);
        queue.enqueueWriteBuffer(d_positions, CL_TRUE, 0, positionsSize, tempPositions);
        queue.enqueueWriteBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
        queue.enqueueWriteBuffer(d_state, CL_TRUE, 0, stateSize, nextState);
        stop_time();
        std::cout << time_elapsed << " seconds to copy data HtoD." << std::endl;
#ifdef BLOOM_FILTER
        queue.enqueueWriteBuffer(d_bloomFilter, CL_TRUE, 0, bloomFilterSize, tempBloomFilter);
#endif
        // Execute the kernel
        mark_time();
        cl::NDRange global(positions->at(0));
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
        queue.enqueueReadBuffer(d_scores, CL_TRUE, 0, scoresSize, scores);
        queue.enqueueReadBuffer(d_result, CL_TRUE, 0, resultSize, result);
        for (int i = 0; i < docLength; i++)
        {
            std::cout << decode(result[i]) << " ";
        }
        queue.finish();
        stop_time();
        std::cout << time_elapsed << " seconds to run kernel and get scores back." << std::endl;

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
    mark_time();
    const std::vector<word_t> *positions = getMarkerPositions(docs);
    stop_time();
    std::cout << time_elapsed << " seconds to get marker positions." << std::endl;
    executeFullOpenCL(docs, profile, bloomFilter, positions);
    std::cout << totalt << " seconds to score documents." << std::endl;
    return 0;
}
