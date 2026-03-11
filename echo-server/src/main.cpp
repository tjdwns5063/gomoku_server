#include "NetworkEngine.h"
#include "Packet.h"
#include "MatchQueue.h"
#include <vector>
#include <thread>

static void WorkerThreadMain(HANDLE hIOCP, IdGenerator* userIdGen) {
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
			pSession->disconnect();
			continue;
		}

		// I/O 타입에 따른 분기 처리
		if (pIoContext->ioType == IO_TYPE::RECV) {
			printf("Worker Thread: received %d bytes\n", bytesTransferred);

			//임시 인증
			if (!pSession->isAuthenticated()) {
				pSession->authenticate(userIdGen->generateId()); // TODO: 실제 인증 로직으로 대체
			}

			// [패킷 처리]
			pSession->onRecv(bytesTransferred);

			// [다음 Recv 재등록]
			ZeroMemory(&pIoContext->overlapped, sizeof(WSAOVERLAPPED));

			DWORD recvBytes = 0;
			DWORD flags = 0;
			int recvResult = pSession->recv(hIOCP);

			if (recvResult == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSA_IO_PENDING) {
					printf("WSARecv 재등록 실패: %d\n", error);
					pSession->disconnect();
				}
			}
		}
		else if (pIoContext->ioType == IO_TYPE::SEND) {
			pSession->onSend(bytesTransferred);
		}
	}
}

static void MatchmakingThreadMain(MatchQueue* matchQueue) {
	
	while (true) {
		matchQueue->match();
	}

}

int main() {
	NetworkEngine* networkEngine = new NetworkEngine();
	PacketHandler::initRoutes();
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	std::vector<std::thread> workerThreads;
	std::shared_ptr<MatchQueue> matchQueue = std::make_shared<MatchQueue>();
	IdGenerator* userIdGen = new IdGenerator();

	networkEngine->ready();
	printf("Echo Server started on port %s\n", DEFAULT_PORT);
	for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors * 2; ++i) {
		workerThreads.emplace_back(WorkerThreadMain, networkEngine->getHIOCP(), userIdGen);
	}
	workerThreads.emplace_back(MatchmakingThreadMain, matchQueue.get());

	while (true) {
		// 4. 메인 스레드는 접속(Accept)만 전담합니다.
		Session* session = networkEngine->accept();
		if (session == nullptr)
			continue;

		printf("new client connected\n");
		session->recv(networkEngine->getHIOCP());
	}

}