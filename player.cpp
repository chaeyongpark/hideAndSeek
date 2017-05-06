#include "player.h"

Player::Player() {	
	// Debug
	cout << "Empty Plyaer Constructor" << endl;
}

Player::Player(STATE _state, int _id) {
	state = _state;
	id = _id;
	init_pos = { 480, 270 };
	dir = DIR_A;

	// set information of Player
	setAnimatePosition(init_pos);
	setAll(init_pos, PLAYER_SIZE, PLAYER_SIZE);
	makeRect(0, 0, PLAYER_SIZE, 0);

	// Debug
	cout << "Plyaer Constructor" << endl;
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
