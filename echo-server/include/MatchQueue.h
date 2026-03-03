#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <utility>
#include "Session.h"

enum class MatchFlag: uint8_t {
	WAITING = 0,
	MATCHED = 1
};

struct MatchInfo {
	MatchFlag flag;
	std::pair<Session*, Session*> matchedSessions;
};

class MatchQueue
{
private:
	std::deque<Session*> queue;
	std::mutex lock;

public:
	void enqueue(Session* session);
	MatchInfo match();
};

