#pragma once
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include "Angel.h"
#include "obj.h"

#define NUM_VERTEX 6

using namespace std;

typedef enum state { NONE, TAGGER, A, B, C } STATE;

class Player : public Obj {
private:
	int id;	
	STATE state;
	string name;
	mat4 projection;
	struct pos init_pos;

public:
	Player();
	struct pos getInitPos();
	//void keyboard(unsigned char key);
	void move(int _x, int _y);
	void drawPlayer();
};