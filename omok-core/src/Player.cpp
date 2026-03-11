#include "Player.h"

Player::Player(uint32_t id) :
    id(id) {}


uint32_t Player::getId() const {
    return this->id;
}

void Player::enterMatchQueue() {
    isMatching.exchange(true);
}

void Player::leaveMatchQueue() {
    isMatching.exchange(false);
}

bool Player::isMatch() const {
    return isMatching.load();
}