#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include <vector>
#include "GL\glew.h"

class VertexBuffer {

private:
	GLuint array_id_, buffer_id_;

public:
	const unsigned int vertexSize;
	const unsigned int numVertices;
	const std::vector<GLfloat> vertices;

	VertexBuffer(unsigned int vertex_size, unsigned int num_vertices, std::vector<GLfloat> vertices)
		: vertexSize(vertex_size), numVertices(num_vertices), vertices(vertices) {
			// Get the vertex and buffer array ids.
			glGenVertexArrays( 1, &array_id_ );
			glBindVertexArray( array_id_ );

			glGenBuffers(1, &buffer_id_);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

			glBindVertexArray( 0 );
			glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	~VertexBuffer() {
		glDeleteVertexArrays( 1, &array_id_ );
		glDeleteBuffers( 1, &buffer_id_ );
	}

	GLuint getArrayID() { return array_id_; }
	GLuint getBufferID() { return buffer_id_; }
	const GLfloat* getDataPointer() const { return &vertices[0]; }
};


#endif // _VERTEX_BUFFER_H