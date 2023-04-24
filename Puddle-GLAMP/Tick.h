#pragma once
#include <amp.h>

#include "World.h"
#include "Lock.h"

unsigned int step = 0;

completion_future* DoTick() {
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wz = world_z;

	const unsigned int _step = step;
	array_view<Cell, 3> _worldGrid(wx, wy, wz, world);
	array_view<Color, 2> _frame(w, h, Frame);
	array_view<Lock, 2> _frameLock(w, h, frameLock);

	parallel_for_each(
		_frame.extent,
		[=](index<2> idx) restrict(amp) {
			index<3> widx(0,idx[0], idx[1]);
			Cell cc = _worldGrid[widx];

			Cell cu = _worldGrid[widx - wz];
			Cell cd = _worldGrid[widx + wz];
			Cell cl = _worldGrid[widx - 1];
			Cell cr = _worldGrid[widx + 1];

			float avg = ((cc.f * 4) + cu.f + cd.f + cl.f + cr.f)/8;

			Color col(avg * 255, avg * 255, avg * 255);
			//Color col(widx[0] * 255, widx[1] * 255, widx[2] * 255);

			//if (idx[0] == 255 && idx[1] == 255) {
			//	//col = Color(255, 255, 255);
			//}

			_frame[idx] = col;
		}
	);

	//parallel_for_each(
	//	_frame.extent,
	//	[=](index<2> idx) restrict(amp) {
	//		/*unsigned int r = (idx[0] + _step) % 255;
	//		unsigned int g = (idx[1] + _step) % 255;
	//		unsigned int b = (r * g / _step) % 255;*/

	//		//_frame[idx] = Color(r, g, b);
	//		_frame[idx] = Color(255, 255, 255);
	//	}
	//);

	step++;

	completion_future* fin = new completion_future[2];

	fin[0] = _frame.synchronize_async(access_type_read_write);
	fin[1] = _worldGrid.synchronize_async(access_type_read_write);

	return fin;
}