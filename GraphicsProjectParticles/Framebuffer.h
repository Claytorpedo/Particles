#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <vector>
#include "GL/glew.h"

static const GLuint _MAX_COLOUR_ATTACH = 16;

class Framebuffer {
public:
	const unsigned int width, height, numTextures;

	Framebuffer(unsigned int width, unsigned int height, unsigned int numTextures)
		: width(width), height(height), numTextures(numTextures), fbo_(0), is_initialized_(false) {}
	~Framebuffer();

	bool init();
	bool isInitialized();

	GLuint getBufferID() { return fbo_; }
	std::vector<GLuint> getTextures() { return textures_; }
	GLuint getTexture(unsigned int num);
	void bindTextures(unsigned int num, GLint *uniformTextureLocations);
	void unbindTextures();
	void drawTo();
	void stopDrawingTo();
	void readFrom();
	void stopReading();
	void printInfo();

private:
	GLuint fbo_;
	std::vector<GLuint> textures_;
	bool is_initialized_;

	void genTexture(GLuint &tex);
};


#endif // _FRAMEBUFFER_H