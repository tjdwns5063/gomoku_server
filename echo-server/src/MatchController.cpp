#include "MatchController.h"
#include "MatchQueue.h"
#include "SessionManager.h"
#include "Session.h"
#include "PlayerRepository.h"
#include "Packet.h"

MatchController::MatchController(
	std::shared_ptr<SessionManager> sessionManager,
	std::shared_ptr<PlayerRepository> playerRepository,
	std::shared_ptr<MatchQueue> matchQueue
) : sessionManager(sessionManager), playerRepository(playerRepository), matchQueue(matchQueue) {}

void MatchController::handleReqMatch(Session* s, const char* buf, size_t size) {
	const PKT_C2S_ReqMatch* pkt = reinterpret_cast<const PKT_C2S_ReqMatch*>(buf);
	uint32_t playerId = sessionManager->findPlayerIdBySessionId(s->getSessionId());
	std::shared_ptr<Player> player = playerRepository->findById(playerId);

	matchQueue->enqueue(player.get());
	
	struct PKT_S2C_ResMatch res {};
	res.header.id = PacketID::S2C_RES_MATCH; // 1004
	res.header.size = sizeof(PKT_S2C_ResMatch); // 6byte
	res.status = StatusCode::SUCCESS; // 0
	res.matchFlag = 0; // 매치 대기 상태
	
	s->send(reinterpret_cast<char*>(&res), sizeof(res));
}

void MatchController::registerHandlers() {
	PacketHandler::addHandler(PacketID::C2S_REQ_MATCH, [this](Session* s, const char* buf, size_t size) {
		this->handleReqMatch(s, buf, size);
	});
}