CFLAGS=-W -Wall -pedantic -std=c++11 -O2
CXXFLAGS=$(CFLAGS)

PROGS=cpu_reference_implementation gpu_implementation_no_wrapper cpu_implementation_no_wrapper clean

CPU_OBJS=cpu_reference_implementation.o read_files.o
CPU_NO_WRAPPER_OBJS=cpu_implementation_no_wrapper.o read_files.o OpenCLUtils.o
GPU_NO_WRAPPER_OBJS=gpu_implementation_no_wrapper.o read_files.o OpenCLUtils.o
CPU_NO_WRAPPER_OBJS_LIB=$(CPU_NO_WRAPPER_OBJS) -lOpenCL
GPU_NO_WRAPPER_OBJS_LIB=$(GPU_NO_WRAPPER_OBJS) -lOpenCL
OBJS = $(CPU_OBJS) $(GPU_NO_WRAPPER_OBJS) $(CPU_NO_WRAPPER_OBJS)

ifeq "$(shell uname)" "Darwin"
CFLAGS+=-framework OpenCL
GPU_NO_WRAPPER_OBJS_LIB=$(GPU_NO_WRAPPER_OBJS)
CPU_NO_WRAPPER_OBJS_LIB=$(CPU_NO_WRAPPER_OBJS)
endif


all: $(PROGS)

cpu_reference_implementation: $(CPU_OBJS)
	$(CXX) $(CFLAGS) -o cpu_reference_implementation $(CPU_OBJS)

gpu_implementation_no_wrapper: $(GPU_NO_WRAPPER_OBJS)
	$(CXX) $(CFLAGS) -o gpu_implementation_no_wrapper $(GPU_NO_WRAPPER_OBJS_LIB)

cpu_implementation_no_wrapper: $(CPU_NO_WRAPPER_OBJS)
	$(CXX) $(CFLAGS) -o cpu_implementation_no_wrapper $(CPU_NO_WRAPPER_OBJS_LIB)

clean:
	rm -f $(OBJS)
