#include "VertexBuffer.h"

#include "GL/glew.h"
#include <vector>


VertexBuffer::VertexBuffer(unsigned int vertex_size, unsigned int num_vertices, std::vector<GLfloat> &vertices)
	: vertexSize(vertex_size), numVertices(num_vertices), array_id_(0), buffer_id_(0) {
		init( vertices );
}
VertexBuffer::VertexBuffer(unsigned int vertex_size, unsigned int num_vertices, const GLfloat vertices[] )
	: vertexSize(vertex_size), numVertices(num_vertices), array_id_(0), buffer_id_(0) {
		std::vector<GLfloat> vert(vertices, vertices + vertexSize*numVertices);
		init( vert );
}

VertexBuffer::~VertexBuffer() {
	glDeleteVertexArrays( 1, &array_id_ );
	glDeleteBuffers( 1, &buffer_id_ );
}

void VertexBuffer::init(std::vector<GLfloat> &vertices) {
	// Get the vertex and buffer array ids.
	glGenVertexArrays( 1, &array_id_ );
	glBindVertexArray( array_id_ );

	glGenBuffers(1, &buffer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
	glBufferData(GL_ARRAY_BUFFER, vertexSize * numVertices * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glBindVertexArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::bind() {
	glBindVertexArray( array_id_ );
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id_ );
}
void VertexBuffer::unbind() {
	glBindVertexArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}