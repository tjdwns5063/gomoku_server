#include "Packet.h"
#include "Session.h"

std::unordered_map<PacketID, void(*)(const char* buffer, size_t size)> PacketHandler::handlers;

void testHandler(const char* buffer, size_t size) {
	PKT_C2S_ReqPlaceStone* pkt = reinterpret_cast<PKT_C2S_ReqPlaceStone*>(const_cast<char*>(buffer));

	std::cout << std::format("id: {}", pkt->header.id) << "\n";
	std::cout << std::format("size: {}", pkt->header.size) << "\n";
	std::cout << std::format("x: {}", static_cast<int>(pkt->x)) << "\n";
	std::cout << std::format("y: {}", static_cast<int>(pkt->y)) << "\n";
	std::cout << std::format("color: {}", static_cast<int>(pkt->color)) << "\n";

}

void PacketHandler::initRoutes() {
	addHandler(PacketID::C2S_REQ_PLACE_STONE, ::testHandler);
	//PacketHandler::addHandler(1002, OmokController::handleEnterRoom);
}

void PacketHandler::addHandler(PacketID id, void(*handler)(const char* buffer, size_t size)) {
	handlers[id] = handler;
}

void PacketHandler::dispatch(Session* session, const char* buffer, size_t size) {
	const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);

	auto it = handlers.find(static_cast<PacketID>(header->id));

	if (it != handlers.end()) {
		it->second(buffer, size);
	}
	else {
		session->disconnect();
	}

}