#pragma once
#include <amp.h>

#include "World.h"

unsigned int step = 0;

bool inWorldRange(unsigned int idx, unsigned int world_cells) restrict(amp,cpu) {
	return idx < world_cells && idx >= 0;
}

completion_future* DoTick() {
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wc = world_cells;

	array_view<Cell, 2> _u(wx, wy, world);
	array_view<Color, 2> _frame(w, h, Frame);

	const float omega = 1.0f;
	const float dx = 1.0;
	const float dt = 1.0;
	const float cs = dx / dt;

	array_view<float, 1> _w(9, sim_w);
	array_view<float, 1> _c(18, sim_c);
	array_view<float, 1> _cu(18, sim_cu);
	array_view<float, 1> _cu2(18, sim_cu);
	array_view<float, 3> _feq(9, wx, wy, sim_feq);

	

	step++;

	completion_future* fin = new completion_future[2];

	fin[0] = _frame.synchronize_async(access_type_read_write);
	fin[1] = _u.synchronize_async(access_type_read_write);

	return fin;
}