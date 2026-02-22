#include "NetworkEngine.h"
#include "Packet.h"
#include <vector>
#include <thread>

static void WorkerThreadMain(HANDLE hIOCP) {
	DWORD bytesTransferred = 0;
	Session* pSession = nullptr; // Per-Socket Data (CompletionKey)
	WSAOVERLAPPED* pOverlapped = nullptr;

	while (true) {
		BOOL result = GetQueuedCompletionStatus(
			hIOCP,
			&bytesTransferred,
			(PULONG_PTR)&pSession,
			&pOverlapped,
			INFINITE
		);

		// 확장 구조체로 캐스팅
		OVERLAPPED_EX* pIoContext = reinterpret_cast<OVERLAPPED_EX*>(pOverlapped);

		//세션 종료 처리
		if (result == 0 || bytesTransferred == 0) {
			std::cerr << "Client disconnected or error: " << GetLastError() << std::endl;
			closesocket(pSession->getSocket());
			delete pSession;
			//if (pIoContext) delete pIoContext; // 누수 방지
			continue;
		}

		// I/O 타입에 따른 분기 처리
		if (pIoContext->ioType == IO_TYPE::RECV) {
			printf("Worker Thread: received %d bytes\n", bytesTransferred);

			// [패킷 처리]
			pSession->onRecv(bytesTransferred);

			// [Send 처리]
			

			// [다음 Recv 재등록]
			ZeroMemory(&pIoContext->overlapped, sizeof(WSAOVERLAPPED));

			DWORD recvBytes = 0;
			DWORD flags = 0;
			int recvResult = pSession->recv(hIOCP);

			if (recvResult == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSA_IO_PENDING) {
					printf("WSARecv 재등록 실패: %d\n", error);
					closesocket(pSession->getSocket());
				}
			}

		}
		else if (pIoContext->ioType == IO_TYPE::SEND) {
			// WSASend의 비동기 작업이 완료되어 GQCS가 깨어난 경우
			// 송신이 완료되었으므로 동적 할당했던 Send용 컨텍스트를 메모리에서 해제
			printf("Worker Thread: sent %d bytes\n", bytesTransferred);
			//delete pIoContext;
		}
	}
}

int main() {
	NetworkEngine* networkEngine = new NetworkEngine();
	PacketHandler::initRoutes();
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	std::vector<std::thread> workerThreads;


	networkEngine->ready();
	printf("Echo Server started on port %s\n", DEFAULT_PORT);
	for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors * 2; ++i) {
		workerThreads.emplace_back(WorkerThreadMain, networkEngine->getHIOCP());
	}

	while (true) {
		// 4. 메인 스레드는 접속(Accept)만 전담합니다.
		Session* session = networkEngine->accept();
		if (session == nullptr)
			continue;

		printf("new client connected\n");
		session->recv(networkEngine->getHIOCP());
	}

}