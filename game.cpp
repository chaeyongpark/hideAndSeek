#include "game.h"

Game::Game() {
	projection = Ortho(0.0f, GAME_WIDTH, 0.0f, GAME_HEIGHT, -2, 2);
	timer_laser = 0;
	tagger_num = -1;
	is_start = false;

	cout << "Input Server Address: ";
	cin >> server_address;

	cout << "Loading..." << endl;
	shaderInit();
	textureInit();
	makeRandomObj();
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
	texture_player[0].load("img/frog.png");
	texture_player[1].load("img/acorn.png");
	texture_player[2].load("img/dog.png");
	texture_flashLight.load("img/flashLight.png");
	texture_laser.load("img/laser_circle.png");
}

void Game::addNewPlayer() {
	Player *new_player;
	struct pos p = genRandomPos();

	new_player = new Player(player.size(), p);
	new_player->setShaderBuffer(loc);
	new_player->setBuffer();

	player.push_back(new_player);
}

void Game::addFlashlight() {
	struct pos init_pos = player[0]->getInitPos();

	flashlight.setStaticObj(init_pos, 1920, 1.2, loc);
	flashlight.setTexture(texture_flashLight.getBuf());
}

void Game::addLaser() {
	struct pos init_pos = { 480, 270 };

	laser.setStaticObj(init_pos, PLAYER_SIZE, 0, loc);
	laser.setTexture(texture_laser.getBuf());
}

void Game::setState() {
	int player_size = player.size();
	int r;
	tagger_num = rand() % player_size;

	for (int i = 0; i < player_size; i++) {
		if (i == tagger_num) {
			player[i]->setState(STATE_TAGGER);
			player[i]->setTexture(texture_tagger.getBuf());
		}
		else {
			r = rand() % 3;
			player[i]->setState(static_cast<STATE>(r+2));
			player[i]->setTexture(texture_player[r].getBuf());
		}
	}
	is_start = true;
}

void Game::makeRandomObj() {
	struct pos r;
	bool check = false;

	while (trash_objects.size() != OBJECT_NUM) {
		check = false;
		r = { rand() % 12 * PLAYER_SIZE + PLAYER_SIZE / 2, rand() % 6 * PLAYER_SIZE + PLAYER_SIZE / 2 };
		for (int i = 0; i < trash_objects.size(); i++) {
			if (r.x == trash_objects[i]->getPos().x && r.y == trash_objects[i]->getPos().y) {
				check = true;
				break;
			}
		}
		if (check)
			continue;
		struct Obj *obj = new Obj();
		obj->setStaticObj({ r.x, r.y }, PLAYER_SIZE, 0, loc);
		obj->setTexture(texture_player[rand() % 3].getBuf());
		trash_objects.push_back(obj);
	}
	cout << "Finish Making Random Objects" << endl;
}

struct pos Game::genRandomPos() {
	struct pos r;
	bool check;

	while (1) {
		check = false;
		cout << "Generating Random Number..." << endl;
		r = { rand() % 12 * PLAYER_SIZE + PLAYER_SIZE / 2, rand() % 6 * PLAYER_SIZE + PLAYER_SIZE / 2 };
		for (int i = 0; i < OBJECT_NUM; i++) {
			if (r.x == trash_objects[i]->getPos().x && r.y == trash_objects[i]->getPos().y) {
				check = true;
				cout << "Generate ERROR" << endl;
				break;
			}
		}
		for (int i = 0; i < player.size(); i++) {
			if (r.x == player[i]->getPos().x && r.y == player[i]->getPos().y) {
				check = true;
				cout << "Generate ERROR" << endl;
				break;
			}
		}
		if (check)
			continue;
		else
			break;
	}
	return r;
}

void Game::drawLaser() {	
	laser.draw();
	laser.animateMove();
	timer_laser--;
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
	int random_player;
	STATE r;

	vector<struct pos> player_pos;

	for (int i = 0; i < player.size(); i++)
		player_pos.push_back(player[i]->getPos());

	switch (key) {
	case 'w':
		//player[0]->move(0, PLAYER_SIZE);
		player[0]->setPos(player[0]->getPos().x, player[0]->getPos().y + PLAYER_SIZE);
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
	case '1': case '2': case '3': case'4':
		addNewPlayer();
		break;
	case 'h':
		shoot();
		hit = laserCollision();
		if (hit >= 0)
			cout << "===" << hit << " HIT!===" << endl;
		break;
	case 't':
		exit(0);
	case 'g':
		setState();
		break;
	default: cout << "Illigal Input " << key << endl;
	}

	if (isCollision()) {
		cout << "Object Collision" << endl;
		for (int i = 0; i < player.size(); i++)
			player[i]->setPos(player_pos[i]);
	}

}

Player * Game::getPlayer(int n) {
	return player[0];
}

void Game::draw() {
	if (is_start) {
		glUniformMatrix4fv(loc.projection, 1, GL_TRUE, projection);
		for (int i = 0; i < player.size(); i++)
			player[i]->drawPlayer();

		if (timer_laser > 0)
			drawLaser();

		for (int i = 0; i < OBJECT_NUM; i++) {
			trash_objects[i]->draw();
		}
	}
	//flashlight.draw();
	//flashlight.animateMove();
}

int Game::laserCollision() {
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

bool Game::isCollision(){
	int obj_size = trash_objects.size();
	int player_size = player.size();
	for (int i = 0; i < player_size; i++) {
		for (int j = 0; j < obj_size; j++) {
			if (player[i]->getPos().x == trash_objects[j]->getPos().x &&
				player[i]->getPos().y == trash_objects[j]->getPos().y)
				return true;
		}
		for (int j = player_size - 1; j > i; j--) {
			if (player[i]->getPos().x == player[j]->getPos().x &&
				player[i]->getPos().y == player[j]->getPos().y)
				return true;
		}
	}
	return false;
}
