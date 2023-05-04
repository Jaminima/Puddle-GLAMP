#pragma once
#include <Windows.h>
#include "Frame.h"
#include "Inputs.h"
#include "Tick.h"

#include "GL/glut.h"
#include "GL/freeglut.h"

unsigned int framesInSec = 0;
time_t fpsTime = clock() + 1000;
time_t tickTime = clock() + 1000;

Camera* _camera = new Camera();

void drawFrame()
{
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, Frame);
	glutSwapBuffers();
}

completion_future* simStep;

void triggerReDraw()
{
	//frame = RenderFrame();
	simStep = DoTick();

	framesInSec++;

	if (clock() - fpsTime >= 1000)
	{
		printf_s("You averaged %d fps\n", framesInSec);
		framesInSec = 0;
		fpsTime = clock();
	}

	for (int i = 0; i < futures_returned; i++) {
		simStep[i].wait();
	}

	delete[] simStep;

	glutPostRedisplay();
}

void SetupFrame(int argc, char** argv)
{
	_input_cam_ref = _camera;
	Setup(_camera);

	glutInit(&argc, argv);
	glutInitWindowSize(w, h);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Puddle GLAMP");

	glutDisplayFunc(drawFrame);
	glutIdleFunc(triggerReDraw);

	glutPassiveMotionFunc(MouseMove);
	glutKeyboardFunc(KeyboardDepressed);

	glutMainLoop();
}
