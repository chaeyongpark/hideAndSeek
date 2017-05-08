/*
 * POSTECH CSED353 Computer Network Project
 * Hide and Seek
 * chaeyong Park, junGyeong Choi and sohyeon Lee
 * 2017.05
 */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <ctime>
#include "game.h"

using namespace std;
#define GLOBALTIMER 16

// game using global variable pointer
Game *game;

// OpenGL init
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
	srand(time(NULL));

	game = new Game();
}

// Draw everr CLOBALTIMER
void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);
	game->draw();
	glutSwapBuffers();
	glFlush();
}

// Keyboard input
void keyboard(unsigned char key, int x, int y) {	
	game->keyboard(key);
}

// Call display 
void timer(int value) {
	glutTimerFunc(GLOBALTIMER, timer, value);
	game->tick();
	glutPostRedisplay();
}

void main(int argc, char **argv) {
	init(argc, argv);
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	timer(0);
	glutMainLoop();
}
