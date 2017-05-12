#include "flash.h"

Flash::Flash() {
	angle = -90;
	animate_angle = -90;
}

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

void Flash::setDir(int _dir) {
	dir = _dir;
}

void Flash::setAngle(int _angle) {
	angle = _angle;
}

void Flash::animateRotate() {
	//cout << animate_angle << "  " << angle << endl;
	animate_angle = animate_angle * 0.7 + angle * 0.3;
}

