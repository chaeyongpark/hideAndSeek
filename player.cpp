#include "player.h"

Player::Player() {	
}

Player::Player(int _id, struct pos p) {
	state = static_cast<STATE>(rand() % 3+2);
	id = _id;

	playerInit(p);
}

Player::Player(STATE _state, int _id, struct pos p) {
	state = _state;
	id = _id;

	playerInit(p);
}

void Player::playerInit(struct pos p) {
	dir = DIR_A;

	// set information of Player
	setAnimatePosition(p);
	setAll(p, PLAYER_SIZE, PLAYER_SIZE);
	makeRect(0, 0, PLAYER_SIZE, 0);
}

struct pos Player::getInitPos() {
	return init_pos;
}

void Player::move(int _x, int _y) {
	setPos(getPos().x + _x, getPos().y + _y);
	cout << id << " MOVE" << " x: " << getPos().x << "y:" << getPos().y << endl;
}

void Player::drawPlayer() {
	draw();
	animateMove();
}

DIRECTION Player::getDir() {
	return dir;
}

void Player::setState(STATE _state) {
	state = _state;
}

STATE Player::getState() {
	return state;
}
