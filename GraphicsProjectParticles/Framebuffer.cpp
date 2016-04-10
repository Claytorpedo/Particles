#include "Framebuffer.h"

#include "GL\glew.h"
#include "SDL2\SDL_opengl.h"

#include <vector>
#include <iostream>
#include <cassert>

Framebuffer::~Framebuffer() {
	if (!textures_.empty()) glDeleteTextures( textures_.size(), textures_.data() );
	if (buffer_id_) glDeleteFramebuffers( 1, &buffer_id_ );
}

bool Framebuffer::init() {
	assert( numTextures <= _MAX_COLOUR_ATTACH );
	// Get the buffer id.
	glGenFramebuffers( 1, &buffer_id_ );
	glBindFramebuffer( GL_FRAMEBUFFER, buffer_id_ );
	// Generate the textures.
	for (unsigned int i = 0; i < numTextures; ++i) {
		GLuint tex;
		glGenTextures( 1, &tex );
		glBindTexture(GL_TEXTURE_2D, tex);
		// Give empty image to OpenGL.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		// Use GL_NEAREST, since we don't want any kind of averaging across values:
		// we just want one pixel to represent a particle's data.
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// This probably isn't necessary, but we don't want to have UV coords past the image edges.
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		textures_.push_back(tex);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Bind our textures to the framebuffer.
	GLenum drawBuffers[ _MAX_COLOUR_ATTACH ];
	for (unsigned int i = 0; i < numTextures; ++i) {
		GLenum attach = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture(GL_FRAMEBUFFER, attach, textures_[i], 0);
		drawBuffers[i] = attach;
	}
	glDrawBuffers( numTextures, drawBuffers );

	if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		std::cerr << "Error: Failed to create framebuffer." << std::endl;
		return false;
	}
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	return true;
}

GLuint Framebuffer::getTexture(unsigned int num) { 
	if ( num >= textures_.size() ) {
		std::cerr << "Error: Invalid texture number (requested: " << num;
		std::cerr << " total textures: " << textures_.size() << ")." << std::endl;
		return 0;
	}
	return textures_[num]; 
}