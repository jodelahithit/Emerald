#pragma once
class VertexBuffer : public Ref<VertexBuffer> {
private:
	uint32 m_bufferID;
	BufferLayout m_layout;
	GLfloat* m_data;
public:
	VertexBuffer() {}
	
	template<typename T>
	VertexBuffer(T* data, uint32 vertexCount, BufferLayout layout, GLenum usage = GL_STATIC_DRAW) : m_layout(layout), m_data((float*)data) {
		GL(glGenBuffers(1, &m_bufferID));
		SetData(data, vertexCount, usage);
	}

	~VertexBuffer() {
		glDeleteBuffers(1, &m_bufferID);
	}

	void ApplyLayout(uint32 attributeIndex) {
		Bind();
		m_layout.Apply(attributeIndex);
		Unbind();
	}

	template<typename T>
	void SetData(T* data, uint32 vertexCount, GLenum usage = GL_STATIC_DRAW) {
		Bind();
		GL(glBufferData(GL_ARRAY_BUFFER, vertexCount * m_layout.GetTotalComponentCountSize(), (const void*)data, usage));
		Unbind();
	}

	inline const BufferLayout& GetLayout() { return m_layout; }
	inline void SetLayout(BufferLayout& layout) { m_layout = layout; }

	inline uint32 GetHandle() { return m_bufferID; }

	inline void Bind() {
		GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));
	}
	inline void Unbind() {
		GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	template<typename T>
	T* Map() {
		Bind();
		T* mapped;
		GL(mapped = (T*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY | GL_MAP_UNSYNCHRONIZED_BIT));
		return mapped;
	}

	void Unmap() {
		Bind();
		GL(glUnmapBuffer(GL_ARRAY_BUFFER));
	}

};

