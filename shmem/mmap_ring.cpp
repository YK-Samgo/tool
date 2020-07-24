#include "mmap_ring.h"

ringBuffer::ringBuffer(unsigned char *headAddr, size_t size, int flagsBuffer){
	bufferSize = size;
	sizeOfFlags = flagsBuffer;
    rbCapacity = size - sizeOfFlags;//一部分空间用于储存从parent发来的flags，用户定义
    head = headAddr;
    rbBuff = sizeOfFlags;
    rbHead[0] = rbBuff;
    rbHead[1] = rbBuff;
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	mmapRing = mmap(headAddr, bufferSize, protection, visibility, 0, 0);
}

ringBuffer::~ringBuffer(){
	rbBuff = NULL;
    rbCapacity = 0;
    munmap(mmapRing, bufferSize);
}

/**
 * @brief ringBuffer::rbCanRead
 * @return 缓冲区可读字节数
 */
unsigned int ringBuffer::canRead()
{
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
 * @return  可写字节数
 */
unsigned int ringBuffer::canWrite()
{
    return rbCapacity - canRead();
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

	memcpy(flag, mmapRing + positionOfFlags, count);
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
        copySz = std::min(count, canRead());
        memcpy(data, mmapRing + rbHead[0], copySz);
        rbHead[0] += copySz;
        return copySz;
    }
    else
    {
        if (count < bufferSize - rbHead[0])
        {
            copySz = count;
            memcpy(data, mmapRing + rbHead[0], copySz);
            rbHead[0] += copySz;
            return copySz;
        }
        else
        {
        	copySz = std::min(count, canRead());
            int dataAfterHead = rbCapacity-(rbHead[0] - rbBuff);
            memcpy(data, mmapRing + rbHead[0], dataAfterHead);
            memcpy(((unsigned char *)data) + dataAfterHead, mmapRing + rbBuff, copySz - dataAfterHead);
            rbHead[0] = rbBuff + copySz - dataAfterHead;
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
        copySz = std::min(count, canRead());
        memcpy(data, mmapRing + rbHead[0], copySz);
        return copySz;
    }
    else
    {
        if (count < bufferSize - rbHead[0])
        {
            copySz = count;
            memcpy(data, mmapRing + rbHead[0], copySz);
            return copySz;
        }
        else
        {
            copySz = std::min(count, canRead());
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

	memcpy(mmapRing + positionOfFlags, data, count);
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
    int tailAvailSz = 0;

    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }

    if(nullptr == data)
    {
        return -1;
    }
    
    if (count >= canWrite())
    {
        printf("data loss: %d\n", count);
        return -1;//缓存溢出
    }

    if (rbHead[0] <= rbHead[1])
    {
        tailAvailSz = bufferSize - rbHead[0];
        if (count < tailAvailSz)
        {
            memcpy(mmapRing + rbHead[1], data, count);
            rbHead[1] += count;
            return count;
        }
        else if (count > tailAvailSz)
        {
            memcpy(mmapRing + rbHead[1], data, tailAvailSz);
            memcpy(mmapRing + rbBuff, (unsigned char *)data + tailAvailSz, count - tailAvailSz);
            rbHead[1] = count - tailAvailSz + rbBuff;
            return count;
        }
        else
        {
            memcpy(mmapRing + rbHead[1], data, tailAvailSz);
            rbHead[1] = rbBuff;
            return count;
        }
    }
    else
    {
        memcpy(mmapRing + rbHead[1], data, count);
        rbHead[1] += count;
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
    int copySz = 0;

    if((NULL == rbBuff)||(NULL == rbHead[0])||(NULL == rbHead[1]))
    {
        return -1;
    }

    if (rbHead[0] <= rbHead[1])
    {
        copySz = std::min(count, canRead());
        rbHead[0] += copySz;
        return copySz;
    }
    else
    {
        if (count < rbCapacity-(rbHead[0] - rbBuff))
        {
            rbHead[0] += count;
            return copySz;
        }
        else
        {
            copySz = std::min(count, canRead());
            rbHead[0] = copySz + rbHead[0] - rbCapacity;
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
