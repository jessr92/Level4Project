CFLAGS=-W -Wall -pedantic -std=c++11 -O2
CXXFLAGS=$(CFLAGS)

PROGS=cpu_reference_implementation
PROGS+=gpu_implementation_no_wrapper
PROGS+=cpu_implementation_no_wrapper
PROGS+=dumpcl
PROGS+=clean

STANDARD_OBJS=read_files.o OpenCLUtils.o
CPU_OBJS=cpu_reference_implementation.o $(STANDARD_OBJS)
CPU_NO_WRAPPER_OBJS=cpu_implementation_no_wrapper.o $(STANDARD_OBJS)
GPU_NO_WRAPPER_OBJS=gpu_implementation_no_wrapper.o $(STANDARD_OBJS)
DUMP_CL_OBJS=dumpcl.o
OBJS = $(CPU_OBJS) $(GPU_NO_WRAPPER_OBJS) $(CPU_NO_WRAPPER_OBJS) $(DUMP_CL_OBJS)

CPU_NO_WRAPPER_OBJS_LIB=$(CPU_NO_WRAPPER_OBJS) -lOpenCL
GPU_NO_WRAPPER_OBJS_LIB=$(GPU_NO_WRAPPER_OBJS) -lOpenCL
DUMP_CL_OBJS_LIB=$(DUMP_CL_OBJS) -lOpenCL

ifeq "$(shell uname)" "Darwin"
	CFLAGS+=-framework OpenCL
	GPU_NO_WRAPPER_OBJS_LIB=$(GPU_NO_WRAPPER_OBJS)
	CPU_NO_WRAPPER_OBJS_LIB=$(CPU_NO_WRAPPER_OBJS)
	DUMP_CL_OBJS_LIB=$(DUMP_CL_OBJS)
endif

all: $(PROGS)

cpu_reference_implementation: $(CPU_OBJS)
	$(CXX) $(CFLAGS) -o cpu_reference_implementation $(CPU_OBJS)

gpu_implementation_no_wrapper: $(GPU_NO_WRAPPER_OBJS)
	$(CXX) $(CFLAGS) -o gpu_implementation_no_wrapper $(GPU_NO_WRAPPER_OBJS_LIB)

cpu_implementation_no_wrapper: $(CPU_NO_WRAPPER_OBJS)
	$(CXX) $(CFLAGS) -o cpu_implementation_no_wrapper $(CPU_NO_WRAPPER_OBJS_LIB)

dumpcl: $(DUMP_CL_OBJS)
	$(CXX) $(CFLAGS) -o dumpcl $(DUMP_CL_OBJS_LIB)

clean:
	rm -f $(OBJS)
