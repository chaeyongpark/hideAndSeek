#include "obj.h"

// constructor
Obj::Obj() {
	cout << "Obj Constructor" << endl;
}

// set buffer
void Obj::setBuffer() {
	GLuint ver_size = num_vertex * sizeof(vec3);
	GLuint color_size = num_vertex * sizeof(vec4);
	GLuint texture_size = num_vertex * sizeof(vec2);

	glGenBuffers(1, &buf_address);
	glBindBuffer(GL_ARRAY_BUFFER, buf_address);
	glBufferData(GL_ARRAY_BUFFER, ver_size + color_size + texture_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ver_size, vec_ver.front());
	glBufferSubData(GL_ARRAY_BUFFER, ver_size, color_size, vec_col.front());
	glBufferSubData(GL_ARRAY_BUFFER, ver_size + color_size, texture_size, vec_texture.front());
}

void Obj::setShaderBuffer(struct location _loc) {
	loc = _loc;
}

struct location Obj::getLocation() {
	return loc;
}

struct pos Obj::getPos() {
	return position;
}

void Obj::setPos(int _x, int _y) {
	position.x = _x;
	position.y = _y;
}

void Obj::setPos(pos _pos) {
	position = _pos;
}

// set position for animation moving 
void Obj::setAnimatePosition(pos _pos) {
	animate_position = _pos;
}

// set position, width and height
void Obj::setAll(pos _pos, int _width, int _height) {
	position = _pos;
	width = _width;
	height = _height;
}

void Obj::setTexture(GLuint _buf) {
	texture_buf = _buf;
}

// move objects with animation
void Obj::animateMove() {
	animate_position.x = (animate_position.x * ANIMATION_SPEED + position.x * (10 - ANIMATION_SPEED)) / 10;
	animate_position.y = (animate_position.y * ANIMATION_SPEED + position.y * (10 - ANIMATION_SPEED)) / 10;
}

// draw
void Obj::draw() {
	vec3 moveVec(animate_position.x, animate_position.y, 0.0);
	mat4 transform = Translate(moveVec);
	GLuint ver_size = num_vertex * sizeof(vec3);
	GLuint col_size = num_vertex * sizeof(vec4);

	// texture buffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_buf);

	glBindBuffer(GL_ARRAY_BUFFER, buf_address);
	glVertexAttribPointer(loc.vertex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(loc.color, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(ver_size));
	glVertexAttribPointer(loc.texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(ver_size + col_size));
	glUniformMatrix4fv(loc.model, 1, GL_TRUE, transform);
	glDrawArrays(GL_TRIANGLES, 0, num_vertex);
}

// make rectangle [x1, y1] -> [x2, y2]
void Obj::makeRect(float x1, float y1, float x2, float y2, float z) {
	vec_ver.push_back(vec3(x1, y1, z));
	vec_ver.push_back(vec3(x1, y2, z));
	vec_ver.push_back(vec3(x2, y2, z));
	vec_ver.push_back(vec3(x2, y2, z));
	vec_ver.push_back(vec3(x2, y1, z));
	vec_ver.push_back(vec3(x1, y1, z));

	for (int i = 0; i < 6; i++)
		//vec_col.push_back(vec4(0.0f, 0.0f, 0.0f, 0.0f));
		vec_col.push_back(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	vec_texture.push_back(vec2(0.0, 0.0));
	vec_texture.push_back(vec2(0.0, 1.0));
	vec_texture.push_back(vec2(1.0, 1.0));
	vec_texture.push_back(vec2(1.0, 1.0));
	vec_texture.push_back(vec2(1.0, 0.0));
	vec_texture.push_back(vec2(0.0, 0.0));
}

// make rectangle: center point is [x1, y1] with size
void Obj::makeRect(float x1, float y1, float size, float z) {
	vec_ver.push_back(vec3(x1-size/2, y1-size/2, z));
	vec_ver.push_back(vec3(x1-size/2, y1+size/2, z));
	vec_ver.push_back(vec3(x1+size/2, y1+size/2, z));
	vec_ver.push_back(vec3(x1+size/2, y1+size/2, z));
	vec_ver.push_back(vec3(x1+size/2, y1-size/2, z));
	vec_ver.push_back(vec3(x1-size/2, y1-size/2, z));

	for (int i = 0; i < 6; i++)
		vec_col.push_back(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	vec_texture.push_back(vec2(0.0f, 1.0f));
	vec_texture.push_back(vec2(0.0f, 0.0f));
	vec_texture.push_back(vec2(1.0f, 0.0f));
	vec_texture.push_back(vec2(1.0f, 0.0f));
	vec_texture.push_back(vec2(1.0f, 1.0f));
	vec_texture.push_back(vec2(0.0f, 1.0f));
}
