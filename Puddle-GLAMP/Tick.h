#pragma once
#include <amp.h>

#include "World.h"

unsigned int step = 0;

completion_future DoTick() {
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wz = world_z;

	const unsigned int _step = step;
	array_view<Cell, 3> _worldGrid(wx, wy, wz, world);
	array_view<Color, 2> _frame(w, h, Frame);

	/*parallel_for_each(
		_worldGrid.extent,
		[=](index<3> idx) restrict(amp) {
		}
	);*/

	parallel_for_each(
		_frame.extent,
		[=](index<2> idx) restrict(amp) {
			unsigned int r = (idx[0] + _step) % 255;
			unsigned int g = (idx[1] + _step) % 255;
			unsigned int b = (r * g / _step) % 255;

			_frame[idx] = Color(r, g, b);
		}
	);

	step++;

	return _frame.synchronize_async(access_type_read_write);
}