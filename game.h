#pragma once
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "player.h"
#include "Angel.h"
#include <cmath>

#define GAME_WIDTH 960
#define GAME_HEIGHT 540
#define LASER_TIMER 10

using namespace std;

class Game {
private:
	int timer;
	int tagger_num;
	int my_id;
	vector<Player*> player;
	mat4 projection;
	Obj flashlight;
	Texture texture_tagger, texture_A, texture_flashLight;
	
	// laser
	Obj laser;
	Texture texture_laser;
	int timer_laser;

	// buffer
	struct location loc;

public:
	Game();
	void shaderInit();
	void textureInit();
	void addNewPlayer(STATE _state);
	void addFlashlight();
	void addLaser();
	void keyboard(unsigned char key);
	Player* getPlayer(int n);
	void draw();
	void drawLaser();
	void shoot();
	int isCollision();
};