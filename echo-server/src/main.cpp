#include "NetworkEngine.h"
#include "Packet.h"
#include "MatchQueue.h"
#include "PlayerRepository.h"
#include "SessionManager.h"
#include <vector>
#include <thread>

static void WorkerThreadMain(HANDLE hIOCP, IdGenerator* sessionIdGen, SessionManager* sessionManager,
	PlayerRepository* playerRepository, IdGenerator* playerIdGen) {
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
				pSession->authenticate(sessionIdGen->generateId()); // TODO: 실제 인증 로직으로 대체
				std::shared_ptr<Player> player = std::make_shared<Player>(playerIdGen->generateId());
				playerRepository->save(player);
				//TODO: Player 생성 오류처리
				std::cout << "playerId: " << player->getId() << "\n";

				sessionManager->save(pSession->getSessionId(), player->getId());
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
		MatchInfo info = matchQueue->match();

		if (info.flag == MatchFlag::MATCHED) {
			std::cout << "Match Success! Player1 ID: " << info.matchedSessions.first->getId()
				<< ", Player2 ID: " << info.matchedSessions.second->getId() << "\n";
			//TODO: 게임방 생성 및 클라이언트 통지
		}
	}

}

int main() {
	NetworkEngine* networkEngine = new NetworkEngine();
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	std::vector<std::thread> workerThreads;
	std::shared_ptr<MatchQueue> matchQueue = std::make_shared<MatchQueue>();
	IdGenerator* userIdGen = new IdGenerator();
	SessionManager* sessionManager = new SessionManager();
	PlayerRepository* playerRepository = new PlayerRepository();
	IdGenerator* playerIdGen = new IdGenerator();

	PacketHandler::addHandler(PacketID::C2S_REQ_MATCH, [matchQueue, sessionManager, playerRepository](Session* s, const char* buf, size_t size) {
		const PKT_C2S_ReqMatch* pkt = reinterpret_cast<const PKT_C2S_ReqMatch*>(buf);

		std::cout << std::format("id: {}", pkt->header.id) << "\n";
		std::cout << std::format("size: {}", pkt->header.size) << "\n";
		std::cout << std::format("userId: {}", pkt->userId) << "\n";

		uint32_t playerId = sessionManager->findPlayerIdBySessionId(s->getSessionId());
		std::shared_ptr<Player> player = playerRepository->findById(playerId);

		matchQueue->enqueue(player.get());

		struct PKT_S2C_ResMatch res {};

		res.header.id = PacketID::S2C_RES_MATCH; // 1004
		res.header.size = sizeof(PKT_S2C_ResMatch); // 6byte
		res.status = StatusCode::SUCCESS; // 0

		s->send(reinterpret_cast<char*>(&res), sizeof(res));

		return;
	});

	networkEngine->ready();
	printf("Echo Server started on port %s\n", DEFAULT_PORT);
	for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors * 2; ++i) {
		workerThreads.emplace_back(WorkerThreadMain, networkEngine->getHIOCP(), userIdGen,
			sessionManager, playerRepository, playerIdGen
		);
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