#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "game.h"
using namespace std;
#define GLOBALTIMER 16

Game *game;

void init(int argc, char **argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(GAME_WIDTH, GAME_HEIGHT);
	glutInitWindowPosition(300, 300);

	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("CSED353 Computer Network Project");

	glewExperimental = GL_TRUE;
	glewInit();
	
	glShadeModel(GL_FLAT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDepthFunc(GL_LEQUAL);

	game = new Game();
	game->shaderInit();
	game->addNewPlayer();
	game->addFlashlight();
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);
	game->draw();
	glutSwapBuffers();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {	
	game->keyboard(key);
}

void timer(int value) {
	glutTimerFunc(GLOBALTIMER, timer, value);
	glutPostRedisplay();
}

void main(int argc, char **argv) {
	init(argc, argv);
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	timer(0);
	glutMainLoop();
}
