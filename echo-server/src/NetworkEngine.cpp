#include "NetworkEngine.h"

NetworkEngine::NetworkEngine()
{
	init();
}

NetworkEngine::~NetworkEngine()
{
	closesocket(serverSocket);
	freeaddrinfo(result);
	WSACleanup();
}

void NetworkEngine::init() {
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		throw std::runtime_error("WSAStartup failed " + iResult);
		return;
	}
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		throw std::runtime_error("getaddrinfo failed " + iResult);
	}
	serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (serverSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		throw std::runtime_error("Socket creation failed");
	}

}

void NetworkEngine::ready()
{
	if (bind() != 0) {
		throw std::runtime_error("Bind failed");
	}

	if (listen() != 0) {
		throw std::runtime_error("Listen failed");
	}
	
}

int NetworkEngine::bind()
{
	int iResult = ::bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl << "Please init() Called" << std::endl;
		freeaddrinfo(result);
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int NetworkEngine::listen()
{
	if (::listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int NetworkEngine::recv(Session* session) {
	DWORD recvBytes = 0;
	DWORD flags = 0;
	OVERLAPPED_EX* ioContext = session->getRecvContext();
	CreateIoCompletionPort((HANDLE)session->getSocket(), hIOCP, (ULONG_PTR)session, 0);

	return WSARecv(session->getSocket(), &(ioContext->wsaBuf), 1, &recvBytes, &flags, &(ioContext->overlapped), NULL);
}

Session* NetworkEngine::accept() {
	SOCKET clientSocket = ::accept(this->serverSocket, NULL, NULL);
	
	if (clientSocket == INVALID_SOCKET) {
		return nullptr;
	}

	return new Session(sessionIdGenerator.generateId(), clientSocket);
}
