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
	const Pixel			SCREEN_WIDTH = 1280;
	const Pixel			SCREEN_HEIGHT = 720;
	// Camera projection.
	const float			FOV = 50.0f;
	const float			ASPECT = float(SCREEN_WIDTH)/float(SCREEN_HEIGHT);
	const float			NEAR = 0.1f;
	const float			FAR = 100.0f;

	const unsigned int	DEFAULT_PARTICLE_EXPONENT = 10; // 2^10 = 1024

	// Shader file paths from the main directory.
	const char* const BASIC_SHADER_VERT_PATH = "shaders/basicShader.vert"; 
	const char* const BASIC_SHADER_FRAG_PATH ="shaders/basicShader.frag";
	const char* const AMB_PART_INIT_VERT_PATH = "shaders/AmbientParticleInit.vert";
	const char* const AMB_PART_INIT_FRAG_PATH = "shaders/AmbientParticleInit.frag";
	const char* const AMB_PART_UPDATE_VERT_PATH = "shaders/AmbientParticleUpdate.vert";
	const char* const AMB_PART_UPDATE_FRAG_PATH = "shaders/AmbientParticleUpdate.frag";
	const char* const AMB_PART_DRAW_VERT_PATH = "shaders/AmbientParticleDraw.vert";
	const char* const AMB_PART_DRAW_FRAG_PATH = "shaders/AmbientParticleDraw.frag";
	const char* const TEXTURE_SHADER_VERT_PATH = "shaders/drawTexture.vert"; 
	const char* const TEXTURE_SHADER_FRAG_PATH ="shaders/drawTexture.frag";
	
	// Everything in mirrored string vectors with enums for easy access.
	// Nested namespaces are so the char arrays don't get scoped instead of the vectors.
	namespace init {
		namespace h {
			const char* const init_vars_char[] = { "inVertexPos" };
			const char* const init_unis_char[] = { "uResolution" };
		}
		enum Variables { IN_VERTEX_POS = 0, TOTAL_VARS };
		enum Uniforms { U_RESOLUTION = 0, TOTAL_UNIFORMS };
		const std::vector<std::string> VARIABLE_NAMES(h::init_vars_char, std::end(h::init_vars_char));
		const std::vector<std::string> UNIFORM_NAMES(h::init_unis_char, std::end(h::init_unis_char));
	}
	namespace update {
		namespace h {
			const char* const update_vars_char[] = { "inVertexPos" };
			const char* const update_unis_char[] = { "uResolution", "uDeltaT", "uInputPos", "uKForce", "uTexPos", "uTexVel", "uTexOther" };
		}
		enum Variables { IN_VERTEX_POS = 0, TOTAL_VARS };
		enum Uniforms { U_RESOLUTION = 0, U_DELTA_T, U_INPUT_POS, U_K_FORCE, U_TEX_0, U_TEX_1, U_TEX_2, TOTAL_UNIFORMS };
		const std::vector<std::string> VARIABLE_NAMES(h::update_vars_char, std::end(h::update_vars_char));
		const std::vector<std::string> UNIFORM_NAMES(h::update_unis_char, std::end(h::update_unis_char));
	}
	namespace draw {
		namespace h {
			const char* const draw_vars_char[] = { "inUV" };
			const char* const draw_unis_char[] = { "uPointSize", "uPVM", "uColour", "uTexPos", "uTexVel", "uTexOther" };
		}
		enum Variables { IN_UV = 0, TOTAL_VARS };
		enum Uniforms { U_POINT_SIZE = 0, U_PVM, U_COLOUR, U_TEX_0, U_TEX_1, U_TEX_2, TOTAL_UNIFORMS };
		const std::vector<std::string> VARIABLE_NAMES(h::draw_vars_char, std::end(h::draw_vars_char));
		const std::vector<std::string> UNIFORM_NAMES(h::draw_unis_char, std::end(h::draw_unis_char));
	}

	const unsigned int NUM_TEXTURES_PER_FRAMEBUFFER = 3;
}

#endif // _CONSTANTS_H_