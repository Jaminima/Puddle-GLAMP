#pragma once

#include "World.h"

completion_future DoTick() {
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wz = world_z;

	array_view<Cell, 3> _worldGrid(wx, wy, wz, world);
	array_view<Color, 2> _frame(w, h, Frame);

	parallel_for_each(
		_worldGrid.extent,
		[=](index<3> idx) restrict(amp) {
		}
	);

	return _frame.synchronize_async(access_type_read_write);
}