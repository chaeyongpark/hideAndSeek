#pragma once
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <cstdlib>
#include "Angel.h"
#include "obj.h"

#define NUM_VERTEX 6

using namespace std;

typedef enum state { STATE_NONE, STATE_TAGGER, STATE_A, STATE_B, STATE_C } STATE;
typedef enum direction { DIR_W, DIR_E, DIR_D, DIR_C, DIR_X, DIR_Z, DIR_A, DIR_Q } DIRECTION;

class Player : public Obj {
private:
	int id;	
	STATE state;
	mat4 projection;
	struct pos init_pos;
	DIRECTION dir;

public:
	Player();
	Player(int _id, struct pos p);
	Player(STATE _state, int _id, struct pos p);
	void setId(int _id);
	void playerInit(struct pos p);
	struct pos getInitPos();
	void move(int _x, int _y);
	void drawPlayer();
	DIRECTION getDir();
	void setState(STATE _state);
	STATE getState();
};