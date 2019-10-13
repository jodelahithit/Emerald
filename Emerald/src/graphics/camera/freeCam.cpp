#include "stdafx.h"

void FreeCam::Update(const TimeStep& time) {
	float speed = movementSpeed * time.GetSeconds();
	if (KeyDown(LSHIFT)) speed *= 10;
	if (KeyDown(LALT)) speed /= 10;

	if (KeyDown('W')) Utils::setPositionInFrontOfCam(m_position, this, speed);
	if (KeyDown('S')) Utils::setPositionInFrontOfCam(m_position, this, -speed);
	if (KeyDown('D')) m_position += Vector3(Math::Cos(m_rotation.yaw), 0, Math::Sin(m_rotation.yaw))  * speed;
	if (KeyDown('A')) m_position -= Vector3(Math::Cos(m_rotation.yaw), 0, Math::Sin(m_rotation.yaw))  * speed;

	if (KeyDown('I')) m_position.z += speed;
	if (KeyDown('K')) m_position.z -= speed;
	if (KeyDown('J')) m_position.x += speed;
	if (KeyDown('L')) m_position.x -= speed;
	if (KeyDown('Q') || KeyDown(' ')) m_position.y += speed;
	if (KeyDown('E') || KeyDown('C') /*|| KeyDown(LCTRL)*/) m_position.y -= speed;

	if (ButtonDown(VK_MOUSE_RIGHT)) {
		m_rotation.y -= GetMouse()->GetDelta().x * 0.005f;
		m_rotation.x = Math::Clamp(m_rotation.x - GetMouse()->GetDelta().y * 0.005f, -Math::HALF_PI, Math::HALF_PI);
	}

	UpdateViewMatrix();
}