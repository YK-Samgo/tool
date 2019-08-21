CXXFLAGS = -std=c++11
CC = g++

LIB = mmap_ring.o
TARGET = tcpTest tcpServer

all: $(TARGET)

tcpTest: $(LIB)

tcpServer: $(LIB)

.PHONY: clean

clean:
	rm -f $(TARGET) *.o *~
