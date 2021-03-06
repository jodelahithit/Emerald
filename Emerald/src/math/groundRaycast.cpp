#include "stdafx.h"

Vector3 GroundRaycast::Get(AssetRef<Camera> camera) {
	return CalculateMouseRay(camera);
}

Vector3 GroundRaycast::CalculateMouseRay(AssetRef<Camera> camera) {
	float mouseX = GetMouse()->GetPosition().x;
	float mouseY = GetMouse()->GetPosition().y;
	Vector2 normalizedCoords = GetNormalizedDeviceCoords(mouseX, mouseY);
	Vector4 clipCoords = Vector4(normalizedCoords.x, normalizedCoords.y, -1.0f, -1.0f);
	Vector4 eyeCoords = ToEyeCoords(clipCoords, camera);
	Vector3 worldRay = ToWorldCoords(eyeCoords, camera);
	return worldRay;
}
Vector2 GroundRaycast::GetNormalizedDeviceCoords(float mouseX, float mouseY) {
	float x = ((2.0f * mouseX) / GetApplication()->GetWindow()->GetWidth()) - 1.0f;
	float y = ((2.0f * mouseY) / GetApplication()->GetWindow()->GetHeight()) - 1.0f;
	return Vector2(x, -1.0f * y);
}
Vector4 GroundRaycast::ToEyeCoords(Vector4& clipCoords, AssetRef<Camera> camera) {
	Matrix4 invertedProjection = Matrix4::Invert(camera->GetProjectionMatrix());
	Vector4 eyeCoords = invertedProjection * clipCoords;
	return Vector4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}
Vector3 GroundRaycast::ToWorldCoords(Vector4& eyeCoords, AssetRef<Camera> camera) {
	Matrix4 invertedView = Matrix4::Invert(camera->GetViewMatrix());
	Vector4 rayWorld = invertedView * eyeCoords;
	Vector3 mouseRay = Vector3(rayWorld.x, rayWorld.y, rayWorld.z);
	return mouseRay.Normalize();
}
Vector3 GroundRaycast::GetGroundPosition(float height) {
	Vector3& ray = Get(GetCamera());
	Vector3 rayStart = GetCamera()->m_position;
	Vector3 rayEnd = GetCamera()->m_position + ray;
	Vector3 delta = rayEnd.Subtract(rayStart);
	delta.Normalize();

	Vector3 rayToPlaneDelta = Vector3(0.0f, height, 0.0f).Subtract(rayStart);

	float ratio = rayToPlaneDelta.Dot(Vector3::Up());
	Vector3 proj = Vector3::Up().Multiply(ratio);

	auto wp = rayToPlaneDelta.Dot(Vector3::Up());
	auto vp = delta.Dot(Vector3::Up());
	auto k = wp / vp;

	return delta.Multiply(k).Add(rayStart);
}

Vector2I GroundRaycast::GetTile() {
	Vector3 pos = GetGroundPosition();
	return { (int)pos.x, (int)pos.z };
}
