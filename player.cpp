#include "player.h"

// Constructor
Player::Player() {	
	dir = DIR_A;

	makeRect(0, 0, PLAYER_SIZE, 0);
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

// Set player ID
void Player::setId(int _id) {
	id = _id;
}

// Init player
void Player::playerInit(struct pos p) {
	dir = DIR_A;

	// set information of Player
	setAnimatePosition(p);
	setAll(p, PLAYER_SIZE, PLAYER_SIZE);
	makeRect(0, 0, PLAYER_SIZE, 0);
}

// Get initial position
struct pos Player::getInitPos() {
	return init_pos;
}

// Move function
void Player::move(int _x, int _y) {
	setPos(getPos().x + _x, getPos().y + _y);
}

// Draw player
void Player::drawPlayer() {
	draw();
	animateMove();
}

// Get flash light direction of player
DIRECTION Player::getDir() {
	return dir;
}

// Set state
void Player::setState(STATE _state) {
	state = _state;
}

// Get state
STATE Player::getState() {
	return state;
}

