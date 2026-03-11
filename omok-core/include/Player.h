#pragma once

#include <cstdint>
#include <atomic>

class Player {
private:
	uint32_t id;
	std::atomic<bool> isMatching; // TODO: 추후에 GameLayer로 이동


public:
	Player(uint32_t id);
	uint32_t getId() const;
	bool isMatch() const; // 현재 매치중인지 확인
	void enterMatchQueue(); // 매치 시작
	void leaveMatchQueue(); // 매치 취소 
};
