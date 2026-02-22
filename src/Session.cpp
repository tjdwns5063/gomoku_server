#include "Session.h"
#include "Packet.h"
#include <memory>

Session::Session(uint64_t id, SOCKET sock): 
    sessionId(id), socket(sock), userId(0), isAuth(false), sendQueue(SendQueue(this)) {
	ZeroMemory(&recvContext, sizeof(OVERLAPPED_EX));
    ZeroMemory(&sendContext, sizeof(OVERLAPPED_EX));
	recvContext.ioType = IO_TYPE::RECV;
    sendContext.ioType = IO_TYPE::SEND;
}

Session::~Session() {
    disconnect();
}

uint64_t Session::getSessionId() const { return sessionId; }
SOCKET Session::getSocket() const { return socket; }
bool Session::isAuthenticated() const { return isAuth; }
uint64_t Session::getUserId() const { return userId; }
OVERLAPPED_EX* Session::getRecvContext() { return &recvContext; }
OVERLAPPED_EX* Session::getSendContext() { return &sendContext; }

void Session::authenticate(uint64_t userId) {
    userId = userId;
    isAuth = true;
}

void Session::disconnect() {
    if (socket != INVALID_SOCKET) {
        closesocket(socket);
        socket = INVALID_SOCKET;
    }
    isAuth = false;
    userId = 0;
}

void Session::onRecv(int transferredBytes) {
    // 1. OS가 데이터를 버퍼에 직접 써주었으므로, 쓰기 포인터만 밀어줍니다.
    if (recvBuffer.onWrite(transferredBytes) == false) {
        // 버퍼 초과 (악의적 공격 또는 서버 처리 지연)
        disconnect();
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
    }
}

int Session::recv(HANDLE hIOCP) {
    recvBuffer.clean();

    ZeroMemory(&recvContext.overlapped, sizeof(WSAOVERLAPPED));
    recvContext.ioType = IO_TYPE::RECV;

    // 핵심: WSABUF가 가리키는 곳을 RecvBuffer의 실제 빈 공간으로 직접 지정
    recvContext.wsaBuf.buf = recvBuffer.getWritePtr();
    recvContext.wsaBuf.len = recvBuffer.getFreeSize();

    DWORD recvBytes = 0;
    DWORD flags = 0;
    OVERLAPPED_EX* ioContext = getRecvContext();
    CreateIoCompletionPort((HANDLE)getSocket(), hIOCP, (ULONG_PTR)this, 0);

    return WSARecv(getSocket(), &(ioContext->wsaBuf), 1, &recvBytes, &flags, &(ioContext->overlapped), NULL);
}

void Session::send(char* data, size_t len) {
    std::shared_ptr<SendBuffer> pSendBuffer = 
        std::make_shared<SendBuffer>(len);

    pSendBuffer->write(data, len);
	
    sendQueue.enqueue(pSendBuffer);

    return;
}
