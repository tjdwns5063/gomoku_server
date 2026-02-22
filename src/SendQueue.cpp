#include "SendQueue.h"
#include "Session.h"

SendQueue::SendQueue(Session* session) : isSending(false), session(session)
{
}

void SendQueue::enqueue(std::shared_ptr<SendBuffer> sendBuffer) {
	{
		std::lock_guard<std::mutex> lock(queueMutex);

		writeQueue.push_back(sendBuffer);
	}

	if (!isSending.exchange(true)) {
		send();
	}
}

void SendQueue::prepaseWsaBufs(WSABUF* wsaBufs, size_t& count) {
	count = 0;
	
	for (auto& buffer : sendPendingQueue) {
		if (count >= MAX_SEND_COUNT) break; // 최대 전송 개수 제한
		wsaBufs[count].buf = buffer->getBufferPtr();
		wsaBufs[count].len = buffer->getWriteSize();
		count++;
	}
}

void SendQueue::send() {
	{
		std::lock_guard<std::mutex> lock(queueMutex);

		if (writeQueue.empty()) {
			isSending.exchange(false);
			return;
		}

		writeQueue.swap(sendPendingQueue);
	}

	WSABUF wsaBufs[MAX_SEND_COUNT];
	OVERLAPPED_EX* pSendContext = session->getSendContext();
	DWORD sendBytes = 0;
	size_t dataCount = 0;

	prepaseWsaBufs(wsaBufs, dataCount);

	int result = WSASend(session->getSocket(), wsaBufs, dataCount, &sendBytes, 0, &(pSendContext->overlapped), NULL);
	if (result == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			// 전송 실패 (세션 끊김 등). 상태를 원복해야 무한 루프나 블로킹을 막음
			isSending.exchange(false);
			session->disconnect();
		}
	}
}

void SendQueue::onSendComplete() {
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		sendPendingQueue.clear();
	}
	send();
}
