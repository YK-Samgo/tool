#include "mmap_ring.h"

ringBuffer::ringBuffer(unsigned char *headAddr, int size, int flagsBuffer){
	sizeOfAddr = SIZE_INT;
	bufferSize = size;
	sizeOfFlags = flagsBuffer;
    rbCapacity = size - 2 * sizeOfAddr - sizeOfFlags;//开头8字节用于保存首尾位置信息，用户接触不到。再一部分空间用于储存从parent发来的flags，用户定义
    head = headAddr;
    rbBuff = 2 * sizeOfAddr + sizeOfFlags;
    rbHead = rbBuff;
    rbTail = rbBuff;
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	mmapRing = mmap(headAddr, bufferSize, protection, visibility, 0, 0);

	memcpy(mmapRing, &rbHead, sizeOfAddr);
	memcpy(mmapRing + sizeOfAddr, &rbTail, sizeOfAddr);
}

ringBuffer::~ringBuffer(){
	rbBuff = NULL;
    rbHead = NULL;
    rbTail = NULL;
    rbCapacity = 0;
    munmap(head, bufferSize);
}

/**
 * @brief ringBuffer::rbCanRead
 * @param 是否需要更新tail和head信息，从外部访问时，参数留空即可
 * @return 缓冲区可读字节数
 */
int ringBuffer::canRead(int d)
{
	if (d)
	{
		memcpy(&rbTail, mmapRing + sizeOfAddr, sizeOfAddr);
		memcpy(&rbHead, mmapRing, sizeOfAddr);
	}
    //ring buffer is NULL, return -1
    if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
    {
        return -1;
    }

    if (rbHead == rbTail)
    {
        return 0;
    }

    if (rbHead < rbTail)
    {
        return rbTail - rbHead;
    }
    return rbCapacity - (rbHead - rbTail);
}

/**
 * @brief ringBuffer::rbCanWrite  缓冲区剩余可写字节数
 * @param 是否需要更新tail和head信息，从外部访问时，参数留空即可
 * @return  可写字节数
 */
int ringBuffer::canWrite(int d)
{
	if (d)
	{
		memcpy(&rbHead, mmapRing, sizeOfAddr);
		memcpy(&rbTail, mmapRing + sizeOfAddr, sizeOfAddr);
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

	memcpy(flag, mmapRing + 2 * sizeOfAddr + positionOfFlags, count);
	return 0;
}

/**
 * @brief ringBuffer::read 从缓冲区取出数据
 * @param 目标数组地址
 * @param 读的字节数
 * @return 实际读的字节数
 */
int ringBuffer::read(void *data, int count)
{
	memcpy(&rbTail, mmapRing + sizeOfAddr, sizeOfAddr);
	memcpy(&rbHead, mmapRing, sizeOfAddr);
    int copySz = 0;

    if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
    {
        return -1;
    }
    if(nullptr == data)
    {
        return -1;
    }

    if (rbHead <= rbTail)
    {
        copySz = std::min(count, canRead(0));
        memcpy(data, mmapRing + rbHead, copySz);
        rbHead += copySz;
        memcpy(mmapRing, &rbHead, sizeOfAddr);
        return copySz;
    }
    else
    {
        if (count < rbCapacity-(rbHead - rbBuff))
        {
            copySz = count;
            memcpy(data, mmapRing + rbHead, copySz);
            rbHead += copySz;
        	memcpy(mmapRing, &rbHead, sizeOfAddr);
            return copySz;
        }
        else
        {
        	copySz = std::min(count, canRead(0));
            int dataAfterHead = rbCapacity-(rbHead - rbBuff);
            memcpy(data, mmapRing + rbHead, dataAfterHead);
            memcpy((unsigned char *)data + dataAfterHead, mmapRing + rbBuff, copySz - dataAfterHead);
            rbHead = rbBuff + copySz - dataAfterHead;
        	memcpy(mmapRing, &rbHead, sizeOfAddr);
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
int ringBuffer::readOnly(void *data, int count)
{
	memcpy(&rbTail, mmapRing + sizeOfAddr, sizeOfAddr);
	memcpy(&rbHead, mmapRing, sizeOfAddr);
    int copySz = 0;

    if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
    {
        return -1;
    }
    if(nullptr == data)
    {
        return -1;
    }

    if (rbHead <= rbTail)
    {
        copySz = std::min(count, canRead(0));
        memcpy(data, mmapRing + rbHead, copySz);
        return copySz;
    }
    else
    {
        if (count < rbCapacity-(rbHead - rbBuff))
        {
            copySz = count;
            memcpy(data, mmapRing + rbHead, copySz);
            return copySz;
        }
        else
        {
            copySz = std::min(count, canRead(0));
            int dataAfterHead = rbCapacity-(rbHead - rbBuff);
            memcpy(data, mmapRing + rbHead, dataAfterHead);
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

	memcpy(mmapRing + 2 * sizeOfAddr + positionOfFlags, data, count);
	return 0;
}

/**
 * @brief ringBuffer::write
 * @param 数据地址
 * @param 要写的字节数
 * @return 实际写入的字节数
 */
int ringBuffer::write(const void *data, int count)
{
	memcpy(&rbHead, mmapRing, sizeOfAddr);
	memcpy(&rbTail, mmapRing + sizeOfAddr, sizeOfAddr);
    int tailAvailSz = 0;

    if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
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

    if (rbHead <= rbTail)
    {
        tailAvailSz = rbCapacity - (rbTail - rbBuff);
        if (count < tailAvailSz)
        {
            memcpy(mmapRing + rbTail, data, count);
            rbTail += count;
			memcpy(mmapRing + sizeOfAddr, &rbTail, sizeOfAddr);
            return count;
        }
        else if (count > tailAvailSz)
        {
            memcpy(mmapRing + rbTail, data, tailAvailSz);
            memcpy(mmapRing + rbBuff, data + tailAvailSz, count - tailAvailSz);
            rbTail = count - tailAvailSz + rbBuff;
			memcpy(mmapRing + sizeOfAddr, &rbTail, sizeOfAddr);
            return count;
        }
        else
        {
            memcpy(mmapRing + rbTail, data, tailAvailSz);
            rbTail = rbBuff;
			memcpy(mmapRing + sizeOfAddr, &rbTail, sizeOfAddr);
            return count;
        }
    }
    else
    {
        memcpy(mmapRing + rbTail, data, count);
        rbTail += count;
		memcpy(mmapRing + sizeOfAddr, &rbTail, sizeOfAddr);
        return count;
    }
}

/**
 * @brief ringBuffer::size
 * @return 缓冲区大小
 */
int ringBuffer::size()
{
    return bufferSize;
}
