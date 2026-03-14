#include "Session.h"
#include "Packet.h"
#include <memory>

Session::Session(uint32_t id, SOCKET sock): 
    sessionId(id), socket(sock), userId(0), isAuth(false),
    isConnected(false), refCount(1), sendQueue(SendQueue(this)) {
	ZeroMemory(&recvContext, sizeof(OVERLAPPED_EX));
    ZeroMemory(&sendContext, sizeof(OVERLAPPED_EX));
	recvContext.ioType = IO_TYPE::RECV;
    sendContext.ioType = IO_TYPE::SEND;
}

Session::~Session() {
    disconnect();
}

uint32_t Session::getSessionId() const { return sessionId; }
SOCKET Session::getSocket() const { return socket; }
bool Session::isAuthenticated() const { return isAuth; }
uint32_t Session::getUserId() const { return userId; }
OVERLAPPED_EX* Session::getRecvContext() { return &recvContext; }
OVERLAPPED_EX* Session::getSendContext() { return &sendContext; }

void Session::authenticate(uint32_t userId) {
    userId = userId;
    isAuth = true;
}

void Session::releaseRef() {
    if (refCount.fetch_sub(1) == 1) {
        std::cout << "All I/O is Disconnected. Release Reseource.\n";
        delete this;
    }
}

void Session::disconnect() {
	bool expected = true;
    if (isConnected.compare_exchange_strong(expected, false)) {
        // 최초로 연결이 끊긴 경우에만 소켓 닫기
        if (socket != INVALID_SOCKET) {
            closesocket(socket);
            socket = INVALID_SOCKET;
        }
    }
    
    refCount.fetch_sub(1);
    isAuth = false;
    userId = 0;
    releaseRef();
}

void Session::onRecv(int transferredBytes) {
    // 1. OS가 데이터를 버퍼에 직접 써주었으므로, 쓰기 포인터만 밀어줍니다.
    if (recvBuffer.onWrite(transferredBytes) == false) {
        disconnect();
		releaseRef();
        return;
    }

    // 2. 패킷 조립 루프 (Zero-Copy)
    while (true) {
        int dataSize = recvBuffer.getDataSize();

        // 헤더 크기조차 안 모였다면 대기
        if (dataSize < sizeof(PacketHeader)) {
            break;
        }

        // 읽기 포인터에 헤더 구조체를 씌워서 훔쳐보기 (복사 없음)
        PacketHeader* header = reinterpret_cast<PacketHeader*>(recvBuffer.getReadPtr());

        // 완성된 패킷 1개의 크기보다 적게 모였다면 대기
        if (dataSize < header->size) {
            break;
        }

        // 3. 온전한 패킷 1개가 완성됨! 핸들러로 전달
		PacketHandler::dispatch(this, recvBuffer.getReadPtr(), header->size);

        // 4. 처리한 패킷 크기만큼 읽기 포인터 이동
        recvBuffer.onRead(header->size);
		refCount.fetch_sub(1);
    }
}

int Session::recv(HANDLE hIOCP) {
    recvBuffer.clean();

    refCount.fetch_add(1);

    ZeroMemory(&recvContext.overlapped, sizeof(WSAOVERLAPPED));
    recvContext.ioType = IO_TYPE::RECV;

    recvContext.wsaBuf.buf = recvBuffer.getWritePtr();
    recvContext.wsaBuf.len = recvBuffer.getFreeSize();

    DWORD recvBytes = 0;
    DWORD flags = 0;
    OVERLAPPED_EX* ioContext = getRecvContext();
    CreateIoCompletionPort((HANDLE)getSocket(), hIOCP, (ULONG_PTR)this, 0);

    return WSARecv(getSocket(), &(ioContext->wsaBuf), 1, &recvBytes, &flags, &(ioContext->overlapped), NULL);
}

void Session::send(char* data, size_t len) {
    refCount.fetch_add(1);
    std::shared_ptr<SendBuffer> pSendBuffer = 
        std::make_shared<SendBuffer>(len);

    pSendBuffer->write(data, len);
	
    sendQueue.enqueue(pSendBuffer);

    return;
}

void Session::onSend(int bytesTransferred) {
    printf("Worker Thread: sent %d bytes\n", bytesTransferred);
    refCount.fetch_sub(1);
}

