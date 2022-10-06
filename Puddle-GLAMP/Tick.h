#pragma once
#include <amp.h>

#include "World.h"
#include "Lock.h"

unsigned int step = 0;

completion_future DoTick() {
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wz = world_z;

	const unsigned int _step = step;
	array_view<Cell, 3> _worldGrid(wx, wy, wz, world);
	array_view<Color, 2> _frame(w, h, Frame);
	array_view<Lock, 2> _frameLock(w, h, frameLock);

	parallel_for_each(
		_worldGrid.extent,
		[=](index<3> idx) restrict(amp) {
			index<2> frameIdx((idx[0]+idx[2] - _step / (float)wx) * w / 8, (idx[1]-idx[2]+_step / (float)wy) * h / 8);

			if (_frameLock[frameIdx].Grant()) {
				//_frame[frameIdx].SetColor(255, 255, 255);
				_frame[frameIdx] += Color(50, 50, 50);
			}
			else {
				int i = 255;
				while (!_frameLock[frameIdx].Grant()) {
					i++;
				}
				_frame[frameIdx].SetColor(255, 0, 0);
			}
			_frameLock[frameIdx].Free();
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

	return _frame.synchronize_async(access_type_read_write);
}