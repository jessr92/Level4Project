CFLAGS=-W -Wall -pedantic -std=c++11 -framework OpenCL -g
CXXFLAGS=$(CFLAGS)

PROGS=cpu_reference_implementation gpu_implementation_no_wrapper

CPU_OBJS=cpu_reference_implementation.o read_files.o
GPU_NO_WRAPPER_OBJS=gpu_implementation_no_wrapper.o read_files.o OpenCLUtils.o
OBJS = $(CPU_OBJS) $(GPU_NO_WRAPPER_OBJS)

all: $(PROGS)

cpu_reference_implementation: $(CPU_OBJS)
	$(CXX) $(CFLAGS) -o cpu_reference_implementation $(CPU_OBJS)

gpu_implementation_no_wrapper: $(GPU_NO_WRAPPER_OBJS)
	$(CXX) $(CFLAGS) -o gpu_implementation_no_wrapper $(GPU_NO_WRAPPER_OBJS)

clean:
	rm -f $(OBJS) $(PROGS)
