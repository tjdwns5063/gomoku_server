#include "gtest/gtest.h"
#include "OmokValidator.h"

#pragma once

std::array<std::array<char, 16>, 16> emptyMap = { {
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


TEST(OmokRuleTest, CheckRightWin) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[1][3] = 'B';
	map[1][4] = 'B';
	map[1][5] = 'B';
	map[1][6] = 'B';

	bool isWin = OmokValidator::isWin(map, 1, 2);

	EXPECT_EQ(isWin, true);

};

TEST(OmokRuleTest, CheckLeftWin) {
	auto map = emptyMap;
	map[1][2] = 'B';
	map[1][3] = 'B';
	map[1][4] = 'B';
	map[1][5] = 'B';
	map[1][6] = 'B';
	bool isWin = OmokValidator::isWin(map, 1, 6);
	EXPECT_EQ(isWin, true);
};

TEST(OmokRuleTest, CheckUpWin) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[2][2] = 'B';
	map[3][2] = 'B';
	map[4][2] = 'B';
	map[5][2] = 'B';
	bool isWin = OmokValidator::isWin(map, 1, 2);
	EXPECT_EQ(isWin, true);
};

TEST(OmokRuleTest, CheckDownWin) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[2][2] = 'B';
	map[3][2] = 'B';
	map[4][2] = 'B';
	map[5][2] = 'B';
	bool isWin = OmokValidator::isWin(map, 5, 2);
	EXPECT_EQ(isWin, true);
};

TEST(OmokRuleTest, CheckDownRightWin) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[2][3] = 'B';
	map[3][4] = 'B';
	map[4][5] = 'B';
	map[5][6] = 'B';
	bool isWin = OmokValidator::isWin(map, 1, 2);
	EXPECT_EQ(isWin, true);
};

TEST(OmokRuleTest, CheckUpLeftWin) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[2][3] = 'B';
	map[3][4] = 'B';
	map[4][5] = 'B';
	map[5][6] = 'B';
	bool isWin = OmokValidator::isWin(map, 5, 6);
	EXPECT_EQ(isWin, true);
};

TEST(OmokRuleTest, CheckUpRightWin) {
	auto map = emptyMap;

	map[1][10] = 'B';
	map[2][9] = 'B';
	map[3][8] = 'B';
	map[4][7] = 'B';
	map[5][6] = 'B';
	bool isWin = OmokValidator::isWin(map, 5, 6);
	EXPECT_EQ(isWin, true);
};

TEST(OmokRuleTest, CheckDownLeftWin) {
	auto map = emptyMap;

	map[1][10] = 'B';
	map[2][9] = 'B';
	map[3][8] = 'B';
	map[4][7] = 'B';
	map[5][6] = 'B';
	bool isWin = OmokValidator::isWin(map, 1, 10);
	EXPECT_EQ(isWin, true);
}

TEST(OmokRuleTest, SixStoneFailure) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[1][3] = 'B';
	map[1][4] = 'B';
	map[1][5] = 'B';
	map[1][6] = 'B';
	map[1][7] = 'B';

	bool isWin = OmokValidator::isWin(map, 1, 2);

	EXPECT_EQ(isWin, false);
}

TEST(OmokRuleTest, CheckMid) {
	auto map = emptyMap;

	map[1][2] = 'B';
	map[1][3] = 'B';
	map[1][4] = 'B';
	map[1][5] = 'B';
	map[1][6] = 'B';

	bool isWin = OmokValidator::isWin(map, 1, 4);

	EXPECT_EQ(isWin, true);
}