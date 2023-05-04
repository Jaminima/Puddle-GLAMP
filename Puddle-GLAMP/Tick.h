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

	int dim = world_x * world_y * NL;
	F = new float[dim];

	for (int i = 0; i < dim; i++) {
		F[i] = 1.0f;
	}
}

bool inWorldRange(unsigned int idx, unsigned int world_cells) restrict(amp,cpu) {
	return idx < world_cells && idx >= 0;
}

#define futures_returned 3

completion_future* DoTick() {
	completion_future* fin = new completion_future[futures_returned];

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
	array_view<float, 1> _W(9, weights);
	array_view<float, 3> _F(wx, wy, NL, F);
	array_view<float, 3> _NF(wx, wy, NL, F);

	array_view<float, 3> _Feq(wx, wy, NL);

	for (int i = 0; i < NL * 2;i++) {
		_NF[index<3>(256,256,i)] += 0.8;
	}

	//Apply Drift
	parallel_for_each(_F.extent,
		[=](index<3> idx) restrict(amp) {
			float v = _F[idx];

			index<3> nidx(idx[0] + _cxsy[idx[2]], idx[1] + _cxsy[idx[2]+ NL], idx[2]);

			_NF[nidx] = v;
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

			_frame[idx].SetColor(ux * 255, uy*255,(ux+uy)*127);
		}
	);

	fin[1] = _XY.synchronize_async(access_type_read_write);

	//Apply Collision
	parallel_for_each(_Feq.extent,
		[=](index<3> idx)restrict(amp) {
			index<2> widx(idx[0], idx[1]);
			Cell u = _XY[widx];
			
			float v = u.rho * _W[idx[2]];
			float cx = _cxsy[idx[2]];
			float cy = _cxsy[idx[2] + NL];

			float cxuxcyuy = cx * u.x + cy * u.y;

			float vb = (1 + 3 * cxuxcyuy + 9 * cxuxcyuy * cxuxcyuy / 2 - 3 * (u.x * u.x + u.y * u.y) / 2);

			v *= vb;

			_Feq[idx] = v;
		}
	);

	step++;

	fin[2] = _frame.synchronize_async(access_type_read_write);

	return fin;
}