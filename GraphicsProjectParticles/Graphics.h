#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "SDL2\SDL.h"
#include "GL\glew.h"
#include <string>

#include "Units.h"
#include "Constants.h"

class Graphics {
public:
	// constructor initializes Window, and Renderer
	Graphics(std::string windowTitle, int glMajorVersion = 4, int glMinorVersion = 0);
	~Graphics();

	bool init();
	void setClearColour(float r, float g, float b, float a);
	void setViewport(units::Pixel x = 0, units::Pixel y = 0, 
		units::Pixel w = constants::DEFAULT_SCREEN_WIDTH, units::Pixel h = constants::DEFAULT_SCREEN_HEIGHT);
	void setFullscreen();
	void toggleFullscreen();
	void getWindowSize( int &w, int &h );
	void clear();
	void present();

private:
	SDL_Window* window_;
	SDL_GLContext glContext_;
	const std::string window_title_;
	const int gl_major_ver_;
	const int gl_minor_ver_;
	bool is_fullscreen_;
};


#endif // _GRAPHICS_H