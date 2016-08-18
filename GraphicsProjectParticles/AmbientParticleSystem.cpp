#include "AmbientParticleSystem.h"

#include <iostream>
#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"
#include "glm/glm.hpp"
#include "Units.h"
#include "Constants.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "Framebuffer.h"

using namespace ambient_particle_system;

AmbientParticleSystem::AmbientParticleSystem(unsigned int dimensions, glm::vec4 pointColour) :
	particle_texture_width_(dimensions), particle_texture_height_(dimensions), colour_(pointColour),
	uv_buffer_(0), quad_buffer_(0), is_paused_(false),
	init_shader_(0), update_shader_(0), draw_shader_(0)
{
	framebuffers_[0] = 0;
	framebuffers_[1] = 0;
}
AmbientParticleSystem::AmbientParticleSystem(unsigned int width, unsigned int height, glm::vec4 pointColour) :
	particle_texture_width_(width), particle_texture_height_(height), colour_(pointColour),
	uv_buffer_(0), quad_buffer_(0), is_paused_(false),
	init_shader_(0), update_shader_(0), draw_shader_(0)
{
	framebuffers_[0] = 0;
	framebuffers_[1] = 0;
}

AmbientParticleSystem::~AmbientParticleSystem() {
	if ( uv_buffer_ )		{ delete uv_buffer_; }
	if ( quad_buffer_ )		{ delete quad_buffer_; }
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
	if ( init_shader_ )		{ delete init_shader_; }
	if ( update_shader_ )	{ delete update_shader_; }
	if ( draw_shader_ )		{ delete draw_shader_; }
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
bool AmbientParticleSystem::initFramebuffers() {
	if ( framebuffers_[0] ) { delete framebuffers_[0];}
	if ( framebuffers_[1] ) { delete framebuffers_[1];}
	framebuffers_[0] = new Framebuffer(particle_texture_width_, particle_texture_height_, NUM_TEXTURES_PER_FRAMEBUFFER);
	framebuffers_[1] = new Framebuffer(particle_texture_width_, particle_texture_height_, NUM_TEXTURES_PER_FRAMEBUFFER);

	return framebuffers_[0]->init() && framebuffers_[1]->init();
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
void AmbientParticleSystem::getShaderAttributes() {
	init_variable_ids_.clear();
	update_variable_ids_.clear();
	draw_variable_ids_.clear();
	getShaderVariableIDs(init_shader_->getProgramID(), init_variable_ids_, init::VARIABLE_NAMES);
	getShaderVariableIDs(update_shader_->getProgramID(), update_variable_ids_, update::VARIABLE_NAMES);
	getShaderVariableIDs(draw_shader_->getProgramID(), draw_variable_ids_, draw::VARIABLE_NAMES);

	init_uniform_ids_.clear();
	update_uniform_ids_.clear(); 
	draw_uniform_ids_.clear();
	getShaderUniformIDs(init_shader_->getProgramID(), init_uniform_ids_, init::UNIFORM_NAMES);
	getShaderUniformIDs(update_shader_->getProgramID(), update_uniform_ids_, update::UNIFORM_NAMES);
	getShaderUniformIDs(draw_shader_->getProgramID(), draw_uniform_ids_, draw::UNIFORM_NAMES);
}
void AmbientParticleSystem::createQuadBuffer() {
	if ( quad_buffer_ )	{ delete quad_buffer_; }
	// Create quad for textures to draw onto.
	static const GLfloat quad_array[] = {
		-1.0f,	-1.0f,  0.0f,	
		 1.0f,	 1.0f,  0.0f,
		-1.0f,	 1.0f,  0.0f,
		-1.0f,	-1.0f,  0.0f,
		 1.0f,	-1.0f,  0.0f,
		 1.0f,	 1.0f,  0.0f,
	};
	quad_buffer_ = new VertexBuffer( 3, 6, quad_array );
}
void AmbientParticleSystem::createUVBuffer() {
	if ( uv_buffer_ )	{ delete uv_buffer_; }
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
	uv_buffer_ = new VertexBuffer(2, count, uv_data);
	uv_data.clear();
}
bool AmbientParticleSystem::init() {
	if ( particle_texture_height_ < 1 || particle_texture_width_ < 1 ) {
		std::cerr << "Error: Texture width and height must be greater than zero!" << std::endl;
		return false;
	}
	if ( !initShaders() ) {
		std::cerr << "Error: Failed to initialize shaders." << std::endl;
		return false;
	}
	if ( !initFramebuffers() ) {
		std::cerr << "Error: Failed to create framebuffers." << std::endl;
		return false;
	}
	glEnable( GL_PROGRAM_POINT_SIZE );
	// Get handles for variables and uniforms in the shader programs.
	getShaderAttributes();

	// Create vertex buffers.
	createQuadBuffer();
	createUVBuffer();
	
	initParticleDrawing();
	return true;
}
bool AmbientParticleSystem::resize(unsigned int dimensions) {
	return resize(dimensions, dimensions);
}
bool AmbientParticleSystem::resize(unsigned int width, unsigned int height) {
	particle_texture_width_ = width;
	particle_texture_height_ = height;

	// Create new framebuffer and uv buffer objects for the new size.
	if ( !initFramebuffers() ) {
		std::cerr << "Error: Failed to create framebuffers." << std::endl;
		return false;
	}
	createUVBuffer();

	initParticleDrawing();
	return true;
}
void AmbientParticleSystem::initParticleDrawing() {
	framebuffers_[0]->drawTo();
	std::vector<GLint> prevViewport = setWindowForUpdate();
	init_shader_->use();
	glUniform2f( init_uniform_ids_[init::U_RESOLUTION]->id, particle_texture_width_, particle_texture_height_ );

	quad_buffer_->bind();
	GLint attrib = init_variable_ids_[init::IN_VERTEX_POS]->id;
	glEnableVertexAttribArray( attrib );
	glVertexAttribPointer( attrib, quad_buffer_->vertexSize,
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset
	glDrawArrays( GL_TRIANGLES, 0, quad_buffer_->numVertices );

	quad_buffer_->unbind();
	framebuffers_[0]->stopDrawingTo();
	glDisableVertexAttribArray( attrib );
	init_shader_->stopUsing();
	// Return the viewport to its previous state.
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );
}
void AmbientParticleSystem::update( const units::MS elapsedTime, const glm::vec4 gravityObjs[constants::MAX_GRAV_OBJECTS], 
								   const unsigned int activeGravObjs[constants::MAX_GRAV_OBJECTS], const unsigned int cohesiveness) {
	if ( is_paused_ ) {
		return;
	}
	update_shader_->use();
	// Bind the framebuffer that wasn't updated last time (or initialized from).
	framebuffers_[1]->drawTo();
	// Bind our uniform/variable handles to the update shader.
	glUniform2f( update_uniform_ids_[update::U_RESOLUTION]->id, particle_texture_width_, particle_texture_height_ );
	glUniform1f( update_uniform_ids_[update::U_ELAPSED_TIME]->id, units::millisToSeconds(elapsedTime) );
	glUniform1i( update_uniform_ids_[update::U_COHESIVENESS]->id, cohesiveness);
	glUniform4fv( update_uniform_ids_[update::U_GRAVITY]->id, constants::MAX_GRAV_OBJECTS, &gravityObjs[0][0]);
	glUniform1uiv( update_uniform_ids_[update::U_ACTIVE_GRAV]->id, constants::MAX_GRAV_OBJECTS, &activeGravObjs[0]);

	std::vector<GLint> prevViewport = setWindowForUpdate();

	// Bind our textures.
	GLint textureUniformLocations[NUM_TEXTURES_PER_FRAMEBUFFER];
	for (unsigned int i = 0; i < NUM_TEXTURES_PER_FRAMEBUFFER; ++i) {
		 textureUniformLocations[i] = update_uniform_ids_[update::U_TEX_0 + i]->id;
	}
	framebuffers_[0]->bindTextures( NUM_TEXTURES_PER_FRAMEBUFFER, textureUniformLocations );

	GLint attrib = update_variable_ids_[update::IN_VERTEX_POS]->id;
	quad_buffer_->bind();
	glEnableVertexAttribArray( attrib );
	glVertexAttribPointer( attrib, quad_buffer_->vertexSize,
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset
	glDrawArrays( GL_TRIANGLES, 0, quad_buffer_->numVertices);

	glDisableVertexAttribArray( attrib );
	quad_buffer_->unbind();
	framebuffers_[0]->unbindTextures();
	framebuffers_[1]->stopDrawingTo();

	// Return the viewport to its previous state.
	glViewport( prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );
	update_shader_->stopUsing();
	swapFramebuffers();
}

void AmbientParticleSystem::swapFramebuffers() {
	Framebuffer *temp = framebuffers_[0];
	framebuffers_[0] = framebuffers_[1];
	framebuffers_[1] = temp;
}
std::vector<GLint> AmbientParticleSystem::setWindowForUpdate() {
	// Store the previous viewport.
	GLint prevViewport[4];
	glGetIntegerv( GL_VIEWPORT, prevViewport );
	glViewport( 0, 0, particle_texture_width_, particle_texture_height_ );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST ); // Draw everything flat.
	glBlendFunc( GL_ONE, GL_ZERO );
	std::vector<GLint> viewport(prevViewport, prevViewport + 4);
	return viewport;
}

void AmbientParticleSystem::draw( const glm::mat4 &PVM, const unsigned int pointSize) {
	draw_shader_->use();
	// Enable additive blending, so overlapping particles appear brighter.
	glBlendFunc( GL_SRC_ALPHA, GL_ONE ); 
	
	glUniformMatrix4fv( draw_uniform_ids_[draw::U_PVM]->id, 1, GL_FALSE, &PVM[0][0] );
	glUniform4f( draw_uniform_ids_[draw::U_COLOUR]->id, colour_.r, colour_.g, colour_.b, colour_.a);
	glUniform1f( draw_uniform_ids_[draw::U_POINT_SIZE]->id, pointSize);

	// Bind our textures.
	GLint textureUniformLocations[NUM_TEXTURES_PER_FRAMEBUFFER];
	for (unsigned int i = 0; i < NUM_TEXTURES_PER_FRAMEBUFFER; ++i) {
		 textureUniformLocations[i] = draw_uniform_ids_[draw::U_TEX_0 + i]->id;
	}
	framebuffers_[0]->bindTextures( NUM_TEXTURES_PER_FRAMEBUFFER, textureUniformLocations );
	GLint attrib = draw_variable_ids_[draw::IN_UV]->id;
	uv_buffer_->bind();
	glEnableVertexAttribArray( attrib );
	glVertexAttribPointer( attrib, uv_buffer_->vertexSize,
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset
	glDrawArrays( GL_POINTS, 0, uv_buffer_->numVertices );

	glDisableVertexAttribArray( attrib );
	framebuffers_[0]->unbindTextures();
	uv_buffer_->unbind();
	draw_shader_->stopUsing();
}

void AmbientParticleSystem::setParticleColour(glm::vec4 colour) {
	colour_ = colour;
}
void AmbientParticleSystem::setParticleColourNoAlpha(glm::vec3 colour) {
	colour_ = glm::vec4(colour, colour_.a);
}
void AmbientParticleSystem::setParticleAlpha(float alpha) {
	colour_.a = alpha;
}