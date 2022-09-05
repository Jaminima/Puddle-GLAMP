#pragma once
#include "vec3.h"
#include "Color.h"
#include "Camera.h"
#include "Consts.h"

#include <amp.h>
#include <amp_math.h>
#include <iostream>
using namespace concurrency;
using namespace fast_math;

Camera* camera;
Color* Frame;

template <typename T> int sgn(T val) restrict(amp, cpu) {
	return (T(0) < val) - (val < T(0));
}

void Setup(Camera* _camera) {
	camera = _camera;

	camera->Position = Vec3(5, 5, 5);
	Frame = new Color[w * h];
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