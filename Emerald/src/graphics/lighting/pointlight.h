#pragma once

class Pointlight {
public:

    Vector3 m_position;
    float m_radius;
    Color m_color;

    Pointlight() : m_position(Vector3(0, 0, 0)), m_radius(1), m_color(Color(1, 0, 0)) {}
    Pointlight(const Vector3& position, float radius, const Color& color) : m_position(position), m_radius(radius), m_color(color) {}
};