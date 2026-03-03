#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(size_t size): 
	buffer(size), writePos(0), readPos(0), capacity(size) {}

size_t RecvBuffer::getDataSize() const
{
	return writePos - readPos;
}

size_t RecvBuffer::getFreeSize() const
{
	return capacity - writePos;
}

void RecvBuffer::clean() {
    int dataSize = getDataSize();

    if (dataSize == 0) {
        readPos = 0;
        writePos = 0;
        return ;
    }

    std::memmove(&buffer[0], &buffer[readPos], dataSize);
    readPos = 0;
    writePos = dataSize;
    
    return ;
}

char* RecvBuffer::getWritePtr() {
    return buffer.data() + writePos;
}

char* RecvBuffer::getReadPtr() {
	return buffer.data() + readPos;
}

bool RecvBuffer::onRead(size_t len) {
    if (len > getDataSize()) {
        return false; // 읽으려는 데이터 크기가 버퍼에 있는 데이터 크기보다 큼
	}
    readPos += len;
    return true;
}

bool RecvBuffer::onWrite(size_t len) {
    if (len > getFreeSize()) {
        return false; // 쓰려는 데이터 크기가 버퍼의 남은 공간보다 큼
	}
    writePos += len;
    return true;
}
