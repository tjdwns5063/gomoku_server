#include "PlayerRepository.h"
#include "Player.h"

bool PlayerRepository::save(const std::shared_ptr<Player>& player) {
	if (!player) return false;

	std::unique_lock<std::shared_mutex> lock(rw_lock);
	auto result = players.insert({ player->getId(), player });
	return result.second;
}

std::shared_ptr<Player> PlayerRepository::findById(uint32_t id) const {
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	auto it = players.find(id);

	if (it == players.end()) {
		return nullptr;
	}

	return it->second;
}

bool PlayerRepository::deleteById(uint32_t id) {
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	auto it = players.find(id);

	if (it == players.end()) {
		return false;
	}

	players.erase(it);

	return true;
}