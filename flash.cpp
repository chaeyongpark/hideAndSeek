#include "flash.h"

// Flash constructor
Flash::Flash() {
	angle = -90;
	animate_angle = -90;
}

// Draw flash
void Flash::draw() {
	vec3 moveVec(animate_position.x, animate_position.y, 0.0);
	mat4 transform = Translate(moveVec);

	animateRotate();
	transform = transform * RotateZ(animate_angle);

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

// Set flash direction along its player
void Flash::setDir(int _dir) {
	dir = _dir;
}

// Set angle of flash
void Flash::setAngle(int _angle) {
	angle = _angle;
}

// Using anle of flahs, this function can draw smooth animation 
void Flash::animateRotate() {
	animate_angle = animate_angle * 0.7 + angle * 0.3;
}

