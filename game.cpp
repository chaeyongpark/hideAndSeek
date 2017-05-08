#include "game.h"

// Constructor
Game::Game() {
	projection = Ortho(0.0f, GAME_WIDTH, 0.0f, GAME_HEIGHT, -2, 2);
	timer_laser = 0;
	tagger_id = -1;
	is_start = false;
	my_id = -1;
	timer_update = 0;

	cout << "Input Server Address: ";
	cin >> server_address;
	cout << "Input Server Port: ";
	cin >> server_port;
	client.init(server_address, server_port, { 0, 0 });
	
	cout << "Loading..." << endl;
	shaderInit();
	textureInit();
	//makeRandomObj();
	addLaser();
	cout << "Finish Loading" << endl;
	my_id = client.getParam().packets[0]->id;
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
	texture_flashLight.load("img/flashLight.png");
	texture_laser.load("img/laser_circle.png");
}

// Add new Player
void Game::addNewPlayer() {
	Player *new_player;
	//struct pos p = genRandomPos();
	//my_id = player.size();
	new_player = new Player();
	new_player->setShaderBuffer(loc);
	new_player->setBuffer();

	player.push_back(new_player);
}

// Add all players using player numbers
void Game::addPlayers() {
	int player_size = client.getParam().packets.size();
	cout << player_size << endl;
	
	for (int i = 0; i < player_size; i++)
		addNewPlayer();
	//cout << client.getParam().packets[0]->id << client.getParam().packets[1]->id << endl;
	//cout << client.getParam().packets[0]->id << client.getParam().packets[1]->id << endl;
}

// Add flash light to game 
void Game::addFlashlight() {
	struct pos init_pos = player[0]->getInitPos();

	flashlight.setStaticObj(init_pos, 1920, 1.2, loc);
	flashlight.setTexture(texture_flashLight.getBuf());
}

// Add Laser to game
void Game::addLaser() {
	struct pos init_pos = { 480, 270 };

	laser.setStaticObj(init_pos, PLAYER_SIZE, 0, loc);
	laser.setTexture(texture_laser.getBuf());
}

// Set state
void Game::setState() {
	int player_size = player.size();
	int r;
	tagger_id = rand() % player_size;

	for (int i = 0; i < player_size; i++) {
		if (i == tagger_id) {
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

// Make random objects (maybe not used)
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

// Generate random position (maybe not used)
struct pos Game::genRandomPos() {
	struct pos r;
	bool check;

	while (1) {
		check = false;
		cout << "Generating Random Number..." << endl;
		r = { rand() % 12 * PLAYER_SIZE + PLAYER_SIZE / 2, rand() % 6 * PLAYER_SIZE + PLAYER_SIZE / 2 };
		for (int i = 0; i <trash_objects.size(); i++) {
			if (r.x == trash_objects[i]->getPos().x && r.y == trash_objects[i]->getPos().y) {
				check = true;
				cout << "Duplicate ERROR" << endl;
				break;
			}
		}
		for (int i = 0; i < player.size(); i++) {
			if (r.x == player[i]->getPos().x && r.y == player[i]->getPos().y) {
				check = true;
				cout << "Duplicate ERROR" << endl;
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
	int hit;
	STATE s;

	// When anyone press button
	if (client.getIsUpdate()) {
		update();
		client.setIsUpdate();
	}

	// When tagger press 'G' button to start game
	if (client.getIsG()) {
		cout << "Game Start" << endl;
		addPlayers();
		update();
		if (player[0]->getState() == STATE_TAGGER) {
			player[0]->setTexture(texture_tagger.getBuf());
			player[1]->setTexture(texture_player[player[1]->getState() - 2].getBuf());
		}
		else {
			player[1]->setTexture(texture_tagger.getBuf());
			player[0]->setTexture(texture_player[player[0]->getState() - 2].getBuf());
		}

		is_start = true;
		client.setIsG();
	}

	// When tagger shoot using 'H' button
	if (client.getIsH()) {
		shoot();
		hit = laserCollision();
		if (hit >= 0)
			cout << "===" << hit << " HIT!===" << endl;
		client.setIsH();
	}
}

// keyboard function
void Game::keyboard(unsigned char key) {
	
	int ran;
	int random_player;
	struct pos p;
	STATE r;
	struct pos my_pos = player[0]->getPos();

	switch (key) {
	case 'w':
		p = { my_pos.x, my_pos.y + PLAYER_SIZE };
		if (!isCollision(p)) {
			client.sendPacket({ p.x, p.y}, 'w');
			polling();
		}
		break;
	case 'a':
		p = { my_pos.x - PLAYER_SIZE, my_pos.y};
		if (!isCollision(p)) {
			client.sendPacket({ p.x, p.y }, 'a');
			polling();
		}
		break;
	case 's':
		p = { my_pos.x, my_pos.y - PLAYER_SIZE };
		if (!isCollision(p)) {
			client.sendPacket({ p.x, p.y }, 's');
			polling();
		}
		break;
	case 'd':
		p = { my_pos.x + PLAYER_SIZE, my_pos.y };
		if (!isCollision(p)) {
			client.sendPacket({ p.x, p.y }, 'd');
			polling();
		}
		break;
	case 'h':
		p = { my_pos.x, my_pos.y };
		client.sendPacket({ p.x, p.y }, 'h');
		break;	
	case 'g':
		p = { 0, 0 };
		client.sendPacket({ p.x, p.y }, 'g');
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
		for (int i = 0; i < player.size(); i++)
			player[i]->drawPlayer();

		if (timer_laser > 0)
			drawLaser();

		for (int i = 0; i < trash_objects.size(); i++) {
			trash_objects[i]->draw();
		}
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
	return false;
}
