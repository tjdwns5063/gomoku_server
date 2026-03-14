#include "Packet.h"
#include "Session.h"

std::unordered_map<PacketID, PacketHandler::HandlerFunc> PacketHandler::handlers;

void testHandler(Session* session, const char* buffer, size_t size) {
	const PKT_C2S_ReqPlaceStone* pkt = reinterpret_cast<const PKT_C2S_ReqPlaceStone*>(buffer);

	std::cout << std::format("id: {}", pkt->header.id) << "\n";
	std::cout << std::format("size: {}", pkt->header.size) << "\n";
	std::cout << std::format("x: {}", static_cast<int>(pkt->x)) << "\n";
	std::cout << std::format("y: {}", static_cast<int>(pkt->y)) << "\n";
	std::cout << std::format("color: {}", static_cast<int>(pkt->color)) << "\n";

	struct PKT_S2C_RES_PLACE_STONE res {};

	res.header.id = PacketID::S2C_RES_PLACE_STONE; // 1001
	res.header.size = sizeof(PKT_S2C_RES_PLACE_STONE); // 6byte
	res.status = StatusCode::SUCCESS; // 0
	//06 00 EA 03 00 00

	session->send(reinterpret_cast<char*>(&res), sizeof(res));
}

void matchHandler(Session* session, const char* buffer, size_t size) {
	const PKT_C2S_ReqMatch* pkt = reinterpret_cast<const PKT_C2S_ReqMatch*>(buffer);

	std::cout << std::format("id: {}", pkt->header.id) << "\n";
	std::cout << std::format("size: {}", pkt->header.size) << "\n";
	std::cout << std::format("userId: {}", pkt->userId) << "\n";

	struct PKT_S2C_ResMatch res {};
	
	res.header.id = PacketID::S2C_RES_MATCH; // 1004
	res.header.size = sizeof(PKT_S2C_ResMatch); // 6byte
	res.status = StatusCode::SUCCESS; // 0
	
	session->send(reinterpret_cast<char*>(&res), sizeof(res));
}

void PacketHandler::initRoutes() {
	addHandler(PacketID::C2S_REQ_PLACE_STONE, ::testHandler);
	//PacketHandler::addHandler(1002, OmokController::handleEnterRoom);
}

void PacketHandler::addHandler(PacketID id, HandlerFunc handler) {
	handlers[id] = handler;
}

void PacketHandler::dispatch(Session* session, const char* buffer, size_t size) {
	const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);

	auto it = handlers.find(static_cast<PacketID>(header->id));

	if (it != handlers.end()) {
		it->second(session, buffer, size);
	}
	else {
		session->disconnect();
	}

}