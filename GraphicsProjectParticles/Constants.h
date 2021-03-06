#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Units.h"
#include <string>
#include <vector>

#include "glm/glm.hpp"

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
	
	const int	DEFAULT_POINT_SIZE = 1;
	const int	MIN_POINT_SIZE = 1;
	const int	MAX_POINT_SIZE = 20;
	const int	POINT_SIZE_IRCR = 1;

	const int	DEFAULT_COHESIVENESS = 500;
	const int	MIN_COHESIVENESS = 10;
	const int	MAX_COHESIVENESS = 2500;
	const int	COHESIVENESS_SMALL_INCR = 10;
	const int	COHESIVENESS_LARGE_INCR = 100;

	const float	DEFAULT_ZOOM = -5.0f;
	const float MIN_ZOOM = -50.0f;
	const float MAX_ZOOM = -0.5f;
	const float ZOOM_SMALL_INCR = 0.1f;
	const float ZOOM_LARGE_INCR = 1.0f;
	const float ZOOM_MOUSE_INCR = 0.25f;
	
	const glm::vec3 DEFAULT_GRAV_POS(-3.0f, -3.0f, 0.0f );

	const float	DEFAULT_GRAV_FORCE = 10;
	const float	MIN_GRAV_FORCE = -10;
	const float	MAX_GRAV_FORCE = 50;
	const float	GRAV_FORCE_SMALL_INCR = 0.1f;
	const float GRAV_FORCE_LARGE_INCR = 1.0f;

	const float DEFAULT_GRAV_DIST = 5.0f;
	const float MIN_GRAV_DIST = 1.0f;
	const float MAX_GRAV_DIST = 50.0f;
	const float GRAV_DIST_SMALL_INCR = 0.1f;
	const float GRAV_DIST_LARGE_INCR = 1.0f;
	const float GRAV_DIST_MOUSE_INCR = 0.25f;

	const int MAX_GRAV_OBJECTS = 10;

	// Number of rotational degrees moving the mouse from one side of the screen to the other translates to.
	const float SCREEN_DEGREES_WIDE = 540.0f;
	const float SCREEN_DEGREES_TALL = 540.0f;

	const float PIXELS_TO_PAN = 0.01f;
	const float SCROLL_TO_ZOOM = 0.1f;

	const unsigned int	DEFAULT_PARTICLE_DIMENSIONS = units::getPowerOf2(10); // 2^10 = 1024 for over 1m particles.
	/*
	A note of the particle maximum:

	2^24 is ~17 million particles. This requires a somewhat strong GPU.
	Keep in mind that there are four RGB 32 bit floating point textures, so if you have 2^24 particles (4k textures) that requires 768MB of VRAM.

	There will also be an long initialization times as the CPU calculates the UV coordinates for all of the particles.
	*/
	const unsigned int MAX_PARTICLES = units::getPowerOf2(24);
	const int PARTICLE_INCR = 32;

	const float DEFAULT_ALPHA = 0.6f;
	const float MIN_ALPHA = 0.01f;
	const float MAX_ALPHA = 1.0f;
	const float ALPHA_SMALL_INCR = 0.01f;
	const float ALPHA_LARGE_INCR = 0.1f;

	const glm::vec3 ORANGE( 0.95f, 0.27f, 0.06f );
	const glm::vec3	CYAN( 0.1f, 0.8f, 0.8f );
	const glm::vec3	VIOLET( 0.8f, 0.2f, 0.8f );
	const glm::vec3 INDIGO( 0.3f, 0.05f, 0.5f );
	const glm::vec3 OLIVE( 0.6f, 0.9f, 0.15f );
	const glm::vec3 YELLOW( 0.85f, 0.85f, 0.08f );
	const glm::vec3 BLUE( 0.1f, 0.1f, 0.9f );
	const glm::vec3 RED( 0.9f, 0.1f, 0.1f );
	const glm::vec3 GREEN( 0.1f, 0.9f, 0.1f);
	const glm::vec3 WHITE( 0.5f, 0.5f, 0.5f );

	// Shader file paths from the main directory.
	const char* const BASIC_SHADER_VERT_PATH = "shaders/testShader.vert"; 
	const char* const BASIC_SHADER_FRAG_PATH = "shaders/testShader.frag";
	const char* const TEXTURE_SHADER_VERT_PATH = "shaders/drawTexture.vert"; 
	const char* const TEXTURE_SHADER_FRAG_PATH ="shaders/drawTexture.frag";
}

#endif // _CONSTANTS_H_