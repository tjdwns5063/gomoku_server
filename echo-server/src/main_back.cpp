//#define WIN32_LEAN_AND_MEAN 
//
//#include <windows.h>
//#include <cstdint>
//#include <iostream>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include "main.h"
//#include <vector>
//#include <thread>
//#include "Session.h"
//
//#pragma comment(lib, "Ws2_32.lib")
//#define DEFAULT_BUFLEN 512
//
//
//constinit const char* DEFAULT_PORT = "28080";
//
//void WorkerThreadMain(HANDLE hIOCP) {
//	DWORD bytesTransferred = 0;
//	Session* pSession = nullptr; // Per-Socket Data (CompletionKey)
//	WSAOVERLAPPED* pOverlapped = nullptr;
//
//	while (true) {
//		BOOL result = GetQueuedCompletionStatus(
//			hIOCP,
//			&bytesTransferred,
//			(PULONG_PTR)&pSession,
//			&pOverlapped,
//			INFINITE
//		);
//
//		// 확장 구조체로 캐스팅
//		OVERLAPPED_EX* pIoContext = reinterpret_cast<OVERLAPPED_EX*>(pOverlapped);
//
//		//세션 종료 처리
//		if (result == 0 || bytesTransferred == 0) {
//			std::cerr << "Client disconnected or error: " << GetLastError() << std::endl;
//			closesocket(pSession->socket);
//			delete pSession;
//			//if (pIoContext) delete pIoContext; // 누수 방지
//			continue;
//		}
//
//		// I/O 타입에 따른 분기 처리
//		if (pIoContext->ioType == IO_TYPE::RECV) {
//			printf("Worker Thread: received %d bytes\n", bytesTransferred);
//			printf("Worker Thread: %.*s\n", bytesTransferred, pIoContext->buffer);
//
//			// [논블로킹 WSASend 처리]
//			// Send를 위한 새로운 I/O 컨텍스트 할당 (Recv와 겹치지 않도록 분리)
//			OVERLAPPED_EX* pSendContext = new OVERLAPPED_EX();
//			ZeroMemory(&pSendContext->overlapped, sizeof(WSAOVERLAPPED));
//			pSendContext->ioType = IO_TYPE::SEND;
//
//			// 에코 서버 가정: 받은 데이터를 그대로 Send 버퍼로 복사
//			memcpy(pSendContext->buffer, pIoContext->buffer, bytesTransferred);
//			pSendContext->wsaBuf.buf = pSendContext->buffer;
//			pSendContext->wsaBuf.len = bytesTransferred;
//
//			DWORD sendBytes = 0;
//			// WSASend 비동기 호출 (마지막에서 두 번째 인자에 pSendContext->overlapped 전달)
//			int sendResult = WSASend(pSession->socket, &(pSendContext->wsaBuf), 1, &sendBytes, 0, &(pSendContext->overlapped), NULL);
//			if (sendResult == SOCKET_ERROR) {
//				int error = WSAGetLastError();
//				if (error != WSA_IO_PENDING) {
//					printf("WSASend 실패: %d\n", error);
//					delete pSendContext;
//					closesocket(pSession->socket);
//					// Session 정리는 설계에 따라 다름 (중복 delete 주의)
//				}
//			}
//
//			// [다음 Recv 재등록]
//			ZeroMemory(&pIoContext->overlapped, sizeof(WSAOVERLAPPED));
//			pIoContext->wsaBuf.buf = pIoContext->buffer;
//			pIoContext->wsaBuf.len = sizeof(pIoContext->buffer); // 최대 버퍼 크기 지정
//
//			DWORD recvBytes = 0;
//			DWORD flags = 0;
//			int recvResult = WSARecv(pSession->socket, &(pIoContext->wsaBuf), 1, &recvBytes, &flags, &(pIoContext->overlapped), NULL);
//			if (recvResult == SOCKET_ERROR) {
//				int error = WSAGetLastError();
//				if (error != WSA_IO_PENDING) {
//					printf("WSARecv 재등록 실패: %d\n", error);
//					closesocket(pSession->socket);
//				}
//			}
//
//		}
//		else if (pIoContext->ioType == IO_TYPE::SEND) {
//			// WSASend의 비동기 작업이 완료되어 GQCS가 깨어난 경우
//			// 송신이 완료되었으므로 동적 할당했던 Send용 컨텍스트를 메모리에서 해제
//			printf("Worker Thread: sent %d bytes\n", bytesTransferred);
//			//delete pIoContext;
//		}
//	}
//}
//
//int main() {
//
//	struct addrinfo* result = NULL;
//	struct addrinfo* ptr = NULL;
//	struct addrinfo hints;
//	int iResult;
//	WSADATA wsaData;
//	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (iResult != 0) {
//		std::cerr << "WSAStartup failed: " << iResult << std::endl;
//		return 1;
//	}
//
//	HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//	SYSTEM_INFO sysInfo;
//	GetSystemInfo(&sysInfo);
//	std::vector<std::thread> workerThreads;
//	for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors * 2; ++i) {
//		workerThreads.emplace_back(WorkerThreadMain, hIOCP);
//	}
//
//	ZeroMemory(&hints, sizeof(hints));
//
//	hints.ai_family = AF_INET;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//	hints.ai_flags = AI_PASSIVE;
//
//	iResult = getaddrinfo(NULL, "28080", &hints, &result);
//
//	SOCKET ListenSocket = NULL;
//	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);	
//	if (ListenSocket == INVALID_SOCKET) {
//		printf("Error at socket(): %ld\n", WSAGetLastError());
//		freeaddrinfo(result);
//		WSACleanup();
//		return 1;
//	}
//
//	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
//	if (iResult == SOCKET_ERROR) {
//		printf("bind failed with error: %d\n", WSAGetLastError());
//		freeaddrinfo(result);
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
//		printf("Listen failed with error: %ld\n", WSAGetLastError());
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	printf("IOCP server start...\n");
//
//	while (true) {
//		// 4. 메인 스레드는 접속(Accept)만 전담합니다.
//		SOCKET clientSocket = accept(ListenSocket, NULL, NULL);
//		if (clientSocket == INVALID_SOCKET) continue;
//
//		printf("new client connected\n");
//
//		// 세션(클라이언트 정보) 생성
//		Session* newSession = new Session(0, clientSocket, IO_TYPE::RECV);
//		
//		// 5. 새로 접속한 소켓을 IOCP에 연결 (Bind)
//		CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)newSession, 0);
//
//		// 6. 첫 데이터 수신 예약 (비동기)
//		DWORD recvBytes = 0;
//		DWORD flags = 0;
//		WSARecv(clientSocket, &(newSession->overlapped.wsaBuf), 1, &recvBytes, &flags, &(newSession->overlapped.overlapped), NULL);
//	}
//
//
//
//	closesocket(ListenSocket);
//	WSACleanup();
//	return 0;
//}
//
//void print_addr_info(addrinfo info) {
//	printf("ai_flags: %d\n", info.ai_flags);
//	printf("ai_family: %d\n", info.ai_family);
//	printf("ai_socktype: %d\n", info.ai_socktype);
//	printf("ai_protocol: %d\n", info.ai_protocol);
//	printf("ai_addr: %p\n", info.ai_addr);
//	printf("ai_addrlen: %zu\n", info.ai_addrlen);
//	printf("ai_canonname: %s\n", info.ai_canonname);
//}
