#include "mmap_ring.h"

ringBuffer::ringBuffer(unsigned char *headAddr, size_t size, int flagsBuffer){
	sizeOfAddr = SIZE_INT;
    sizeOfAddrs = 2 * SIZE_INT;
	bufferSize = size;
	sizeOfFlags = flagsBuffer;
    rbCapacity = size - sizeOfAddrs - sizeOfFlags;//开头8字节用于保存首尾位置信息，用户接触不到。再一部分空间用于储存从parent发来的flags，用户定义
    head = headAddr;
    rbBuff = sizeOfAddrs + sizeOfFlags;
    rbHead[0] = rbBuff;
    rbHead[1] = rbBuff;
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	mmapRing = mmap(headAddr, bufferSize, protection, visibility, 0, 0);

	memcpy(mmapRing, &rbHead, sizeOfAddrs);
}

ringBuffer::~ringBuffer(){
	rbBuff = NULL;
    rbCapacity = 0;
    munmap(mmapRing, bufferSize);
}

/**
 * @brief ringBuffer::rbCanRead
 * @param 是否需要更新tail和head信息，从外部访问时，参数留空即可
 * @return 缓冲区可读字节数
 */
unsigned int ringBuffer::canRead(int d)
{
	if (d)
	{
		memcpy(&rbHead, mmapRing, sizeOfAddrs);
	}
    //ring buffer is NULL, return -1
    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }

    if (rbHead[0] == rbHead[1])
    {
        return 0;
    }

    if (rbHead[0] < rbHead[1])
    {
        return rbHead[1] - rbHead[0];
    }
    return rbCapacity - (rbHead[0] - rbHead[1]);
}

/**
 * @brief ringBuffer::rbCanWrite  缓冲区剩余可写字节数
 * @param 是否需要更新tail和head信息，从外部访问时，参数留空即可
 * @return  可写字节数
 */
unsigned int ringBuffer::canWrite(int d)
{
	if (d)
	{
		memcpy(&rbHead, mmapRing, sizeOfAddrs);
	}
    return rbCapacity - canRead(0);
}

/**
 * @brief ringBuffer::readFlags 从缓冲区取出数据
 * @目标flag
 * @param 目标flag位置，从0开始计数
 * @param flag长度
 * @return 是否读取成功
 */
int ringBuffer::readFlags(void *flag, int positionOfFlags, int count)
{
	if (positionOfFlags + count > sizeOfFlags)
	{
		return -1;//invalid flag position
	}

	memcpy(flag, mmapRing + sizeOfAddrs + positionOfFlags, count);
	return 0;
}

/**
 * @brief ringBuffer::read 从缓冲区取出数据
 * @param 目标数组地址
 * @param 读的字节数
 * @return 实际读的字节数
 */
unsigned int ringBuffer::read(void *data, unsigned int count)
{
	memcpy(&rbHead, mmapRing, sizeOfAddrs);
    int copySz = 0;

    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }
    if(nullptr == data)
    {
        return -1;
    }

    if (rbHead[0] <= rbHead[1])
    {
        copySz = std::min(count, canRead(0));
        memcpy(data, mmapRing + rbHead[0], copySz);
        rbHead[0] += copySz;
        memcpy(mmapRing, rbHead, sizeOfAddr);
        return copySz;
    }
    else
    {
        if (count < rbCapacity-(rbHead[0] - rbBuff))
        {
            copySz = count;
            memcpy(data, mmapRing + rbHead[0], copySz);
            rbHead[0] += copySz;
        	memcpy(mmapRing, rbHead, sizeOfAddr);
            return copySz;
        }
        else
        {
        	copySz = std::min(count, canRead(0));
            int dataAfterHead = rbCapacity-(rbHead[0] - rbBuff);
            memcpy(data, mmapRing + rbHead[0], dataAfterHead);
            memcpy(((unsigned char *)data) + dataAfterHead, mmapRing + rbBuff, copySz - dataAfterHead);
            rbHead[0] = rbBuff + copySz - dataAfterHead;
        	memcpy(mmapRing, rbHead, sizeOfAddr);
            return copySz;
        }
    }
}

