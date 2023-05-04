#pragma once
#include <amp.h>

#include "World.h"

unsigned int step = 0;

//Lattice Speed
#define NL 9

float *cxsy, *weights, *F;

void InitTick() {
	cxsy = new float[18]{ 0, 0, 1, 1, 1, 0,-1,-1,-1 , 0, 1, 1, 0,-1,-1,-1, 0, 1 };

	weights = new float[] {4 / 9, 1 / 9, 1 / 36, 1 / 9, 1 / 36, 1 / 9, 1 / 36, 1 / 9, 1 / 36};

	F = new float[world_x * world_y * NL]{ 1.0f };
}

bool inWorldRange(unsigned int idx, unsigned int world_cells) restrict(amp,cpu) {
	return idx < world_cells && idx >= 0;
}

completion_future* DoTick() {
	completion_future* fin = new completion_future[5];

	//NX, NY
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wc = world_cells;

	//X, Y
	array_view<Cell, 2> _XY(wx, wy, world);

	//View
	array_view<Color, 2> _frame(w, h, Frame);

	//Shared
	array_view<float, 1> _cxsy(18, cxsy);
	array_view<float, 1> _weights(9, weights);
	array_view<float, 3> _F(wx, wy, NL, F);
	array_view<float, 3> _NF(wx, wy, NL, F);

	array_view<float, 3> _Feq(wx, wy, NL);

	//Apply Drift
	parallel_for_each(_F.extent,
		[=](index<3> idx) restrict(amp) {
			float v = _F[idx];

			idx[0] += _cxsy[idx[2]];
			idx[1] += _cxsy[idx[2] + NL];

			_NF[idx] = v;
		}
	);

	fin[0] = _NF.synchronize_async(access_type_read_write);

	parallel_for_each(_XY.extent,
		[=](index<2> idx) restrict(amp) {
			float rho = 0;
			float ux = 0;
			float uy = 0;

			for (int i = 0; i < NL; i++) {
				index<3> _FIdx(idx[0], idx[1], i);

				rho += _NF[_FIdx];
				ux += _NF[_FIdx] * _cxsy[i];
				uy += _NF[_FIdx] * _cxsy[i + 8];
			}

			_XY[idx].rho = rho;
			_XY[idx].x = ux;
			_XY[idx].y = uy;
		}
	);

	fin[1] = _XY.synchronize_async(access_type_read_write);

	step++;

	fin[4] = _frame.synchronize_async(access_type_read_write);

	return fin;
}