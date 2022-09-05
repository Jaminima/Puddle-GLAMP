#pragma once
#include "Camera.h"
#include "GL/glut.h"
#include "GL/freeglut.h"
#include "Consts.h"

Camera* _input_cam_ref;

void KeyboardDepressed(unsigned char key, int x, int y) {
	Vec3 camMove(0, 0, 0);

	if (key == 'w') camMove.z += camMoveStep;
	if (key == 's') camMove.z -= camMoveStep;

	if (key == 'a') camMove.x -= camMoveStep;
	if (key == 'd') camMove.x += camMoveStep;

	if (key == 'p') { *doGameTicks = !*doGameTicks; }

	if (key == 'r') { _automota->InitGrid(); }

	if (key == 'q') exit(0);

	_input_cam_ref->MoveCamera(camMove);
}

void MouseMove(int x, int y) {
	int movex = w_half - x, movey = h_half - y;

	_input_cam_ref->RotateCamera(Vec3(movey * camSensitvity, -movex * camSensitvity, 0));

	glutWarpPointer(w_half, h_half);
}
