#ifndef RINGBUFFER_H
#define RINGBUFFER_H
/*************************************/
/*     shared memory in a ring       */
/*           Controller              */
/*                                   */
/*Created by YK_Sam                  */
/*        at 2018-12-08              */
/*                                   */
/*Description:                       */
/*    When transferring data between */
/*    between 2 seperate programs    */
/*    with the shared memory(mmap()),*/
/*    a controller that controls the */
/*    memory usage is necessary.     */
/*    Here the program arrange the   */
/*    shared memory as a ring buffer.*/
/*************************************/

#define SIZE_128M 134217728
#define TEST_SIZE 3569000
#define SIZE_INT 4

#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

class ringBuffer
{
public:
	ringBuffer(unsigned char *headAddr, int size = SIZE_128M, int flagsBuffer = 0);
	~ringBuffer();

	void *mmapRing;
    unsigned char *head;
	int canRead(int d = 1);    //how much can read
    int canWrite(int d = 1);   //how much can write
    int readFlags(void* flag,int positionOfFlags, int count = 1);  //read the flags data, when using the flags, make sure you arrange the memory well
    int read(void *data, int count);  //take data frome ringbuffer
    int readOnly(void *data, int count);    //only read from the buffer, without changing the data in the ringbuffer
    int writeFlags(void *data, int positionOfFlags ,int count = 1);  //write the flags data, if the flag is longer than 1 byte
    int write(const void *data, int count);//write data into the buffer
    int size();

private:
	unsigned int bufferSize{NULL};       //buffer size
    /*环形缓冲区变量*/
    unsigned int rbCapacity{NULL}; //容量
    unsigned int rbHead{NULL};
    unsigned int rbTail{NULL};
    unsigned int rbBuff{NULL};
    int sizeOfFlags{0};
    int sizeOfAddr{SIZE_INT};
};

#endif