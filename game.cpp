#include "game.h"

// Constructor
Game::Game() {
	projection = Ortho(0.0f, GAME_WIDTH, 0.0f, GAME_HEIGHT, -2, 2);
	timer_laser = 0;
	tagger_id = -1;
	is_start = false;
	my_id = -1;
	timer_update = 0;
	timer = 0;
	is_game = 1;

	cout << "Input Server Address: ";
	cin >> server_address;
	cout << "Input Server Port: ";
	cin >> server_port;
	client.init(server_address, server_port, { 0, 0 });
	
	cout << "Loading..." << endl;
	shaderInit();
	textureInit();
	addLaser();
	ending.setStaticObj({ 480, 270 }, 540, 2, loc);
	my_id = client.getParam().packets[0]->id;
	vector<OBSTACLE_PACKET*> obstacle = client.getObstaclePos();
	cout << obstacle.size() << endl;

	for (int i = 0; i < obstacle.size(); i++) {
		struct Obj *obj = new Obj();
		cout << obstacle[i]->pos.x << "  " << obstacle[i]->pos.y << " " << obstacle[i]->type << endl;
		obj->setStaticObj({ obstacle[i]->pos.x, obstacle[i]->pos.y }, PLAYER_SIZE, 0, loc);
		obj->setTexture(texture_player[obstacle[i]->type].getBuf());
		trash_objects.push_back(obj);
	}
	cout << "Finish Loading" << endl;
	addBlock();
}

// Init shader
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

// Init texture 
void Game::textureInit() {
	texture_tagger.load("img/tagger.png");
	texture_player[0].load("img/frog.png");
	texture_player[1].load("img/acorn.png");
	texture_player[2].load("img/dog.png");
	texture_flashLight.load("img/flashlight.png");
	texture_laser.load("img/laser_circle.png");
	texture_block.load("img/block.png");
	texture_tagger_win.load("img/tagger_win.png");
	texture_runner_win.load("img/runner_win.png");
}

// Add new Player
void Game::addNewPlayer() {
	Player *new_player;

	new_player = new Player();
	new_player->setShaderBuffer(loc);
	new_player->setBuffer();
	player.push_back(new_player);
}

// Add all players using player numbers
void Game::addPlayers() {
	int player_size = client.getParam().packets.size();

	for (int i = 0; i < player_size; i++)
		addNewPlayer();
	client.sortClient();
}

// Add flash light to game 
void Game::addFlashlight() {
	struct pos flash_pos = player[my_id]->getPos();

	flashlight.setStaticObj(flash_pos, 1920, 1.2, loc);
	flashlight.setTexture(texture_flashLight.getBuf());
}

// Add Laser to game
void Game::addLaser() {
	struct pos init_pos = { 480, 270 };

	laser.setStaticObj(init_pos, PLAYER_SIZE, 0, loc);
	laser.setTexture(texture_laser.getBuf());
}

// Draw Laser
void Game::drawLaser() {	
	laser.draw();
	laser.animateMove();
	timer_laser--;
}

// Laser shoot
void Game::shoot() {
	const int boundary = PLAYER_SIZE * 3;
	struct pos tagger_pos = player[tagger_id]->getPos();

	timer_laser = LASER_TIMER;
	laser.setAnimatePosition({ tagger_pos.x, tagger_pos.y });
	laser.setPos({ tagger_pos.x, tagger_pos.y });

	switch (player[tagger_id]->getDir()) {
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
}

// Update function
void Game::update() {
	int player_size = player.size();
	static int cnt = 0;
	struct pos p;

	for (int i = 0; i < player_size; i++) {
		p = { client.getParam().packets[i]->pos.x, client.getParam().packets[i]->pos.y };
		player[i]->setPos(p);
		if (cnt < player_size) {
			STATE s;
			s = static_cast<STATE>(client.getParam().packets[i]->state);
			player[i]->setState(s);			
			player[i]->setId(client.getParam().packets[i]->id);
			cnt++;

			if (s == STATE_TAGGER)
				tagger_id = client.getParam().packets[i]->id;			
		}
	}	
}

// Update position when occur input until finish
void Game::polling() {
	while (1) {
		if (client.getIsUpdate()) {
			update();
			client.setIsUpdate();
			break;
		}
	}
}

// Call every tick
void Game::tick() {
	int hit, i;
	static int time = 0;
	STATE s;
	time++;

	if (is_game > 1)
		return;

	// runner win
	if (timer > 60) {
		cout << "BACK" << endl;
		ending.setTexture(texture_runner_win.getBuf());
		is_game = 3;
	}

	// tagger win
	if (client.getIsT()) {
		cout << "END" << endl;
		ending.setTexture(texture_tagger_win.getBuf());
		is_game = 2;
	}

	// calculate time
	if (time == GLOBAL_TIMER) {
		time = 0;
		timer++;
	}

	// When anyone press button
	if (client.getIsUpdate()) {
		update();
		client.setIsUpdate();
	}

	// When tagger press 'G' button to start game
	if (client.getIsG()) {
		addPlayers();
 		update();
		for (i = 0; i < player.size(); i++) {
			s = player[i]->getState();
			switch (s) {
			case STATE_TAGGER:
				player[i]->setTexture(texture_tagger.getBuf());
				break;
			default:
				player[i]->setTexture(texture_player[s - 2].getBuf());
				break;
			}
		}
		is_start = true;
		timer = 0;
		addFlashlight();
		client.setIsG();
	}

	// When tagger shoot using 'H' button
	if (client.getIsH()) {
		shoot();
		hit = laserCollision();
		if (hit = !tagger_id) {
			cout << "Hit to " << hit << endl;
			client.sendPacket({ player[my_id]->getPos().x,  player[my_id]->getPos().y }, 't', my_id);
		}
		client.setIsH();
	}
}

void Game::drawBlock() {
	const int width = GAME_WIDTH / PLAYER_SIZE;
	const int height = GAME_HEIGHT / PLAYER_SIZE;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			block.setAnimatePosition({ i*PLAYER_SIZE + PLAYER_SIZE / 2, j*PLAYER_SIZE + PLAYER_SIZE / 2 });
			block.draw();
		}
	}	
}

