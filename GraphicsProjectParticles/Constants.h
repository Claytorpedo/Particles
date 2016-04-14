#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Units.h"
#include <string>
#include <vector>

using namespace units;

namespace constants {

	const int ESCAPE_CHAR = 27; // Keyboard escape key.
	const std::string WINDOW_TITLE = "Alexander Dyck; Graphics 2 Project; Particle Simulator";
	const int GL_MAJOR_VER = 4;
	const int GL_MINOR_VER = 0;

	const MS			SECOND_DURATION = 1000;			// In miliseconds.
	const MS			MAX_FRAME_TIME = 50;			// The maximum lengh one frame can last in MS.
	const Coordinate	EPSILON = 0.00001f;				// A very small value, used to check if floats are "close enough" to zero.
	const Pixel			SCREEN_WIDTH = 1920;
	const Pixel			SCREEN_HEIGHT = 1080;
	// Camera projection.
	const float			FOV = 50.0f;
	const float			ASPECT = float(SCREEN_WIDTH)/float(SCREEN_HEIGHT);
	const float			NEAR = 0.1f;
	const float			FAR = 100.0f;

	const MS			SIXTY_FPS_FRAME_DUR = 17;

	const unsigned int	DEFAULT_PARTICLE_EXPONENT = 10; // 2^10 = 1024

	// Shader file paths from the main directory.
	const char* const BASIC_SHADER_VERT_PATH = "shaders/basicShader.vert"; 
	const char* const BASIC_SHADER_FRAG_PATH = "shaders/basicShader.frag";
	const char* const TEXTURE_SHADER_VERT_PATH = "shaders/drawTexture.vert"; 
	const char* const TEXTURE_SHADER_FRAG_PATH ="shaders/drawTexture.frag";
}

#endif // _CONSTANTS_H_