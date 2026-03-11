#include "MatchQueue.h"

void MatchQueue::enqueue(Player* session) {
	{
		std::lock_guard<std::mutex> guard(lock);
		session->enterMatchQueue();
		queue.push_back(session);
	}
	return;
}

MatchInfo MatchQueue::match() {
	Player* waitPlayer[2] = { nullptr, };

	{
		std::lock_guard<std::mutex> guard(lock);

		if (queue.size() < 2) {
			return { MatchFlag::WAITING, std::pair(nullptr, nullptr) };
		}

		int count = 0;

		while (!queue.empty() && count < 2) {
			Player* player = queue.front();
			
			if (player->isMatch()) {
				waitPlayer[count] = player;
				++count;
			}
			queue.pop_front();
		}

		if (count < 2) {
			if (waitPlayer[0] != nullptr) {
				queue.push_front(waitPlayer[0]);
			}
			return { MatchFlag::WAITING, std::pair(nullptr, nullptr) };
		}

		waitPlayer[0]->leaveMatchQueue();
		waitPlayer[1]->leaveMatchQueue();
	}
	
	return { MatchFlag::MATCHED, std::pair(waitPlayer[0], waitPlayer[1]) };

}
