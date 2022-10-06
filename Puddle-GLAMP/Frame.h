#pragma once
#include "vec3.h"
#include "Color.h"
#include "Camera.h"
#include "Consts.h"
#include "Lock.h"

#include <iostream>

Camera* camera;
Color* Frame;
Lock* frameLock = new Lock[w * h];

template <typename T> int sgn(T val) restrict(amp, cpu) {
	return (T(0) < val) - (val < T(0));
}

void Setup(Camera* _camera) {
	camera = _camera;

	camera->Position = Vec3(5, 5, 5);
	Frame = new Color[w * h];

	for (int x = 0, y = 0; y < h;) {
		Frame[(y * w) + x] = Color(0, 0, 0);

		x++;
		if (x == w) {
			x = 0;
			y++;
		}
	}
}

struct Hit {
public:
	int axis = 0;
	float j = 0;

	Hit() restrict(amp, cpu) {}
	Hit(int _axis, float _j) restrict(amp, cpu) { axis = _axis; j = _j; }
};

float subtract_abs(float f1, float f2) restrict(amp, cpu) {
	return f1 > f2 ? f1 - f2 : f2 - f1;
}