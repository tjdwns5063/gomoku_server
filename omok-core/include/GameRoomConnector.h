#pragma once

#include <unordered_map>
#include <mutex>
#include <memory>
#include "IdGenerator.h"

class GameRoom;

class Player;

class GameRoomConnector {
private:
	IdGenerator idGenerator;
	std::unordered_map<unsigned int, std::shared_ptr<GameRoom>> gameRooms;
	std::unordered_map<unsigned int, unsigned int> playerToRoomMap;
	std::mutex lock;

public:
	GameRoom* create(std::pair<std::shared_ptr<Player>, std::shared_ptr<Player>> players);
	GameRoom* find(unsigned int id);
	GameRoom* findByPlayerId(unsigned int playerId);
	bool remove(unsigned int id);
};