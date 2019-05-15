#include "stdafx.h"

Vector3 GroundRaycast::Get(Camera& camera) {
	return CalculateMouseRay(camera);
}

Vector3 GroundRaycast::CalculateMouseRay(Camera& camera) {
	float mouseX = GetMouse()->GetPosition().x;
	float mouseY = GetMouse()->GetPosition().y;
	Vector2 normalizedCoords = GetNormalizedDeviceCoords(mouseX, mouseY);
	Vector4 clipCoords = Vector4(normalizedCoords.x, normalizedCoords.y, -1, -1);
	Vector4 eyeCoords = ToEyeCoords(clipCoords, camera);
	Vector3 worldRay = ToWorldCoords(eyeCoords, camera);
	return worldRay;
}

Vector2 GroundRaycast::GetNormalizedDeviceCoords(float mouseX, float mouseY) {
	float x = (2.0f * mouseX) / GetApplication()->GetWindow()->GetWidth() - 1;
	float y = (2.0f * mouseY) / GetApplication()->GetWindow()->GetHeight() - 1;
	return Vector2(x, y);
}

Vector4 GroundRaycast::ToEyeCoords(Vector4& clipCoords, Camera& camera) {
	Matrix4 invertedProjection = Matrix4::Invert(camera.GetProjectionMatrix());
	Vector4 eyeCoords = invertedProjection * clipCoords;
	return Vector4(eyeCoords.x, eyeCoords.y, -1, 0);
}

Vector3 GroundRaycast::ToWorldCoords(Vector4& eyeCoords, Camera& camera) {
	Matrix4 invertedView = Matrix4::Invert(camera.GetViewMatrix());
	Vector4 rayWorld = invertedView * eyeCoords;
	Vector3 mouseRay = Vector3(rayWorld.x, rayWorld.y, rayWorld.z);
	return mouseRay.Normalize();
}