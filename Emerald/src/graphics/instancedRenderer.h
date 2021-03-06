#pragma once

class InstancedRenderer{
private:
    bool m_started;
    bool m_ended;
    Mesh* m_mesh;
    const uint MAX_OBJECTS = 4096 * 4;
    VertexBuffer* m_offsetsBuffer;
    int m_amount;
    int m_numindices;
    Vector3* m_offsets;
    Vector3* m_offsetsPtr;

    void Initialize();
public:
    void Begin();
    void Submit(Vector3& offset);
    void End();
    void Draw(uint mode = GL_TRIANGLES);

    InstancedRenderer(Mesh* mesh) : m_mesh(mesh) { Initialize(); }
    ~InstancedRenderer();
};