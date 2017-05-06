#pragma once
#include <GL/glut.h>

class Texture {
private:
	unsigned w, h;
	GLuint buf;

public:
	bool load(char* fn);
	void bind();
	GLuint getBuf();
};