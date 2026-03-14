#pragma once

#include <unordered_map>
#include <shared_mutex>

class SessionManager {
private:
	std::unordered_map<uint32_t, uint32_t> sessionIdToPlayerIdMap;
	mutable std::shared_mutex rw_lock;

public:
	bool save(uint32_t sessionId, uint32_t playerId);
	uint32_t findPlayerIdBySessionId(uint32_t sessionId) const;
	bool deleteBySessionId(uint32_t sessionId);
};