#pragma once

#include <array>
#include <utility>

class OmokValidator {
private:
    static constexpr std::array<std::pair<int, int>, 4> dirs = { {
        {0, 1},
        {1, 0},
        {1, -1},
        {1, 1}
    } };

    static bool isInside(int x, int y);

public:
    static constexpr int BOARD_SIZE = 16;

	static bool isWin(const std::array<std::array<char, 16>, 16>& map, int x, int y);
};