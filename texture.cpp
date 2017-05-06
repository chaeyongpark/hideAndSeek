#include "texture.h"
#include "lodepng.h"
#include <vector>
#include <iostream>
using namespace std;

bool Texture::load(char * fn) {
	vector<unsigned char> png;
	vector<unsigned char> image;
	unsigned error;

	error = lodepng::load_file(png, fn);
	if (error) {
		cout << "Error " << error << " loading image " << fn << endl;
		cout << endl;
		return false;
	}

	error = lodepng::decode(image, w, h, png);
	if (error) {
		cout << "Error " << error << " loading image " << fn << endl;
		cout << endl;
		return false;
	}

	glGenTextures(1, &buf);
	glBindTexture(GL_TEXTURE_2D, buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	cout << "Texture: " << fn << " is successfully loaded" << endl;
	return true;
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, buf);
}

GLuint Texture::getBuf() {
	return buf;
}