void Game::addBlock() {
	block.setStaticObj({ 0, 0 }, PLAYER_SIZE, 0.0, loc);
	block.setTexture(texture_block.getBuf());
}

// keyboard function
void Game::keyboard(unsigned char key) {
	int ran;
	int random_player;
	struct pos p;
	STATE r;
	struct pos my_pos;

	if (is_game > 1)
		return;

	switch (key) {
	// Change direction
	case 'w':
		player[my_id]->setDir(DIR_W);
		flashlight.setAngle(180);
		break;
	case 'e':
		player[my_id]->setDir(DIR_E);
		flashlight.setAngle(135);
		break;
	case 'd':
		player[my_id]->setDir(DIR_D);
		flashlight.setAngle(90);
		break;
	case 'c':
		player[my_id]->setDir(DIR_C);
		flashlight.setAngle(45);
		break;
	case 'x':
		player[my_id]->setDir(DIR_X);
		flashlight.setAngle(0);
		break;
	case 'z':
		player[my_id]->setDir(DIR_Z);
		flashlight.setAngle(-45);
		break;
	case 'a':
		player[my_id]->setDir(DIR_A);
		flashlight.setAngle(-90);
		break;
	case 'q':
		player[my_id]->setDir(DIR_Q);
		flashlight.setAngle(-135);
		break;
	// Move
	case 'j':
		my_pos = player[my_id]->getPos();
		switch (player[my_id]->getDir()) {
		case DIR_W:
			p = { my_pos.x, my_pos.y + PLAYER_SIZE };
			break;
		case DIR_E:
			p = { my_pos.x + PLAYER_SIZE, my_pos.y + PLAYER_SIZE };
			break;
		case DIR_D:
			p = { my_pos.x + PLAYER_SIZE, my_pos.y };
			break;
		case DIR_C:
			p = { my_pos.x + PLAYER_SIZE, my_pos.y - PLAYER_SIZE };
			break;
		case DIR_X:
			p = { my_pos.x, my_pos.y - PLAYER_SIZE };
			break;
		case DIR_Z:
			p = { my_pos.x - PLAYER_SIZE, my_pos.y - PLAYER_SIZE };
			break;
		case DIR_A:
			p = { my_pos.x - PLAYER_SIZE, my_pos.y };
			break;
		case DIR_Q:
			p = { my_pos.x - PLAYER_SIZE, my_pos.y + PLAYER_SIZE };
			break;
		}
		if (!isCollision(p)) {
			client.sendPacket({ p.x, p.y }, 'j', my_id);
			polling();
			flashlight.setPos({ p.x, p.y });
		}
		break;
	case 'h':
		if (my_id != tagger_id)
			break;
		my_pos = player[my_id]->getPos();
		p = { my_pos.x, my_pos.y };
		client.sendPacket({ p.x, p.y }, 'h', my_id);
		break;
	case 'g':
		if (my_id != 0)
			break;
		client.sendPacket({ client.getParam().packets[0]->pos.x, client.getParam().packets[0]->pos.y }, 'g', my_id);
		break;
	case 't':
		exit(0);
	default: cout << "Illigal Input " << key << endl;
	}
}

// Get player
Player * Game::getPlayer(int n) {
	return player[0];
}

// Draw function, every tick
void Game::draw() {
	static int cnt = 0;
	if (is_start) {
		glUniformMatrix4fv(loc.projection, 1, GL_TRUE, projection);
		drawBlock();
		for (int i = 0; i < player.size(); i++)
			player[i]->drawPlayer();

		if (timer_laser > 0)
			drawLaser();

		for (int i = 0; i < trash_objects.size(); i++) {
			trash_objects[i]->draw();
		}

		flashlight.draw();
		flashlight.animateMove();

		if (is_game > 1)
			ending.draw();
	}	
}

// Caculate laser collision
int Game::laserCollision() {
	struct pos tagger_pos = player[tagger_id]->getPos();
	struct pos player_pos;
	const int boundary = PLAYER_SIZE * 3 + 1;
	int min = 10000;
	int num = -1;

	for (int i = 0; i < player.size(); i++) {
		if (i == tagger_id)
			continue;

		player_pos = player[i]->getPos();
		switch (player[tagger_id]->getDir()) {
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

// Cacluate collistion using position
bool Game::isCollision(struct pos p){
	int obj_size = trash_objects.size();
	int player_size = player.size();

	for (int i = 0; i < obj_size; i++) {
		if (p.x == trash_objects[i]->getPos().x &&
			p.y == trash_objects[i]->getPos().y)
			return true;
	}

	for (int i = 0; i < player_size; i++) {
		if (i == my_id)
			continue;
		if (player[i]->getPos().x == p.x &&
			player[i]->getPos().y == p.y)
			return true;
	}
	if (p.x < 0 || p.x > GAME_WIDTH || p.y < 0 || p.y > GAME_HEIGHT)
		return true;

	return false;
}
