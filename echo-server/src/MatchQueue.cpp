#include "MatchQueue.h"

void MatchQueue::enqueue(Session* session) {
	{
		std::lock_guard<std::mutex> guard(lock);
		session->enterMatchQueue();
		queue.push_back(session);
	}
	return;
}

MatchInfo MatchQueue::match() {
	Session* waitSession[2] = { nullptr, };

	{
		std::lock_guard<std::mutex> guard(lock);

		if (queue.size() < 2) {
			return { MatchFlag::WAITING, std::pair(nullptr, nullptr) };
		}

		int count = 0;

		while (!queue.empty() && count < 2) {
			Session* session = queue.front();
			
			if (session->isMatch()) {
				waitSession[count] = session;
				++count;
			}
			queue.pop_front();
		}

		if (count < 2) {
			if (waitSession[0] != nullptr) {
				queue.push_front(waitSession[0]);
			}
			return { MatchFlag::WAITING, std::pair(nullptr, nullptr) };
		}

		waitSession[0]->leaveMatchQueue();
		waitSession[1]->leaveMatchQueue();
	}
	
	return { MatchFlag::MATCHED, std::pair(waitSession[0], waitSession[1]) };

}
