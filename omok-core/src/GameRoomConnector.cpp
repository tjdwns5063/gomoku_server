#include "GameRoomConnector.h"
#include "GameRoom.h"
#include "Player.h"

GameRoom* GameRoomConnector::create(std::pair<std::shared_ptr<Player>, std::shared_ptr<Player>> players) {

	unsigned int roomId = idGenerator.generateId();
	std::shared_ptr<GameRoom> pGameRoom = std::make_shared<GameRoom>(roomId);
	GameRoom* gameRoom = pGameRoom.get();
	gameRoom->enter(players);

	{
		std::lock_guard<std::mutex> guard(lock);
		gameRooms.emplace(roomId, pGameRoom);
		playerToRoomMap.emplace(players.first.get()->getId(), roomId);
		playerToRoomMap.emplace(players.second.get()->getId(), roomId);
	}

	return gameRoom;
}

GameRoom* GameRoomConnector::find(unsigned int id) {
	std::lock_guard<std::mutex> guard(lock);

	auto it = gameRooms.find(id);
	
	if (it != gameRooms.end()) {
		return it->second.get();
	}
	
	return nullptr;
}

bool GameRoomConnector::remove(unsigned int id) {
	std::lock_guard<std::mutex> guard(lock);
	auto it = gameRooms.find(id);
	
	if (it != gameRooms.end()) {
		GameRoom* gameRoom = it->second.get();

		auto players = gameRoom->getPlayers();

		auto p1It = playerToRoomMap.find(players.first->getId());
		auto p2It = playerToRoomMap.find(players.second->getId());

		if (p1It != playerToRoomMap.end()) {
			playerToRoomMap.erase(p1It);
		}
		if (p2It != playerToRoomMap.end()) {
			playerToRoomMap.erase(p2It);
		}

		gameRooms.erase(it);
		return true;
	}
	
	return false;
}

GameRoom* GameRoomConnector::findByPlayerId(unsigned int playerId) {
	std::lock_guard<std::mutex> guard(lock);
	auto it = playerToRoomMap.find(playerId);
	
	if (it != playerToRoomMap.end()) {
		unsigned int roomId = it->second;

		auto roomIt = gameRooms.find(roomId);

		if (roomIt != gameRooms.end()) {
			return roomIt->second.get();
		}
	}
	
	return nullptr;
}
