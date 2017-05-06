#include "player.h"

Player::Player() {	
	// Debug
	cout << "Empty Plyaer Constructor" << endl;
}

Player::Player(int _id) {
	state = static_cast<STATE>(rand() % 3+2);
	id = _id;

	playerInit();

	// Debug
	cout << "Player Constructor Using ID" << endl;
}

Player::Player(STATE _state, int _id) {
	state = _state;
	id = _id;

	playerInit();
	// Debug
	cout << "Player Constructor Using State and ID" << endl;
}

void Player::playerInit() {
	init_pos = { 480, 270 };
	dir = DIR_A;

	// set information of Player
	setAnimatePosition(init_pos);
	setAll(init_pos, PLAYER_SIZE, PLAYER_SIZE);
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

STATE Player::getState() {
	return state;
}
