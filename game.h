#pragma once
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "player.h"
#include "Angel.h"

#define GAME_WIDTH 960
#define GAME_HEIGHT 540

using namespace std;

class Game {
private:
	int timer;
	vector<Player*> player;
	mat4 projection;
	Obj flashlight;

	/* buffer */
	struct location loc;

public:
	Game();
	void shaderInit();
	void addNewPlayer();
	void addFlashlight();
	void keyboard(unsigned char key);
	Player* getPlayer(int n);
	void draw();
};