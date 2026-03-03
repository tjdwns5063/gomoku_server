#pragma once

#include <WinSock2.h>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include "SendBuffer.h"


constexpr int MAX_SEND_COUNT = 64;

class Session;

class SendQueue
{
private:
	std::vector<std::shared_ptr<SendBuffer>> sendPendingQueue;
	std::vector<std::shared_ptr<SendBuffer>> writeQueue;
	std::mutex queueMutex;
	std::atomic<bool> isSending;
	Session* session;
	void send();

public:
	SendQueue(Session* session);
	void enqueue(std::shared_ptr<SendBuffer> sendBuffer);
	void prepaseWsaBufs(WSABUF* wsaBufs, size_t& count);
	void onSendComplete();
};

