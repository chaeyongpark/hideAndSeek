#include "game.h"

Game::Game() {
	projection = Ortho(0.0f, GAME_WIDTH, 0.0f, GAME_HEIGHT, -2, 2);
	timer_laser = 0;

	cout << "Game Constructor" << endl;
	cout << "Loading..." << endl;
	shaderInit();
	textureInit();	
	addLaser();
	cout << "Finish Loading" << endl;
}

void Game::shaderInit() {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	loc.vertex = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc.vertex);
	loc.color = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc.color);
	loc.texture = glGetAttribLocation(program, "vTex");
	glEnableVertexAttribArray(loc.texture);
	loc.model = glGetUniformLocation(program, "model_view");
	loc.projection = glGetUniformLocation(program, "projection");

	glUniform1i(glGetUniformLocation(program, "texture_img"), 0);
}

void Game::textureInit() {
	texture_tagger.load("img/tagger.png");
	texture_A.load("img/acorn.png");
	texture_flashLight.load("img/flashLight.png");
	texture_laser.load("img/laser_circle.png");
}

void Game::addNewPlayer(STATE _state) {
	Player *new_player = new Player(_state, player.size());
	new_player->setShaderBuffer(loc);
	new_player->setBuffer();

	player.push_back(new_player);
}

void Game::addFlashlight() {
	struct pos init_pos = player[0]->getInitPos();

	flashlight.setAnimatePosition(init_pos);
	flashlight.setAll(init_pos, 1920, 1920);
	flashlight.makeRect(0, 0, 1920, 1.0);
	flashlight.setShaderBuffer(loc);
	flashlight.setBuffer();
	flashlight.setTexture(texture_flashLight.getBuf());
}

void Game::addLaser() {
	struct pos init_pos = { 480, 270 };

	laser.setAnimatePosition(init_pos);
	laser.setAll(init_pos, PLAYER_SIZE, PLAYER_SIZE);
	laser.makeRect(0, 0, PLAYER_SIZE, 1.1);
	laser.setShaderBuffer(loc);
	laser.setBuffer();
	laser.setTexture(texture_laser.getBuf());
}

void Game::drawLaser() {	
	laser.draw();
	laser.animateMove();
	timer_laser--;
	//cout << timer_laser << endl;
}

void Game::shoot() {
	const int boundary = PLAYER_SIZE * 3;
	
	timer_laser = LASER_TIMER;
	laser.setAnimatePosition({ player[tagger_num]->getPos().x, player[tagger_num]->getPos().y });
	laser.setPos({ player[tagger_num]->getPos().x, player[tagger_num]->getPos().y });

	switch (player[tagger_num]->getDir()) {
	case DIR_W:
		laser.setPos(laser.getPos().x, laser.getPos().y + boundary);
		break;
	case DIR_E:
		laser.setPos(laser.getPos().x + boundary, laser.getPos().y + boundary);
		break;
	case DIR_D:
		laser.setPos(laser.getPos().x + boundary, laser.getPos().y);
		break;
	case DIR_C:
		laser.setPos(laser.getPos().x + boundary, laser.getPos().y - boundary);
		break;
	case DIR_X:
		laser.setPos(laser.getPos().x, laser.getPos().y - boundary);
		break;
	case DIR_Z:
		laser.setPos(laser.getPos().x - boundary, laser.getPos().y - boundary);
		break;
	case DIR_A:
		laser.setPos(laser.getPos().x - boundary, laser.getPos().y);
		break;
	case DIR_Q:
		laser.setPos(laser.getPos().x - boundary, laser.getPos().y + boundary);
		break;
	}
	cout << laser.getPos().x << " " << laser.getPos().y << endl;
}

