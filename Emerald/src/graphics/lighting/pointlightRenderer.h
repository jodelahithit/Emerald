#pragma once

class PointlightRenderer : public Singleton<PointlightRenderer> {
private:
	InstancedRenderer2D<Pointlight>* m_renderer;
	PointlightRenderer() {}
	~PointlightRenderer() {
		DELETE(m_renderer);
	}
	friend Singleton;

public:
	static const int32 MAX_LIGHTS = 32768;
	void Initialize(Mesh* mesh, uint32 maxLights = MAX_LIGHTS) {
		if (maxLights > MAX_LIGHTS)LOG_ERROR("[Rendering] Too many lights. Engine max is 32768");

		BufferLayout layout = {
		{ShaderDataType::Float4, "vars", 5, true},
		{ShaderDataType::Float4, "col", 6, true}
		};

		m_renderer = NEW(InstancedRenderer2D<Pointlight>(mesh, MAX_LIGHTS, layout));
	}
	void Begin();
	void End();
	void Draw();
	void Submit(Pointlight& pointlight);
};

static PointlightRenderer* GetPointlightRenderer() { return PointlightRenderer::GetInstance(); }