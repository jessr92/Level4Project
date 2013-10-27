/*
 OpenCL Utility functions
 */

#ifndef CLUTILS_H_INC
#define CLUTILS_H_INC

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

/* Returns a string representing the simple error message associated with the cl_int */
const char *clErrorString(cl_int err);

#endif /* CLUTILS_H_INC */
