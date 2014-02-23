CFLAGS=-W -Wall -O2
CFLAGS+=-Wno-deprecated -Wno-unused-parameter -Wno-deprecated-declarations
CFLAGS+=-Wno-ignored-qualifiers
CFLAGS+=-I/opt/AMDAPP/include
CXXFLAGS=$(CFLAGS) -std=c++11

PROGS=cpu_reference_implementation
PROGS+=gpu_implementation_no_wrapper
PROGS+=cpu_implementation_no_wrapper
PROGS+=cpu_reference_implementation_mt
PROGS+=opencl_full
PROGS+=opencl_full_fork
PROGS+=clean

OPENCL_LIB=-lOpenCL -L/opt/AMDAPP/lib/x86_64

STANDARD_OBJS=read_files.o OpenCLUtils.o
CPU_OBJS=cpu_reference_implementation.o $(STANDARD_OBJS)
CPU_NO_WRAPPER_OBJS=cpu_implementation_no_wrapper.o $(STANDARD_OBJS)
CPU_MT_OBJS=cpu_reference_implementation_mt.o $(STANDARD_OBJS)
GPU_NO_WRAPPER_OBJS=gpu_implementation_no_wrapper.o $(STANDARD_OBJS)
DUMP_CL_OBJS=dumpcl.o
OPENCL_FULL_OBJS=opencl_full.o $(STANDARD_OBJS)
OPENCL_FULL_FORK_OBJS=opencl_full_fork.o $(STANDARD_OBJS)
OBJS = $(CPU_OBJS) $(GPU_NO_WRAPPER_OBJS) $(CPU_NO_WRAPPER_OBJS) $(CPU_MT_OBJS)
OBJS+= $(DUMP_CL_OBJS) $(OPENCL_FULL_OBJS) $(OPENCL_FULL_FORK_OBJS)

CPU_OBJS_LIB=$(CPU_OBJS) -lpthread
CPU_MT_OBJS_LIB=$(CPU_MT_OBJS) -lpthread
CPU_NO_WRAPPER_OBJS_LIB=$(CPU_NO_WRAPPER_OBJS) -lpthread $(OPENCL_LIB)
GPU_NO_WRAPPER_OBJS_LIB=$(GPU_NO_WRAPPER_OBJS) -lpthread $(OPENCL_LIB)
DUMP_CL_OBJS_LIB=$(DUMP_CL_OBJS) -lpthread $(OPENCL_LIB)
OPENCL_FULL_OBJS_LIB=$(OPENCL_FULL_OBJS) -lpthread $(OPENCL_LIB)
OPENCL_FULL_FORK_OBJS_LIB=$(OPENCL_FULL_FORK_OBJS) -lpthread $(OPENCL_LIB)

ifeq "$(shell uname)" "Darwin"
	CFLAGS+=-framework OpenCL
	GPU_NO_WRAPPER_OBJS_LIB=$(GPU_NO_WRAPPER_OBJS)
	CPU_NO_WRAPPER_OBJS_LIB=$(CPU_NO_WRAPPER_OBJS)
	DUMP_CL_OBJS_LIB=$(DUMP_CL_OBJS)
	OPENCL_FULL_OBJS_LIB=$(OPENCL_FULL_OBJS)
	OPENCL_FULL_FORK_OBJS_LIB=$(OPENCL_FULL_FORK_OBJS)
endif

ifeq "$(shell hostname)" "fara.dcs.gla.ac.uk"
	CC = gcc-mp-4.8
	CXX = g++-mp-4.8
endif

all: $(PROGS)

cpu_reference_implementation: $(CPU_OBJS)
	$(CXX) $(CXXFLAGS) -o cpu_reference_implementation $(CPU_OBJS_LIB)

gpu_implementation_no_wrapper: $(GPU_NO_WRAPPER_OBJS)
	$(CXX) $(CXXFLAGS) -o gpu_implementation_no_wrapper $(GPU_NO_WRAPPER_OBJS_LIB)

cpu_implementation_no_wrapper: $(CPU_NO_WRAPPER_OBJS)
	$(CXX) $(CXXFLAGS) -o cpu_implementation_no_wrapper $(CPU_NO_WRAPPER_OBJS_LIB)

cpu_reference_implementation_mt: $(CPU_MT_OBJS)
	$(CXX) $(CXXFLAGS) -o cpu_reference_implementation_mt $(CPU_MT_OBJS_LIB)

dumpcl: $(DUMP_CL_OBJS)
	$(CC) $(CFLAGS) -o dumpcl $(DUMP_CL_OBJS_LIB)

opencl_full: $(OPENCL_FULL_OBJS)
	$(CXX) $(CXXFLAGS) -o opencl_full $(OPENCL_FULL_OBJS_LIB)

opencl_full_fork: $(OPENCL_FULL_FORK_OBJS)
	$(CXX) $(CXXFLAGS) -o opencl_full_fork $(OPENCL_FULL_FORK_OBJS_LIB)

clean:
	rm -f $(OBJS)
