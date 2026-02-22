#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <cstdint>
#include "RecvBuffer.h"

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
    OVERLAPPED_EX overlapped;
    uint64_t sessionId;
    SOCKET socket;
    uint64_t userId;
    bool isAuth;
    RecvBuffer recvBuffer{65536};

public:
    Session(uint64_t id, SOCKET sock, IO_TYPE ioType);
    ~Session();

    uint64_t getSessionId() const;
    SOCKET getSocket() const;
    bool isAuthenticated() const;
    uint64_t getUserId() const;
    OVERLAPPED_EX* getOverlapped();

    void authenticate(uint64_t userId);
    void disconnect();
    int recv(HANDLE hIOCP);
    void onRecv(int transferredBytes);
};