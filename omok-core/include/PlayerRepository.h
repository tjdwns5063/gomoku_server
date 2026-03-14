#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <memory>

class Player;

class IdGenerator;

class PlayerRepository {

private:
	std::unordered_map<uint32_t, std::shared_ptr<Player>> players;
	mutable std::shared_mutex rw_lock;

public:
	bool save(const std::shared_ptr<Player>& player);
	std::shared_ptr<Player> findById(uint32_t id) const;
	bool deleteById(uint32_t id);
};
