#pragma once
#include <iostream>
#include "player.h"

typedef enum direction {UP, DOWN, RIGHT, LEFT} DIRECTION;

class Tagger : public Player {
private:
	DIRECTION direction;

public:
	int shoot();
};