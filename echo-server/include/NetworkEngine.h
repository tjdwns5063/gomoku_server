#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <ws2tcpip.h>
#include "Session.h"
#include "IdGenerator.h"
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "28080"

class NetworkEngine
{
private:
	WSADATA wsaData;
	struct addrinfo* result;
	struct addrinfo* ptr;
	struct addrinfo hints;
	HANDLE hIOCP;
	SOCKET serverSocket;
	IdGenerator sessionIdGenerator;

	int bind();
	int listen();
	void completionCallback(); //TODO: 구현해야함

public:
	NetworkEngine(); //TODO: 스레드 풀 인자 추가, 
	~NetworkEngine();
	void init();
	void ready();
	Session* accept(); //TODO: 구현해야함
	int recv(Session* session);
	HANDLE getHIOCP() const { return hIOCP; }
};

