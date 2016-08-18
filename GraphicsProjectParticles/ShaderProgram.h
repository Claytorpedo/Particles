#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include "GL/glew.h"

#include <vector>


class ShaderProgram {
public:
	struct ShaderInfo {
	private:
		GLuint id_;
	public:
		const char* filePath;
		const GLuint type;

		ShaderInfo(const char* file_path, const GLuint type) : filePath(file_path), type(type), id_(0) {}
		~ShaderInfo();
		void setID(GLuint id) { id_ = id; }
		GLuint getID() { return id_; }
		void unload(const GLuint program);
	};

	ShaderProgram(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path = 0);
	~ShaderProgram();

	GLuint getProgramID() const;
	void use() const;
	void stopUsing() const;
	bool load();
	void unload();

private:
	GLuint shader_program_;
	bool compileShader(ShaderInfo shaderInfo);
	std::vector< ShaderInfo > shaders_;
};


#endif // _SHADER_PROGRAM_H