/**
* @brief ringBuffer::readOnly 从缓冲区读数据，而不改变有效数据头位置
* @param 目标数组地址
* @param 读的字节数
* @return 实际读的字节数
*/
unsigned int ringBuffer::readOnly(void *data, unsigned int count)
{
	memcpy(&rbHead, mmapRing, sizeOfAddrs);
    int copySz = 0;

    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }
    if(nullptr == data)
    {
        return -1;
    }

    if (rbHead[0] <= rbHead[1])
    {
        copySz = std::min(count, canRead(0));
        memcpy(data, mmapRing + rbHead[0], copySz);
        return copySz;
    }
    else
    {
        if (count < rbCapacity-(rbHead[0] - rbBuff))
        {
            copySz = count;
            memcpy(data, mmapRing + rbHead[0], copySz);
            return copySz;
        }
        else
        {
            copySz = std::min(count, canRead(0));
            int dataAfterHead = rbCapacity-(rbHead[0] - rbBuff);
            memcpy(data, mmapRing + rbHead[0], dataAfterHead);
            memcpy((unsigned char *)data + dataAfterHead, mmapRing + rbBuff, copySz - dataAfterHead);
            return copySz;
        }
    }
}

/**
 * @brief ringBuffer::writeFlags 从缓冲区取出数据
 * @param 目标flag位置，从0开始计数
 * @param flag长度
 * @return 是否写入成功
 */
int ringBuffer::writeFlags(void *data, int positionOfFlags, int count)
{
	if (positionOfFlags + count > sizeOfFlags)
	{
		return -1;//invalid flag position
	}

	memcpy(mmapRing + sizeOfAddrs + positionOfFlags, data, count);
	return 0;
}

/**
 * @brief ringBuffer::write
 * @param 数据地址
 * @param 要写的字节数
 * @return 实际写入的字节数
 */
unsigned int ringBuffer::write(const void *data, unsigned int count)
{
	memcpy(&rbHead, mmapRing, sizeOfAddrs);
    int tailAvailSz = 0;

    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }

    if(nullptr == data)
    {
        return -1;
    }
    
    if (count >= canWrite(0))
    {
        printf("data loss: %d\n", count);
        return -1;//缓存溢出
    }

    if (rbHead[0] <= rbHead[1])
    {
        tailAvailSz = rbCapacity - (rbHead[1] - rbBuff);
        if (count < tailAvailSz)
        {
            memcpy(mmapRing + rbHead[1], data, count);
            rbHead[1] += count;
			memcpy(mmapRing + sizeOfAddr, rbHead + 1, sizeOfAddr);
            return count;
        }
        else if (count > tailAvailSz)
        {
            memcpy(mmapRing + rbHead[1], data, tailAvailSz);
            memcpy(mmapRing + rbBuff, (unsigned char *)data + tailAvailSz, count - tailAvailSz);
            rbHead[1] = count - tailAvailSz + rbBuff;
			memcpy(mmapRing + sizeOfAddr, rbHead + 1, sizeOfAddr);
            return count;
        }
        else
        {
            memcpy(mmapRing + rbHead[1], data, tailAvailSz);
            rbHead[1] = rbBuff;
			memcpy(mmapRing + sizeOfAddr, rbHead + 1, sizeOfAddr);
            return count;
        }
    }
    else
    {
        memcpy(mmapRing + rbHead[1], data, count);
        rbHead[1] += count;
		memcpy(mmapRing + sizeOfAddr, rbHead + 1, sizeOfAddr);
        return count;
    }
}

/**
 * @brief ringBuffer::throwData
 * @param  要丢弃的字节数
 * @return   实际丢弃的字节数
 */
unsigned int ringBuffer::throwData(unsigned int count)
{
    memcpy(&rbHead, mmapRing, sizeOfAddrs);
    int copySz = 0;

    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }

    if (rbHead[0] <= rbHead[1])
    {
        copySz = std::min(count, canRead(0));
        rbHead[0] += copySz;
        memcpy(mmapRing, rbHead, sizeOfAddr);
        return copySz;
    }
    else
    {
        if (count < rbCapacity-(rbHead[0] - rbBuff))
        {
            rbHead[0] += count;
            memcpy(mmapRing, rbHead, sizeOfAddr);
            return copySz;
        }
        else
        {
            copySz = std::min(count, canRead(0));
            rbHead[0] = (rbBuff << 1) + copySz - rbCapacity - rbHead[0];
            memcpy(mmapRing, rbHead, sizeOfAddr);
            return copySz;
        }
    }
}

/**
 * @brief ringBuffer::size
 * @return 缓冲区大小
 */
unsigned int ringBuffer::size()
{
    return bufferSize;
}
