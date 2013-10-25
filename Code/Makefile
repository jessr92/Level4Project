CPPFLAGS=-W -Wall -pedantic -std=c++11
CFLAGS=$(CPPFLAGS)
PROGS=cpu_reference_implementation
OBJS=cpu_reference_implementation.o read_files.o

all:$(PROGS)

cpu_reference_implementation: $(OBJS)
	g++ $(CFLAGS) -o cpu_reference_implementation $(OBJS)

clean:
	rm -f $(OBJS) $(PROGS)
