#include "AmbientParticleSystem.h"

#include <iostream>
#include "GL\glew.h"
#include "SDL2\SDL_opengl.h"
#include "glm\glm.hpp"
#include "Units.h"
#include "Constants.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "Framebuffer.h"

using namespace constants;

#define NUM_TEXTURES_PER_FRAMEBUFFER 3

namespace {



}

AmbientParticleSystem::AmbientParticleSystem(unsigned int dimen) :
	particle_texture_width_(units::getPowerOf2(dimen)), particle_texture_height_(units::getPowerOf2(dimen)),
	uv_(0), full_size_quad_(0), is_paused_(false),
	init_shader_(0), update_shader_(0), draw_shader_(0)
{
	framebuffers_[0] = 0;
	framebuffers_[1] = 0;
}
AmbientParticleSystem::AmbientParticleSystem(unsigned int width, unsigned int height) :
	particle_texture_width_(units::getPowerOf2(width)), particle_texture_height_(units::getPowerOf2(height)), 
	uv_(0), full_size_quad_(0), is_paused_(false),
	init_shader_(0), update_shader_(0), draw_shader_(0)
{
	framebuffers_[0] = 0;
	framebuffers_[1] = 0;
}

AmbientParticleSystem::~AmbientParticleSystem() {
	if ( uv_ )				{ delete uv_; }
	if ( full_size_quad_ )	{ delete full_size_quad_; }
	if ( init_shader_ )		{ delete init_shader_; }
	if ( update_shader_ )	{ delete update_shader_; }
	if ( draw_shader_ )		{ delete draw_shader_; }
	if ( framebuffers_[0] ) { delete framebuffers_[0];}
	if ( framebuffers_[1] ) { delete framebuffers_[1];}
	init_variable_ids_.clear();
	update_variable_ids_.clear();
	draw_variable_ids_.clear();
	init_uniform_ids_.clear();
	update_uniform_ids_.clear(); 
	draw_uniform_ids_.clear();
}

bool AmbientParticleSystem::initShaders() {
	init_shader_ = new ShaderProgram(AMB_PART_INIT_VERT_PATH, AMB_PART_INIT_FRAG_PATH);
	if ( !init_shader_->load() ) {
		std::cerr << "Failed to load particle initialization shader." << std::endl;
		return false;
	}
	update_shader_ = new ShaderProgram(AMB_PART_UPDATE_VERT_PATH, AMB_PART_UPDATE_FRAG_PATH);
	if ( !update_shader_->load() ) {
		std::cerr << "Failed to load particle update shader." << std::endl;
		return false;
	}
	draw_shader_ = new ShaderProgram(AMB_PART_DRAW_VERT_PATH, AMB_PART_DRAW_FRAG_PATH);
	if ( !draw_shader_->load() ) {
		std::cerr << "Failed to load particle drawing shader." << std::endl;
		return false;
	}
	return true;
}
void AmbientParticleSystem::getShaderVariableIDs(GLuint shaderProgramID, ShaderVariableIDs &ids, std::vector<std::string> names) {
	for (int i = 0; i < names.size(); ++i ) {
		GLint id = glGetAttribLocation( shaderProgramID, names[i].c_str() );
		ids.push_back( new IDPair(id, names[i]) );
	}
}
void AmbientParticleSystem::getShaderUniformIDs(GLuint shaderProgramID, ShaderUniformIDs &ids, std::vector<std::string> names) {
	for (int i = 0; i < names.size(); ++i ) {
		GLint id = glGetUniformLocation( shaderProgramID, names[i].c_str() );
		ids.push_back( new IDPair(id, names[i]) );
	}
}
void AmbientParticleSystem::initVertexBuffers() {
	// Build the UV coordinate array for the given dimensions.
	// This is what gives us the number of particles (it functions as the particle veretx data by indexing to uv coordinates).
	unsigned int count = particle_texture_width_ * particle_texture_height_;
	std::vector<GLfloat> uv_data;
	uv_data.reserve(count * 2);
	// We do a lot of these, so compute the divisions once for faster multiplication operations in the loop.
	const float heightRecip = 1.0f/particle_texture_height_;
	const float widthRecip = 1.0f/particle_texture_width_;
	for (unsigned int y = 0; y < particle_texture_height_; ++y) {
		for (unsigned int x = 0; x < particle_texture_width_; ++x) {
			uv_data.push_back((GLfloat)(x) * widthRecip);
			uv_data.push_back((GLfloat)(y) * heightRecip);
		}
	}
	uv_ = new VertexBuffer(2, count, uv_data);

	// Create quad for textures to draw onto.
	static const GLfloat quad_array[] = {
		-1.0f,	-1.0f,  0.0f,	
		 1.0f,	 1.0f,  0.0f,
		-1.0f,	 1.0f,  0.0f,
		-1.0f,	-1.0f,  0.0f,
		 1.0f,	-1.0f,  0.0f,
		 1.0f,	 1.0f,  0.0f,
	};
	std::vector<GLfloat> quad(quad_array, quad_array + 18);
	full_size_quad_ = new VertexBuffer(3, 6, quad);
}

