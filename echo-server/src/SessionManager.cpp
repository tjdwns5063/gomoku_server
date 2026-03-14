#include "SessionManager.h"

bool SessionManager::save(uint32_t sessionId, uint32_t playerId) {
	std::unique_lock<std::shared_mutex> lock(rw_lock);

	auto result = sessionIdToPlayerIdMap.insert({ sessionId, playerId });
	
	return result.second;
}

uint32_t SessionManager::findPlayerIdBySessionId(uint32_t sessionId) const {
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	auto it = sessionIdToPlayerIdMap.find(sessionId);
	
	if (it == sessionIdToPlayerIdMap.end()) {
		return 0; // Not Found
	}
	
	return it->second;
}

bool SessionManager::deleteBySessionId(uint32_t sessionId) {
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	auto it = sessionIdToPlayerIdMap.find(sessionId);
	
	if (it == sessionIdToPlayerIdMap.end()) {
		return false; // Not Found
	}
	
	sessionIdToPlayerIdMap.erase(it);
	return true;
}