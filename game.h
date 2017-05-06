#pragma once
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "player.h"
#include "Angel.h"
#include <cmath>

#define SERVER_ADDRESS 1232112313123123
#define GAME_WIDTH 960
#define GAME_HEIGHT 540
#define LASER_TIMER 10
#define OBJECT_NUM 10

using namespace std;

class Game {
private:
	int timer;
	int tagger_num;
	int my_id;
	bool is_start;
	vector<Player*> player;
	vector<Obj*> trash_objects;
	Obj flashlight;

	// laser
	Obj laser;
	Texture texture_laser;
	int timer_laser;

	// object shader
	struct location loc;
	mat4 projection;
	Texture texture_tagger, texture_flashLight;
	Texture texture_player[3];

	// Network
	char server_address[16];

public:
	Game();
	void shaderInit();
	void textureInit();
	void addFlashlight();
	void addLaser();
	void setState();
	void makeRandomObj();
	struct pos genRandomPos();
	void addNewPlayer();
	void keyboard(unsigned char key);
	Player* getPlayer(int n);
	void draw();
	void drawLaser();
	void shoot();

	int laserCollision();
	bool isCollision();
};