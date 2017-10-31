#ifndef _AMBIENT_PARTICLE_SYSTEM_H
#define _AMBIENT_PARTICLE_SYSTEM_H

#include <vector>
#include <string>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "Units.h"
#include "Constants.h"

class VertexBuffer;
class ShaderProgram;
class Framebuffer;

class AmbientParticleSystem {
public:
	AmbientParticleSystem(unsigned int dimensions, glm::vec4 pointColour);
	AmbientParticleSystem(unsigned int width, unsigned int height, glm::vec4 pointColour);
	~AmbientParticleSystem();

	bool init(const units::Pixel viewportWidth, const units::Pixel viewportHeight);
	bool resize(unsigned int width, unsigned int height, const units::Pixel viewportWidth, const units::Pixel viewportHeight);
	void update(const units::MS elapsedTime, const units::Pixel viewportWidth, const units::Pixel viewportHeight, const glm::vec4 gravityObjs[constants::MAX_GRAV_OBJECTS],
			const unsigned int activeGravObjs[constants::MAX_GRAV_OBJECTS], const unsigned int cohesiveness = constants::DEFAULT_COHESIVENESS);
	void draw( const glm::mat4 &PVM, const unsigned int pointSize = 1 );
	void togglePause() { is_paused_ = !is_paused_; }
	void setParticleColour(glm::vec4 colour);
	void setParticleColourNoAlpha(glm::vec3 colour);
	void setParticleAlpha(float alpha);
	unsigned int getWidth() { return particle_texture_width_; }
	unsigned int getHeight() { return particle_texture_height_; }
	unsigned int getNumParticles() { return particle_texture_width_ * particle_texture_height_; }

private:
	bool is_paused_;
	unsigned int particle_texture_width_, particle_texture_height_;
	VertexBuffer *uv_buffer_, *quad_buffer_;
	ShaderProgram *init_shader_, *update_shader_, *draw_shader_;
	Framebuffer *framebuffers_[2];
	glm::vec4 colour_;

	struct IDPair {
		const GLint id;
		const std::string name;
		IDPair(GLint id, std::string name) : id(id), name(name) {}
		~IDPair() {}
	};
	typedef std::vector< IDPair* > ShaderVariableIDs;
	typedef ShaderVariableIDs ShaderUniformIDs;
	ShaderVariableIDs init_variable_ids_, update_variable_ids_, draw_variable_ids_;
	ShaderUniformIDs init_uniform_ids_, update_uniform_ids_, draw_uniform_ids_;

	bool initShaders();
	bool initFramebuffers();
	void getShaderVariableIDs(GLuint shaderProgramID, ShaderVariableIDs &ids, std::vector<std::string> names);
	void getShaderUniformIDs(GLuint shaderProgramID, ShaderUniformIDs &ids, std::vector<std::string> names);
	void getShaderAttributes();
	void createQuadBuffer();
	void createUVBuffer();
	void initParticleDrawing(const units::Pixel viewportWidth, const units::Pixel viewportHeight);
	void swapFramebuffers();
	void setWindowForUpdate();
};

namespace ambient_particle_system {
	// Shader file paths from the main directory.
	const char* const AMB_PART_INIT_VERT_PATH = "shaders/AmbientParticleInit.vert";
	const char* const AMB_PART_INIT_FRAG_PATH = "shaders/AmbientParticleInit.frag";
	const char* const AMB_PART_UPDATE_VERT_PATH = "shaders/AmbientParticleUpdate.vert";
	const char* const AMB_PART_UPDATE_FRAG_PATH = "shaders/AmbientParticleUpdate.frag";
	const char* const AMB_PART_DRAW_VERT_PATH = "shaders/AmbientParticleDraw.vert";
	const char* const AMB_PART_DRAW_FRAG_PATH = "shaders/AmbientParticleDraw.frag";
	
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
			const char* const update_unis_char[] = { "uResolution", "uElapsedTime", "uCohesiveness", "uGravity", "uActiveGrav", "uTexPos", "uTexVel" };
		}
		enum Variables { IN_VERTEX_POS = 0, TOTAL_VARS };
		enum Uniforms { U_RESOLUTION = 0, U_ELAPSED_TIME, U_COHESIVENESS, U_GRAVITY, U_ACTIVE_GRAV, U_TEX_0, U_TEX_1, TOTAL_UNIFORMS };
		const std::vector<std::string> VARIABLE_NAMES(h::update_vars_char, std::end(h::update_vars_char));
		const std::vector<std::string> UNIFORM_NAMES(h::update_unis_char, std::end(h::update_unis_char));
	}
	namespace draw {
		namespace h {
			const char* const draw_vars_char[] = { "inUV" };
			const char* const draw_unis_char[] = { "uPointSize", "uPVM", "uColour", "uTexPos", "uTexVel" };
		}
		enum Variables { IN_UV = 0, TOTAL_VARS };
		enum Uniforms { U_POINT_SIZE = 0, U_PVM, U_COLOUR, U_TEX_0, U_TEX_1, TOTAL_UNIFORMS };
		const std::vector<std::string> VARIABLE_NAMES(h::draw_vars_char, std::end(h::draw_vars_char));
		const std::vector<std::string> UNIFORM_NAMES(h::draw_unis_char, std::end(h::draw_unis_char));
	}
	const unsigned int NUM_TEXTURES_PER_FRAMEBUFFER = 2;
}


#endif // _AMBIENT_PARTICLE_SYSTEM_H