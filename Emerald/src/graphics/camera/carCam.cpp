#include "stdafx.h"
#include "carCam.h"

CarCam::CarCam() : Camera(70, 0.1f, 1000.0f) {
	transform.m_rotation.x = 0.1f * Math::PI;
}