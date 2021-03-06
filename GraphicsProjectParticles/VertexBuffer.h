#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include <vector>

#include "GL/glew.h"

class VertexBuffer {
public:
	const unsigned int vertexSize;
	const unsigned int numVertices;

	VertexBuffer(unsigned int vertex_size, unsigned int num_vertices, std::vector<GLfloat> &vertices);
	VertexBuffer(unsigned int vertex_size, unsigned int num_vertices, const GLfloat *vertices );
	~VertexBuffer();

	void bind();
	void unbind();

private:
	GLuint array_id_, buffer_id_;

	void init(std::vector<GLfloat> &vertices);
};


#endif // _VERTEX_BUFFER_H