#include "game.h"

Game::Game() {
	projection = Ortho(0.0f, GAME_WIDTH, 0.0f, GAME_HEIGHT, -2, 2);

	/* Debug */
	cout << "Game Constructor" << endl;
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

void Game::addNewPlayer() {
	Player *new_player = new Player();
	new_player->setShaderBuffer(loc);
	new_player->setBuffer();
	new_player->setTexture("img/acorn.png");

	player.push_back(new_player);
}

void Game::addFlashlight() {
	struct pos init_pos = player[0]->getInitPos();

	flashlight.setAnimatePosition(init_pos);
	flashlight.setAll(init_pos, 1920, 1920);
	flashlight.makeRect(0, 0, 1920, 1.0);
	flashlight.setShaderBuffer(loc);
	flashlight.setBuffer();
	flashlight.setTexture("img/flashLight.png");
}

void Game::keyboard(unsigned char key) {
	int x = player[0]->getPosition().x;
	int y = player[0]->getPosition().y;

	switch (key) {
	case 'w':
		player[0]->move(0, PLAYER_SIZE);
		flashlight.setPos(x, y + PLAYER_SIZE);
		break;
	case 'a':
		player[0]->move(-PLAYER_SIZE, 0);
		flashlight.setPos(x - PLAYER_SIZE, y);
		break;
	case 's':
		player[0]->move(0, -PLAYER_SIZE);
		flashlight.setPos(x, y - PLAYER_SIZE);
		break;
	case 'd':
		player[0]->move(PLAYER_SIZE, 0);
		flashlight.setPos(x + PLAYER_SIZE, y);
		break;
	default: cout << "OTHER KEY " << key << endl;
	}
}

Player * Game::getPlayer(int n) {
	return player[0];
}

void Game::draw() {
	glUniformMatrix4fv(loc.projection, 1, GL_TRUE, projection);
	player.front()->drawPlayer();

	//flashlight.draw();
	//flashlight.animateMove();
}