bool AmbientParticleSystem::initFramebuffers() {
	framebuffers_[0] = new Framebuffer(particle_texture_width_, particle_texture_height_, NUM_TEXTURES_PER_FRAMEBUFFER);
	framebuffers_[1] = new Framebuffer(particle_texture_width_, particle_texture_height_, NUM_TEXTURES_PER_FRAMEBUFFER);

	return framebuffers_[0]->init() && framebuffers_[1]->init();
}
bool AmbientParticleSystem::init() {
	if ( !initShaders() ) {
		std::cerr << "Error: failed to initialize shaders." << std::endl;
		return false;
	}
	if ( !initFramebuffers() ) {
		std::cerr << "Error: failed to create framebuffers." << std::endl;
		return false;
	}
	// Get handles for variables and uniforms in these shader programs.
	getShaderVariableIDs(init_shader_->getProgramID(), init_variable_ids_, constants::init::VARIABLE_NAMES);
	getShaderVariableIDs(update_shader_->getProgramID(), update_variable_ids_, constants::update::VARIABLE_NAMES);
	getShaderVariableIDs(draw_shader_->getProgramID(), draw_variable_ids_, constants::draw::VARIABLE_NAMES);
	
	getShaderUniformIDs(init_shader_->getProgramID(), init_uniform_ids_,constants::init::UNIFORM_NAMES);
	getShaderUniformIDs(update_shader_->getProgramID(), update_uniform_ids_, constants::update::UNIFORM_NAMES);
	getShaderUniformIDs(draw_shader_->getProgramID(), draw_uniform_ids_, constants::draw::UNIFORM_NAMES);

	initVertexBuffers();

	// Set resolution uniforms for the init and update shader programs.
	init_shader_->use();
	glUniform2f( init_uniform_ids_[constants::init::Uniforms::U_RESOLUTION]->id, particle_texture_width_, particle_texture_height_ );
	update_shader_->use();
	glUniform2f( update_uniform_ids_[constants::update::Uniforms::U_RESOLUTION]->id, particle_texture_width_, particle_texture_height_ );
	update_shader_->stopUsing();
	
	initParticleDrawing();
	glEnable( GL_PROGRAM_POINT_SIZE );
	return true;
}
void AmbientParticleSystem::initParticleDrawing() {
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, framebuffers_[0]->getBufferID() );
	// Store the previous viewport.
	GLint prevViewport[4];
	glGetIntegerv( GL_VIEWPORT, prevViewport );
	glViewport( 0, 0, particle_texture_width_, particle_texture_height_ );
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST );
	glBlendFunc( GL_ONE, GL_ZERO);

	init_shader_->use();
	GLint attrib = init_variable_ids_[constants::init::Variables::IN_VERTEX_POS]->id;
	glEnableVertexAttribArray( attrib );
	glBindBuffer( GL_ARRAY_BUFFER, full_size_quad_->getBufferID() );
	glVertexAttribPointer( attrib, full_size_quad_->vertexSize,
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset
	glDrawArrays( GL_TRIANGLES, 0, full_size_quad_->numVertices );


	glBindTexture( GL_TEXTURE_2D, framebuffers_[0]->getTexture(0) );
	GLfloat *pixels = new GLfloat[particle_texture_width_ * particle_texture_height_ * 4];
	glReadPixels( 0, 0, particle_texture_width_, particle_texture_height_, GL_RGBA, GL_FLOAT, pixels);

	std::cout << "some pixel entries:\n";
	std::cout << "pixel0:     " << pixels[0] << "  " << pixels[1] << "  " << pixels[2] << "  " << pixels[3] << std::endl;
	std::cout << "pixel10:    " << pixels[40] << "  " << pixels[41] << "  " << pixels[42] << "  " << pixels[43] << std::endl;
	std::cout << "pixel100:   " << pixels[400] << "  " << pixels[401] << "  " << pixels[402] << "  " << pixels[403] << std::endl;
	std::cout << "pixel10000: " << pixels[40000] << "  " << pixels[40001] << "  " << pixels[40002] << "  " << pixels[40003] << std::endl;

	delete pixels;

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( attrib );
	init_shader_->stopUsing();
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	// Return the viewport to its previous state.
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );
}
void AmbientParticleSystem::update(units::MS elapsedTime, glm::vec3 gravityPos) {
	if ( is_paused_ ) {
		return;
	}
	update_shader_->use();
	// Bind our uniform/variable handles to the update shader (resolution is already bound).
	glUniform1f( update_uniform_ids_[constants::update::U_DELTA_T]->id, 0.05f );
	glUniform1f( update_uniform_ids_[constants::update::U_K_FORCE]->id, 5.0f);
	glUniform3f( update_uniform_ids_[constants::update::U_INPUT_POS]->id, gravityPos.x, gravityPos.y, gravityPos.z );
	drawUpdateBuffer();
	update_shader_->stopUsing();
	swapFramebuffers();
}
void AmbientParticleSystem::drawUpdateBuffer() {
	// Assumes we're already using update shader program.
	// Bind the framebuffer that wasn't updated last time (or initialized from).
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffers_[1]->getBufferID() );
	// Store the previous viewport.
	GLint prevViewport[4];
	glGetIntegerv( GL_VIEWPORT, prevViewport );
	glViewport( 0, 0, framebuffers_[1]->width, framebuffers_[1]->height );
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST ); // Draw everything flat.
	glBlendFunc( GL_ONE, GL_ZERO );

	// Bind our textures.
	for (unsigned int i = 0; i < framebuffers_[0]->numTextures; ++i) {
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( GL_TEXTURE_2D, framebuffers_[0]->getTexture(i) );
		glUniform1i( update_uniform_ids_[constants::update::Uniforms::U_TEX_0 + i]->id, i );
	}
	GLint attrib = update_variable_ids_[constants::update::Variables::IN_VERTEX_POS]->id;
	glEnableVertexAttribArray( attrib );
	glBindBuffer( GL_ARRAY_BUFFER, full_size_quad_->getBufferID() );
	glVertexAttribPointer( attrib, full_size_quad_->vertexSize,
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset

	glDrawArrays( GL_TRIANGLES, 0, full_size_quad_->numVertices);

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( attrib );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	// Return the viewport to its previous state.
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );
}

