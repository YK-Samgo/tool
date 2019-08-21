CXXFLAGS = -std=c++11
CC = g++

LIB = mmap_ring.o
TARGET = shmem_test tcpTest copyTest

all: $(TARGET)

shmem_test: $(LIB)

tcpTest: $(LIB)

copyTest: $(LIB)

.PHONY: clean

clean:
	rm -f $(TARGET) *.o *~
