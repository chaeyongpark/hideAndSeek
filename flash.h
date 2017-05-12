#pragma once
#include <iostream>
#include "obj.h"
#include <cmath>

class Flash : public Obj {
private:
	int dir;
	mat4 transform;
	int angle;
	int animate_angle;

public:
	Flash();
	void draw();
	void setDir(int _dir);
	void setAngle(int _angle);
	void animateRotate();
};