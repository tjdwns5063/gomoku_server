#include "OmokValidator.h"

bool OmokValidator::isWin(std::array<std::array<char, 16>, 16> map, int x, int y) {
	char currColor = map[x][y];

	for (auto dir : dirs) {
		int nx = x + dir.first;
		int ny = y + dir.second;
		
		if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) continue;

		char nextColor = map[nx][ny];

		int count = 1;
		while (currColor == nextColor) {
			++count;
			nx += dir.first;
			ny += dir.second;

			if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) break;

			nextColor = map[nx][ny];
		}
		if (count == 5) return true;
	}
	return false;
}