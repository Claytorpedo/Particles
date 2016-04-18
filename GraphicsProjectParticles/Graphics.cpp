#include "Graphics.h"

#include "SDL2\SDL.h"
#include "GL\glew.h"
#include "SDL2\SDL_opengl.h"
#include <iostream>

#include "Units.h"
#include "Constants.h"

using namespace constants;

Graphics::Graphics(std::string windowTitle, int glMajorVersion, int glMinorVersion) :
	window_title_(windowTitle), gl_major_ver_(glMajorVersion), gl_minor_ver_(glMinorVersion) { }
Graphics::~Graphics() {
	// free renderer and window
	SDL_DestroyWindow(window_);
}
bool Graphics::init() {
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "Error: SDL video could not be initialized. SLD Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Set the OpenGL version.
	
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver_) < 0 ||
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver_) < 0 ) {
			std::cerr << "Error: Could not set OpenGl to version " << gl_major_ver_ << "." << gl_minor_ver_ << "." << std::endl;
			return false;
	}
	// Disable depreciated functionality.
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Create the window.
	window_ = SDL_CreateWindow(window_title_.c_str(), SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	if ( NULL == window_ ) {
		std::cerr << "Error: the window could not be created. SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Create the GL context.
	glContext_ = SDL_GL_CreateContext(window_);
	if ( NULL == glContext_ ) {
		std::cerr << "Error: the OpenGL context could not be created. SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Initialize GLEW.
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if ( error != GLEW_OK ) {
		std::cerr << "Error: GLEW could not be initialized. GLEW Error: " << glewGetErrorString(error) << std::endl;
		return false;
	}
	// Enable Vsync.
	if ( SDL_GL_SetSwapInterval(1) < 0 ) {
		std::cerr << "Warning: VSync could not be enabled. SDL Error: " << SDL_GetError() << std::endl;
	}
	is_fullscreen_ = false;
	// Initialize clear colour to black.
	glClearColor(0,0,0,1);
	glEnable( GL_BLEND );
	clear();
	present();
	return true;
}
void Graphics::setClearColour(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}
void Graphics::clear() {
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
void Graphics::setViewport(units::Pixel x, units::Pixel y, units::Pixel w, units::Pixel h) {
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	glViewport(x, y, w, h);
}
void Graphics::setFullscreen() {
	SDL_SetWindowFullscreen( window_, SDL_WINDOW_FULLSCREEN_DESKTOP );
	is_fullscreen_ = true;
}
void Graphics::toggleFullscreen() {
	if ( !is_fullscreen_ ) {
		SDL_SetWindowFullscreen( window_, SDL_WINDOW_FULLSCREEN_DESKTOP );
	} else {
		SDL_SetWindowFullscreen( window_, SDL_FALSE );
	}
	is_fullscreen_ = !is_fullscreen_;
}
void Graphics::getWindowSize( int &w, int &h ) {
	SDL_DisplayMode dm;
	if ( SDL_GetDesktopDisplayMode( 0, &dm ) != 0 ) {
		std::cerr << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError() << std::endl;
		return;
	}
	w = dm.w;
	h = dm.h;
}
void Graphics::present() {
	SDL_GL_SwapWindow( window_ );
}