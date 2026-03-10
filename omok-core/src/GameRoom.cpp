#include "GameRoom.h"
#include "Player.h"

constexpr std::array<std::array<char, OmokValidator::BOARD_SIZE>, OmokValidator::BOARD_SIZE> emptyBoard = { {
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
	} };

GameRoom::GameRoom(unsigned int roomId) :
	roomId(roomId), board(emptyBoard), turn('B') { }

void GameRoom::enter(std::pair<std::shared_ptr<Player>, std::shared_ptr<Player>> players) {
	std::lock_guard<std::mutex> guard(lock);
	this->players = std::move(players);
}

PlaceRes GameRoom::place(int x, int y, unsigned int playerId) {
	std::lock_guard<std::mutex> guard(lock);

	//turn 검증
	Player* player = (turn == 'B') ? players.first.get() : players.second.get();

	if (player->getId() != playerId) {
		return { 0, (unsigned char)x, (unsigned char)y, turn, false, false };
	}

	if (x < 0 || y < 0 || x >= OmokValidator::BOARD_SIZE || y >= OmokValidator::BOARD_SIZE) {
		return { 0, (unsigned char)x, (unsigned char)y, turn, false, false };
	}

	if (board[x][y] != '.') {
		return { 0, (unsigned char)x, (unsigned char)y, turn, false, false };
	}

	board[x][y] = turn;

	if (OmokValidator::isWin(board, x, y)) {
		unsigned int winnerId = (turn == 'B') ? players.first->getId() : players.second->getId();
		
		return { winnerId, (unsigned char)x, (unsigned char)y, turn, true, true };
	}

	turn = (turn == 'B') ? 'W' : 'B';

	return { 0, (unsigned char)x, (unsigned char)y, turn, false, true};
} 

std::pair<std::shared_ptr<const Player>, std::shared_ptr<const Player>> GameRoom::getPlayers() {
	std::lock_guard<std::mutex> guard(lock);

	return { players.first, players.second };
}