void Game::keyboard(unsigned char key) {
	int hit;

	switch (key) {
	case 'w':
		player[0]->move(0, PLAYER_SIZE);
		break;
	case 'a':
		player[0]->move(-PLAYER_SIZE, 0);
		break;
	case 's':
		player[0]->move(0, -PLAYER_SIZE);
		break;
	case 'd':
		player[0]->move(PLAYER_SIZE, 0);
		break;
	case 'i':
		player[1]->move(0, PLAYER_SIZE);
		break;
	case 'j':
		player[1]->move(-PLAYER_SIZE, 0);
		break;
	case 'k':
		player[1]->move(0, -PLAYER_SIZE);
		break;
	case 'l':
		player[1]->move(PLAYER_SIZE, 0);
		break;
	case '1':
		addNewPlayer(STATE_TAGGER);
		tagger_num = player.size() - 1;
		player.back()->setTexture(texture_tagger.getBuf());
		break;
	case '2':
		addNewPlayer(STATE_A);
		player.back()->setTexture(texture_A.getBuf());
		break;
	case 'h':
		cout << "INPUT HIT" << endl;
		shoot();
		hit = isCollision();
		if (hit >= 0)
			cout << "===" << hit << " HIT!===" << endl;
		break;
	case 't':
		exit(0);
	default: cout << "ILLIGAL INPUT " << key << endl;
	}
}

Player * Game::getPlayer(int n) {
	return player[0];
}

void Game::draw() {
	glUniformMatrix4fv(loc.projection, 1, GL_TRUE, projection);
	for (int i = 0; i < player.size(); i++)
		player[i]->drawPlayer();

	if (timer_laser > 0)
		drawLaser();

	//flashlight.draw();
	//flashlight.animateMove();
}

int Game::isCollision() {
	struct pos tagger_pos = player[tagger_num]->getPos();
	struct pos player_pos;
	const int boundary = PLAYER_SIZE * 3 + 1;
	int min = 10000;
	int num = -1;

	for (int i = 0; i < player.size(); i++) {
		if (i == tagger_num)
			continue;
		player_pos = player[i]->getPos();
		switch (player[tagger_num]->getDir()) {
		case DIR_W:
			if (tagger_pos.x == player_pos.x && tagger_pos.y < player_pos.y && player_pos.y < (tagger_pos.y + boundary)) {
				if (abs(player_pos.y - tagger_pos.y) < min) {
					min = abs(player_pos.y - tagger_pos.y);
					num = i;
				}
			}
			break;
		case DIR_X:
			if (tagger_pos.x == player_pos.x && tagger_pos.y > player_pos.y && player_pos.y > (tagger_pos.y - boundary)) {
				if (abs(player_pos.y - tagger_pos.y) < min) {
					min = abs(player_pos.y - tagger_pos.y);
					num = i;
				}
			}
			break;
		case DIR_D:
			if (tagger_pos.y == player_pos.y && tagger_pos.x < player_pos.x && player_pos.x < (tagger_pos.x + boundary)) {
				if (abs(player_pos.x - tagger_pos.x) < min) {
					min = abs(player_pos.x - tagger_pos.x);
					num = i;
				}
			}
			break;
		case DIR_A:
			if (tagger_pos.y == player_pos.y && tagger_pos.x > player_pos.x && player_pos.x > (tagger_pos.x - boundary)) {
				if (abs(player_pos.x - tagger_pos.x) < min) {
					min = abs(player_pos.x - tagger_pos.x);
					num = i;
				}
			}
			break;
		case DIR_E: case DIR_C:
			if (tagger_pos.y / tagger_pos.y == player_pos.y / player_pos.x && tagger_pos.x < player_pos.x && player_pos.x < (tagger_pos.x + boundary)) {
				if (abs(player_pos.x - tagger_pos.x) < min) {
					min = abs(player_pos.x - tagger_pos.x);
					num = i;
				}
			}
			break;
		case DIR_Q: case DIR_Z:
			if (tagger_pos.y / tagger_pos.y == player_pos.y / player_pos.x && tagger_pos.x > player_pos.x && player_pos.x > (tagger_pos.x - boundary)) {
				if (abs(player_pos.x - tagger_pos.x) < min) {
					min = abs(player_pos.x - tagger_pos.x);
					num = i;
				}
			}
			break;
		}
	}
	return num;
}
