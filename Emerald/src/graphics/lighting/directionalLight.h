#pragma once

class DirectionalLight {
private:
	Color m_color = Color::White();
	Vector3 m_rotation = Vector3(0.3f, 0.0f, 0.3f);

public:
	float m_multiplier = 2;

	Color GetColor() { return m_color * m_multiplier; }
	Vector3 GetDirection() {
		Matrix4 mat = Matrix4::Identity();
		mat *= Matrix4::Rotate(m_rotation.x, Vector3::XAxis());
		mat *= Matrix4::Rotate(m_rotation.y, Vector3::YAxis());
		mat *= Matrix4::Rotate(m_rotation.z, Vector3::ZAxis());
		Vector3 direction = mat * Vector3::Up();
		direction.Normalize();
		return direction;
	}

	void OnImGui() {
		ImGui::InputFloat3("Direction###1", (float*)&m_rotation, -3, 3);
		ImGui::SliderFloat3("Direction###2", (float*)&m_rotation, -3, 3);
		ImGui::SliderFloat("Multiplier", &m_multiplier, 0, 10);
		Vector3& direction = GetDirection();
		ImGui::LabelText("euler", "%.3f %.3f %.3f", direction.x, direction.y, direction.z);
		ImGui::ColorEdit3("Color", (float*)&m_color);
	}
};