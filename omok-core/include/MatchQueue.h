#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <utility>
#include "Player.h"

enum class MatchFlag: uint8_t {
	WAITING = 0,
	MATCHED = 1
};

struct MatchInfo {
	MatchFlag flag;
	std::pair<Player*, Player*> matchedSessions;
};

class MatchQueue
{
private:
	std::deque<Player*> queue;
	std::mutex lock;

public:
	void enqueue(Player* session);
	MatchInfo match();
};

