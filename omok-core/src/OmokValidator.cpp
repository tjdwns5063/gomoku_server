#include "OmokValidator.h"

bool OmokValidator::isInside(int x, int y) {
	return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}

bool OmokValidator::isWin(const std::array<std::array<char, 16>, 16>& map, int x, int y) {
	char currColor = map[x][y];

	for (auto dir : dirs) {
		int leftNx = x + dir.first;
		int leftNy = y + dir.second;
		int rightNx = x + (dir.first * -1);
		int rightNy = y + (dir.second * -1);

		int leftCount = 0;

		while (isInside(leftNx, leftNy) && currColor == map[leftNx][leftNy]) {
			++leftCount;
			leftNx += dir.first;
			leftNy += dir.second;
		}

		int rightCount = 0;
		
		while (isInside(rightNx, rightNy) && currColor == map[rightNx][rightNy]) {
			++rightCount;
			rightNx += dir.first * -1;
			rightNy += dir.second * -1;
		}

		if (leftCount + rightCount + 1 == 5) 
			return true;
	}
	return false;
}