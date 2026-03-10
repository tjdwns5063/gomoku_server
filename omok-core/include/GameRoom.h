#pragma once

#include <utility>
#include <memory>
#include <array>
#include <mutex>
#include <cstdint>
#include "OmokValidator.h"

class Player;

#pragma pack(push, 1)
struct PlaceRes {
	uint32_t winnerId;
	uint8_t x;
	uint8_t y;
	uint8_t turn;
	uint8_t isEnd;
	uint8_t status;
};
#pragma pack(pop)

class GameRoom {
private:
	const uint32_t roomId;
	std::pair<std::shared_ptr<Player>, std::shared_ptr<Player>> players;
	std::array<std::array<char, OmokValidator::BOARD_SIZE>, OmokValidator::BOARD_SIZE> board;
	unsigned char turn; // 0: player1(black), 1: player2(white)
	std::mutex lock;
	

public:
	GameRoom(unsigned int roomId);
	void enter(std::pair<std::shared_ptr<Player>, std::shared_ptr<Player>> players);
	PlaceRes place(int x, int y, unsigned int playerId);
	std::pair<std::shared_ptr<const Player>, std::shared_ptr<const Player>> getPlayers();
};