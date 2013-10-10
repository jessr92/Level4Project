CPPFLAGS=-W -Wall -pedantic -std=c++11
CFLAGS=$(CPPFLAGS) -framework OpenCL
PROGS=test_reading_files
OBJS=test_reading_files.o

all:$(PROGS)

test_reading_files: $(OBJS)
	g++ $(CFLAGS) -o test_reading_files $(OBJS)

clean:
	rm -f $(OBJS) $(PROGS)
