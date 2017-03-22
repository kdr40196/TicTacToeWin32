#include "stdafx.h"
#include"Player.hpp"

Player::Player(int id, HBRUSH brush) {
	//LoadIcon(...)
	this->brush = brush;
	this->id = id;
}

Player& Player::operator = (Player& other) {
	Player temp(other.id, other.brush);
	return temp;
}

Player::~Player() {
	id = 0;
	DeleteObject(brush);
	//DestroyIcon
}

bool Player::operator == (Player& other) {
	if (other.id == id)
		return true;
	return false;
}