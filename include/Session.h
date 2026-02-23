#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <cstdint>
#include <atomic>
#include "RecvBuffer.h"
#include "SendQueue.h"

#define DEFAULT_BUFLEN 1024

enum IO_TYPE {
	SEND,
	RECV
};

struct OVERLAPPED_EX {
	WSAOVERLAPPED overlapped; // [오프셋 0] 반드시 첫 번째 위치
	IO_TYPE ioType;           // [오프셋 32] (32비트 환경 기준)
	WSABUF wsaBuf;            // [오프셋 36]
};

class Session {
private:
	std::atomic<bool> isConnected;
    std::atomic<int> refCount;
    OVERLAPPED_EX recvContext;
    OVERLAPPED_EX sendContext;
    uint64_t sessionId;
    SOCKET socket;
    uint64_t userId;
    bool isAuth;
    SendQueue sendQueue;
    RecvBuffer recvBuffer{ 65536 };

public:
    Session(uint64_t id, SOCKET sock);
    ~Session();

    uint64_t getSessionId() const;
    SOCKET getSocket() const;
    bool isAuthenticated() const;
    uint64_t getUserId() const;
    OVERLAPPED_EX* getRecvContext();
    OVERLAPPED_EX* getSendContext();

    void authenticate(uint64_t userId);
    void disconnect();
    int recv(HANDLE hIOCP);
    void onRecv(int transferredBytes);
	void send(char* data, size_t len);
    void onSend(int bytesTransferred);
    void releaseRef();
};