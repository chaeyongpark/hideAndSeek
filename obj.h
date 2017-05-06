#pragma once
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include "Angel.h"
#include "texture.h"

#define PLAYER_SIZE 40
#define ANIMATION_SPEED 7

using namespace std;

struct pos {
	int x;
	int y;
};

struct location {
	GLuint vertex;
	GLuint color;
	GLuint texture;
	GLuint model;
	GLuint projection;
};

class Obj {
private:
	struct pos position;
	struct pos animate_position;
	int width, height;
	Texture texture;

	/* shader */
	struct location loc;
	GLuint buf_address;
	vector<vec3> vec_ver;
	vector<vec4> vec_col;
	vector<vec2> vec_texture;
	int num_vertex = 6;

public:
	/* Constructor */
	Obj();
	
	/* Buffer maybe do not use */
	void setBuffer();
	void setShaderBuffer(struct location _loc);
	struct location getLocation();

	/* Game */
	struct pos getPosition();
	void setPos(int _x, int _y);
	void setPos(struct pos _pos);
	void setAnimatePosition(struct pos _pos);
	void setAll(struct pos _pos, int _width, int _height);
	void setTexture(char * file_name);
	void animateMove();
	void draw();
	void makeRect(float x1, float y1, float x2, float y2, float z);
	void makeRect(float x1, float y1, float size, float z);
};