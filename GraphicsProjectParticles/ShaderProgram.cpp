#include "ShaderProgram.h"

#include "GL\glew.h"
#include "SDL2\SDL_opengl.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

namespace {
	// Some utility functions.

	std::string getNameFromType(const GLuint type) {
		return	type == GL_VERTEX_SHADER ? "vertex" : 
				type == GL_FRAGMENT_SHADER ? "fragment" : 
				type == GL_GEOMETRY_SHADER ? "geometry" : 
				"unknown type";
	}
	std::string readFile(const char* file_path) {
		std::string fileData;
		std::ifstream fileStream(file_path, std::ios::in);
		if(!fileStream.is_open()){
			std::cout << "Error: the file \"" << file_path << "\" could not be opened." << std::endl;
			return "";
		}
		std::string Line = "";
		while(getline(fileStream, Line)) {
			fileData += "\n" + Line;
		}
		fileStream.close();
		if ( fileData.empty() ) {
			std::cout << "Error: The file \"" << file_path << "\" was empty or could not be read." << std::endl;
		}
		return fileData;
	}
}

ShaderProgram::ShaderProgram(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path) 
	: shader_program_(0) {
		if ( vertex_file_path ) {
			shaders_.push_back( ShaderInfo(vertex_file_path, GL_VERTEX_SHADER) );
		}
		if ( fragment_file_path ) {
			shaders_.push_back( ShaderInfo(fragment_file_path, GL_FRAGMENT_SHADER) );
		}
		if ( geometry_file_path ) {
			shaders_.push_back( ShaderInfo(geometry_file_path, GL_GEOMETRY_SHADER) );
		}
}
ShaderProgram::~ShaderProgram() {
	unload();
}
GLuint ShaderProgram::getProgramID() const {
	return shader_program_;
}
void ShaderProgram::use() const {
	glUseProgram( shader_program_ );
}
void ShaderProgram::stopUsing() const {
	glUseProgram ( 0 );
}

bool ShaderProgram::load() {
	if ( shader_program_ ) {
		unload();
	}
	shader_program_ = glCreateProgram();
	// Compile the shaders and add them to the program.
	for (int i = 0; i < shaders_.size(); ++i) {
		if ( !compileShader( shaders_[i] ) ) {
			unload();
			return false;
		}
	}
	// Link the program.
	glLinkProgram(shader_program_);
	GLint result = GL_FALSE;
	glGetProgramiv(shader_program_, GL_LINK_STATUS, &result);
	if ( result == GL_FALSE ) {
		int logLen;
		glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &logLen);
		if ( logLen > 0 ){
			std::vector<char> log(logLen+1);
			glGetProgramInfoLog(shader_program_, logLen, NULL, &log[0]);
			std::cout << "Error: the program could not be linked." << std::endl << &log[0] << std::endl;
		}
		unload();
		return false;
	}
	// Clean up.
	for (int i = 0; i < shaders_.size(); ++i) {
		shaders_[i].unload( shader_program_ );
	}
	return true;
}
void ShaderProgram::unload() {
	if ( shader_program_ ) {
		for (int i = 0; i < shaders_.size(); ++i) {
			shaders_[i].unload( shader_program_ );
		}
		glDeleteProgram( shader_program_ );
		shader_program_ = 0;
	}
}

bool ShaderProgram::compileShader(ShaderInfo shaderInfo) {
	shaderInfo.setID( glCreateShader( shaderInfo.type ) );
	std::string shaderCode = readFile( shaderInfo.filePath );
	if ( shaderCode.empty() ) {
		return 0;
	}
	GLint result = GL_FALSE;
	const char* charCode = shaderCode.c_str();
	// Compile Shader
	glShaderSource(shaderInfo.getID(), 1, &charCode, NULL);
	glCompileShader( shaderInfo.getID() );

	// Check Shader
	glGetShaderiv(shaderInfo.getID(), GL_COMPILE_STATUS, &result);
	if ( result == GL_FALSE ) {
		std::string type_s = getNameFromType(shaderInfo.type);
		std::cout << "Error: Failed to compile " << type_s<< " shader." << std::endl;
		int logLen;
		glGetShaderiv(shaderInfo.getID(), GL_INFO_LOG_LENGTH, &logLen);
		if ( logLen > 0 ){
			std::vector<char> log(logLen+1);
			glGetShaderInfoLog(shaderInfo.getID(), logLen, NULL, &log[0]);
			std::cout << "Error: the " << type_s << " shader could not be compiled." << std::endl << &log[0] << std::endl;
		}
		return false;
	}
	glAttachShader(shader_program_, shaderInfo.getID());
	return true;
}

// ShaderInfo functions.

void ShaderProgram::ShaderInfo::unload(const GLuint program) {
	if ( id_ ) {
		if ( program ) {
			glDetachShader(program, id_);
		}
		glDeleteShader( id_ );
		id_ = 0;
	}
}
ShaderProgram::ShaderInfo::~ShaderInfo() {
	if ( id_ ) {
		glDeleteShader( id_ );
		id_ = 0;
	}
}