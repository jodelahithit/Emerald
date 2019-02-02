#include "stdafx.h"

Buffer::Buffer(GLfloat* data, GLuint count, GLuint componentCount, GLenum usage) : m_componentCount(componentCount) {
    GL(glGenBuffers(1, &m_bufferID));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));

    GL(glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), data, usage));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_bufferID);
}

void Buffer::Bind() {
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));
}
void Buffer::Unbind() {
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


