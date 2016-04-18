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
	const Pixel			SCREEN_WIDTH = 1280;
	const Pixel			SCREEN_HEIGHT = 720;
	// Camera projection.
	const float			FOV = 50.0f;
	const float			ASPECT = float(SCREEN_WIDTH)/float(SCREEN_HEIGHT);
	const float			NEAR = 0.1f;
	const float			FAR = 100.0f;

	const MS			SIXTY_FPS_FRAME_DUR = 17;
	const MS			MAX_FRAME_DUR = SIXTY_FPS_FRAME_DUR * 2;
	
	const unsigned int	DEFAULT_GRAV_FORCE = 10;
	const unsigned int	MIN_GRAV_FORCE = 1;
	const unsigned int	MAX_GRAV_FORCE = 1000;
	const unsigned int	GRAV_FORCE_INCR = 1;

	const unsigned int	DEFAULT_POINT_SIZE = 1;
	const unsigned int	MIN_POINT_SIZE = 1;
	const unsigned int	MAX_POINT_SIZE = 20;
	const unsigned int	POINT_SIZE_IRCR = 1;

	const unsigned int	DEFAULT_PARTICLE_EXPONENT = 10; // 2^10 = 1024 for over 1m particles.

	const unsigned int MIN_PARTICLE_EXPONENT = 2;
	/*
	A note of the maximum particle exponent value:

	2^12 in both dimensions is ~17 million particles. This requires a somewhat strong GPU.
	Keep in mind that there are four textures (so if you have dimensions 2^10 x 2^10, you are creating 4x 1024x1024 textures).
	These are RGBA 32 bit floating point textures, so if you use 2^12 in both dimensions (4k textures) that requires a full 1GB
	of VRAM.

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