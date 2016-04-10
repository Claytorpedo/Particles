#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <vector>
#include "GL\glew.h"

static const GLuint _MAX_COLOUR_ATTACH = 32;

class Framebuffer {

private:
	GLuint buffer_id_;
	std::vector<GLuint> textures_;

public:
	const unsigned int width, height, numTextures;

	Framebuffer(unsigned int width, unsigned int height, unsigned int numTextures)
		: width(width), height(height), numTextures(numTextures), buffer_id_(0) {}
	~Framebuffer();

	bool init();

	GLuint getBufferID() { return buffer_id_; }
	std::vector<GLuint> getTextures() { return textures_; }
	GLuint getTexture(unsigned int num);
};


#endif // _FRAMEBUFFER_H