#include "player.h"

Player::Player() {	
	init_pos = { 480, 270 };
	
	// set information of Player
	setAnimatePosition(init_pos);
	setAll(init_pos, PLAYER_SIZE, PLAYER_SIZE);
	makeRect(0, 0, PLAYER_SIZE, 0);
	state = NONE;
	name = "CSED353";

	// Debug
	cout << "Plyaer Constructor" << endl;
}

struct pos Player::getInitPos() {
	return init_pos;
}

void Player::move(int _x, int _y) {
	setPos(getPosition().x + _x, getPosition().y + _y);
}

void Player::drawPlayer() {
	draw();
	animateMove();
}
