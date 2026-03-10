#include "Player.h"

Player::Player(uint32_t id) :
    id(id) {}


uint32_t Player::getId() const {
    return this->id;
}