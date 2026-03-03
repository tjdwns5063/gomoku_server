#include "SendBuffer.h"

SendBuffer::SendBuffer(size_t capacity)
	: buffer(capacity), writeSize(0)
{
}

void SendBuffer::write(const char* data, size_t len)
{
	if (writeSize + len > buffer.size()) {
		throw std::runtime_error("SendBuffer overflow");
	}
	std::copy(data, data + len, buffer.data() + writeSize);
	writeSize += len;
}

char* SendBuffer::getBufferPtr()
{
	return buffer.data();
}

int SendBuffer::getWriteSize() const
{
	return static_cast<int>(writeSize);
}
