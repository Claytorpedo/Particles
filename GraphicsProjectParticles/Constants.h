#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Units.h"
#include <string>
#include <vector>

#include "glm\glm.hpp"

using namespace units;

namespace constants {

	const int ESCAPE_CHAR = 27; // Keyboard escape key.
	const std::string WINDOW_TITLE = "Alexander Dyck; Graphics 2 Project; Particle Simulator";
	const int GL_MAJOR_VER = 4;
	const int GL_MINOR_VER = 0;

	const MS			SECOND_DURATION = 1000;			// In miliseconds.
	const MS			MAX_FRAME_TIME = 50;			// The maximum lengh one frame can last in MS.
	const Coordinate	EPSILON = 0.00001f;				// A very small value, used to check if floats are "close enough" to zero.
	const Pixel			DEFAULT_SCREEN_WIDTH = 1280;
	const Pixel			DEFAULT_SCREEN_HEIGHT = 720;
	// Camera projection.
	const float			FOV = 50.0f;
	const float			DEFAULT_ASPECT = float(DEFAULT_SCREEN_WIDTH)/float(DEFAULT_SCREEN_HEIGHT);
	const float			NEAR = 0.1f;
	const float			FAR = 100.0f;

	const MS			SIXTY_FPS_FRAME_DUR = 17;
	const MS			MAX_FRAME_DUR = SIXTY_FPS_FRAME_DUR * 2;
	
	const float	DEFAULT_GRAV_FORCE = 10;
	const float	MIN_GRAV_FORCE = -10;
	const float	MAX_GRAV_FORCE = 50;
	const float	GRAV_FORCE_SMALL_INCR = 0.1f;
	const float GRAV_FORCE_LARGE_INCR = 1.0f;

	const int	DEFAULT_POINT_SIZE = 1;
	const int	MIN_POINT_SIZE = 1;
	const int	MAX_POINT_SIZE = 20;
	const int	POINT_SIZE_IRCR = 1;

	const int	DEFAULT_COHESIVENESS = 500;
	const int	MIN_COHESIVENESS = 1;
	const int	MAX_COHESIVENESS = 2500;
	const int	COHESIVENESS_SMALL_INCR = 10;
	const int	COHESIVENESS_LARGE_INCR = 100;

	const float	DEFAULT_ZOOM = -5.0f;
	const float MIN_ZOOM = -40.0f;
	const float MAX_ZOOM = -0.5f;
	const float ZOOM_SMALL_INCR = 0.1f;
	const float ZOOM_LARGE_INCR = 1.0f;
	const float ZOOM_MOUSE_INCR = 0.25f;
	

	const float DEFAULT_GRAV_DEPTH = 5.0f;

	// Number of rotational degrees moving the mouse from one side of the screen to the other translates to.
	const float SCREEN_DEGREES_WIDE = 540.0f;
	const float SCREEN_DEGREES_TALL = 540.0f;

	const float PIXELS_TO_PAN = 0.01f;
	const float SCROLL_TO_ZOOM = 0.1f;

	const unsigned int	DEFAULT_PARTICLE_EXPONENT = 10; // 2^10 = 1024 for over 1m particles.

	const unsigned int MIN_PARTICLE_EXPONENT = 2;
	/*
	A note of the maximum particle exponent value:

	2^12 in both dimensions is ~17 million particles. This requires a somewhat strong GPU.
	Keep in mind that there are four textures (so if you have dimensions 2^10 x 2^10, you are creating 4x 1024x1024 textures).
	These are RGB 32 bit floating point textures, so if you use 2^12 in both dimensions (4k textures) that requires ~768MB of VRAM.

	There will also be an exponentially longer initialization time as the CPU calculates UV coordinates for all of the particles.
	*/
	const unsigned int MAX_PARTICLE_EXPONENT = 12; 

	const glm::vec4		GOLD( 0.9f, 0.3f, 0.1f, 0.6f );

	// Shader file paths from the main directory.
	const char* const BASIC_SHADER_VERT_PATH = "shaders/testShader.vert"; 
	const char* const BASIC_SHADER_FRAG_PATH = "shaders/testShader.frag";
	const char* const TEXTURE_SHADER_VERT_PATH = "shaders/drawTexture.vert"; 
	const char* const TEXTURE_SHADER_FRAG_PATH ="shaders/drawTexture.frag";
}

#endif // _CONSTANTS_H_