void AmbientParticleSystem::swapFramebuffers() {
	Framebuffer *temp = framebuffers_[0];
	framebuffers_[0] = framebuffers_[1];
	framebuffers_[1] = temp;
}

void AmbientParticleSystem::draw( glm::mat4 PVM ) {
	draw_shader_->use();
	// Enable additive blending, so overlapping particles appear brighter.
	glBlendFunc( GL_SRC_ALPHA, GL_ONE ); 
	
	glUniformMatrix4fv( update_uniform_ids_[constants::draw::Uniforms::U_PVM]->id, 1, GL_FALSE, &PVM[0][0] );
	glUniform4f(draw_uniform_ids_[constants::draw::Uniforms::U_COLOUR]->id, 0.0f, 0.1f, 1.0f, 0.6f);
	glUniform1f(draw_uniform_ids_[constants::draw::Uniforms::U_POINT_SIZE]->id, 4.0f);

	// Bind our textures.
	for (unsigned int i = 0; i < framebuffers_[0]->numTextures; ++i) {
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( GL_TEXTURE_2D, framebuffers_[0]->getTexture(i) );
		glUniform1i( draw_uniform_ids_[constants::draw::Uniforms::U_TEX_0 + i]->id, i );
	}
	GLint attrib = draw_variable_ids_[constants::draw::Variables::IN_UV]->id;
	glEnableVertexAttribArray( attrib );
	glBindBuffer( GL_ARRAY_BUFFER, uv_->getBufferID() );
	glVertexAttribPointer( attrib, uv_->vertexSize,
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset

	glDrawArrays( GL_POINTS, 0, uv_->numVertices );

	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( attrib );
	draw_shader_->stopUsing();
}