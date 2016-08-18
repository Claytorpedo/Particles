#include "Framebuffer.h"

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include <vector>
#include <iostream>
#include <cassert>

Framebuffer::~Framebuffer() {
	if ( !textures_.empty() )	{ glDeleteTextures( textures_.size(), textures_.data() ); }
	if ( fbo_ )					{ glDeleteFramebuffers( 1, &fbo_ ); }
}

bool Framebuffer::init() {
	assert( numTextures <= _MAX_COLOUR_ATTACH );
	// Create the FBO.
	glGenFramebuffers( 1, &fbo_ );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo_ );

	std::vector<GLenum> drawBuffers( numTextures );
	// Generate the textures.
	for (unsigned int i = 0; i < numTextures; ++i) {
		GLuint tex;
		genTexture( tex );
		textures_.push_back( tex );
		// Bind texture to the framebuffer.
		GLenum attach = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, attach, tex, 0);
		drawBuffers[i] = attach;
	}
	glDrawBuffers( numTextures, drawBuffers.data() );

	if ( glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		std::cerr << "Error: Failed to create framebuffer." << std::endl;
		return false;
	}
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	is_initialized_ = true;
	return true;
}
bool Framebuffer::isInitialized() {
	return is_initialized_;
}
GLuint Framebuffer::getTexture(unsigned int num) { 
	if ( !is_initialized_ ) {
		std::cerr << "Warning: Framebuffer is not initialized, call to getTexture invalid." << std::endl;
		return 0;
	}
	if ( num >= textures_.size() ) {
		std::cerr << "Error: Invalid texture number (requested: " << num;
		std::cerr << " total textures: " << textures_.size() << ")." << std::endl;
		return 0;
	}
	return textures_[num]; 
}
void Framebuffer::bindTextures(unsigned int num, GLint *uniformTextureLocations) {
	if ( !is_initialized_ ) {
		std::cerr << "Warning: Framebuffer is not initialized, call to bindTextures invalid." << std::endl;
		return;
	}
	for ( unsigned int i = 0; i < numTextures && i < num; ++i ) {
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( GL_TEXTURE_2D, textures_[i] );
		glUniform1i( uniformTextureLocations[i], i );
	}
}
void Framebuffer::unbindTextures() {
	glBindTexture( GL_TEXTURE_2D, 0);
}
void Framebuffer::drawTo() {
	if ( !is_initialized_ ) {
		std::cerr << "Warning: Framebuffer is not initialized. Drawing is invalid." << std::endl;
	}
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo_);
}
void Framebuffer::stopDrawingTo() {
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0);
}
void Framebuffer::readFrom() {
	if ( !is_initialized_ ) {
		std::cerr << "Warning: Framebuffer is not initialized. Reading is invalid." << std::endl;
	}
	glBindFramebuffer( GL_READ_FRAMEBUFFER, fbo_);
}
void Framebuffer::stopReading() {
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0);
}
void Framebuffer::genTexture(GLuint &tex) {
	glGenTextures( 1, &tex );
	glBindTexture(GL_TEXTURE_2D, tex);
	// Give empty image to OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	// Use GL_NEAREST, since we don't want any kind of averaging across values:
	// We just want one pixel to represent a particle's data.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// This probably isn't necessary, but we don't want to have UV coords past the image edges.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Framebuffer::printInfo() {
	glBindFramebuffer( GL_FRAMEBUFFER, fbo_ );

	int res;
	GLint buffer;
	int i = 0;
	do {
		glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);
		if (buffer != GL_NONE) {
			printf("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0);

			glGetFramebufferAttachmentParameteriv(
				GL_FRAMEBUFFER, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
			printf("\tAttachment Type: %s\n", res==GL_TEXTURE?"Texture":"Render Buffer");
			glGetFramebufferAttachmentParameteriv(
				GL_FRAMEBUFFER, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
			printf("\tAttachment object name: %d\n",res);
		}
		++i;
	} while (buffer != GL_NONE);
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}