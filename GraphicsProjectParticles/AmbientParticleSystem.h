#ifndef _AMBIENT_PARTICLE_SYSTEM_H
#define _AMBIENT_PARTICLE_SYSTEM_H

#include <vector>
#include <string>
#include "GL\glew.h"
#include "glm\glm.hpp"
#include "Units.h"

class VertexBuffer;
class ShaderProgram;
class Framebuffer;

class AmbientParticleSystem {
private:
	bool is_paused_;
	const unsigned int particle_texture_width_, particle_texture_height_;
	VertexBuffer *uv_, *full_size_quad_;
	ShaderProgram *init_shader_, *update_shader_, *draw_shader_, *draw_texture_shader_;
	Framebuffer *framebuffers_[2]; // Two frame buffers to swap between.

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
	void getShaderVariableIDs(GLuint shaderProgramID, ShaderVariableIDs &ids, std::vector<std::string> names);
	void getShaderUniformIDs(GLuint shaderProgramID, ShaderUniformIDs &ids, std::vector<std::string> names);
	void initVertexBuffers();
	bool initFramebuffers();
	void initParticleDrawing();
	void drawUpdateBuffer();
	void swapFramebuffers();
public:
	AmbientParticleSystem(unsigned int dimentions_exponent);
	AmbientParticleSystem(unsigned int width_exponent, unsigned int height_exponent);
	~AmbientParticleSystem();

	bool init();
	void update(units::MS elapsedTime, glm::vec3 gravityPos );
	void draw( glm::mat4 PVM );
	void drawTexture(unsigned int texNum);
};


#endif // _AMBIENT_PARTICLE_SYSTEM_H