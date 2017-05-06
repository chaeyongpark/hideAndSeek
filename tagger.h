#pragma once
#include <iostream>
#include "player.h"

class Tagger : public Player {
private:
	DIRECTION direction;

public:
	int shoot();
};