#include "stdafx.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_arrayID);
}
VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_arrayID);
}

void VertexArray::AddBuffer(ManagedRef<VertexBuffer> buffer) {
    for (auto buff : m_buffers) {
        ASSERT(buffer->GetHandle() != buff->GetHandle(), "VAO already contains this buffer");
    }

	//Bind();
	//buffer->Bind();
	//glEnableVertexAttribArray(index);
	//glVertexAttribPointer(index, buffer->GetComponentCount(), GL_FLOAT, GL_FALSE, stride, 0);
	//if (divisor) glVertexAttribDivisor(index, 1);
	//Unbind();
    m_buffers.push_back(buffer);
}

void VertexArray::Bind() {
    glBindVertexArray(m_arrayID);
	//for (auto& buffer : m_buffers) {
	//    buffer->Bind();
	//}
}

void VertexArray::Unbind() {
    glBindVertexArray(0);
    //for (auto& buffer : m_buffers) {
    //    buffer->Unbind();
    //}
}


