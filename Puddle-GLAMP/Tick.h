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
	array_view<float, 3> _feq(9, wx, wy, sim_feq);

	//Calculate CU
	parallel_for_each(
		_cu.extent / 2,
		[=](index<1> idx) restrict(amp) {
			idx *= 2;

			for (int x = 0; x < wx; x++) {
				for (int y = 0; y < wy; y++) {
					Cell cc = _u[index<2>(x,y)];

					_cu[idx] = _c[idx] * cc.x;
					_cu[idx + 1] = _c[idx + 1] * cc.y;
				}
			}
		}
	);

	//cu.synchronize();

	//Calculate usq
	parallel_for_each(
		_u.extent,
		[=](index<2> idx) restrict(amp) {
			Cell cc = _u[idx];

			_u[idx].sim_usq = powf(cc.x, 2) + powf(cc.y, 2);
		}
	);

	//_u.synchronize();

	//Calculate feq
	parallel_for_each(
		_feq.extent,
		[=](index<3> idx) restrict(amp) {
			index<2> widx(idx[1], idx[2]);

	_feq[idx] = _u[widx].rho * _w[idx[0]] * (1.0f + (3.0f * _cu[idx[0]]) + (4.5f *))
		}
	);

		/*double feq[9][nx][ny];
			for (int i = 0; i < 9; i++) {
				double cu2 = cu[i] * cu[i];
				for (int j = 0; j < nx; j++) {
					for (int k = 0; k < ny; k++) {
						feq[i][j][k] = rho[j][k] * w[i] * (1.0 + 3.0 * cu[i] + 4.5 * cu2 - 1.5 * usq[j][k]);
					}
				}
			}*/

	step++;

	completion_future* fin = new completion_future[2];

	fin[0] = _frame.synchronize_async(access_type_read_write);
	fin[1] = _u.synchronize_async(access_type_read_write);

	return fin;
}