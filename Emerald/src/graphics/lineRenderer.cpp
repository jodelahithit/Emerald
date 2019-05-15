#include "stdafx.h"

LineRenderer::~LineRenderer() {
	delete[] m_lines;
	delete m_shader;
}

void LineRenderer::Initialize() {
	m_lines = new Vector3[MAX_OBJECTS * 2];

	m_lineBuffer = new Buffer((float*)m_lines, MAX_OBJECTS * 2 * 3, 3, GL_DYNAMIC_DRAW);

	Vector3 vertices[2] = { Vector3(0), Vector3(0) };
	VertexArray* vao = new VertexArray();
	vao->AddBuffer(new Buffer((float*)vertices, 2 * 3, 3), 0, false);
	vao->AddBuffer(m_lineBuffer, 1, true);

	vector<uint> indices;
	indices.push_back(0);
	indices.push_back(1);

	IndexBuffer* ibo = new IndexBuffer(indices.data(), indices.size());
	m_mesh = new Mesh(vao, ibo);

	m_shader = new Shader("Line", "src/shader/line.vert", "src/shader/line.frag");

	LOG("[~bEEngine~x] Line renderer initialized");
}

void LineRenderer::Begin() {
	m_amount = 0;
	m_ended = false;
	m_started = true;

	m_lineBuffer->Bind();
	GL(m_linesPtr = (Vector3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void LineRenderer::Submit(Vector3& begin, Vector3& end) {
	m_linesPtr->x = begin.x;
	m_linesPtr->y = begin.y;
	m_linesPtr->z = begin.z;
	m_linesPtr++;
	m_linesPtr->x = end.x;
	m_linesPtr->y = end.y;
	m_linesPtr->z = end.z;
	m_linesPtr++;
	m_amount++;
}
void LineRenderer::Submit(float x1, float y1, float z1, float x2, float y2, float z2) {
	m_linesPtr->x = x1;
	m_linesPtr->y = y1;
	m_linesPtr->z = z1;
	m_linesPtr++;
	m_linesPtr->x = x2;
	m_linesPtr->y = y2;
	m_linesPtr->z = z2;
	m_linesPtr++;
	m_amount++;
}

void LineRenderer::End() {
	ASSERT(m_started, "Call Renderer::Begin before calling Renderer::End");
	m_lineBuffer->Bind();
	GL(glUnmapBuffer(GL_ARRAY_BUFFER));
	//if (m_amount == 0) return;
	//m_offsetsBuffer->Bind();
	//glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(m_offsets), m_offsets);
	m_started = false;
	m_ended = true;
}
void LineRenderer::Draw() {
	ASSERT(m_ended, "Call Renderer::End before calling Renderer::Draw");
	m_shader->Bind();
	m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
	m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());

	m_lineBuffer->Bind();
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), BUFFEROFFSET(12));
	glVertexAttribDivisor(2, 1);

	m_mesh->GetIBO()->Bind();
	m_mesh->GetIBO()->DrawInstanced(m_amount, GL_LINES);
}