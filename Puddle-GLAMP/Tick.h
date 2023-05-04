#pragma once
#include <amp.h>

#include "World.h"
#include <stdlib.h> 

unsigned int step = 0;

//Lattice Speed
#define NL 9
#define tau 0.6

float *cxsy, *weights, *F;

unsigned int F_Dim = world_x * world_y * NL;

void InitTick() {
	cxsy = new float[18]{ 0, 0, 1, 1, 1, 0,-1,-1,-1 , 0, 1, 1, 0,-1,-1,-1, 0, 1 };

	weights = new float[] {4 / 9, 1 / 9, 1 / 36, 1 / 9, 1 / 36, 1 / 9, 1 / 36, 1 / 9, 1 / 36};

	F = new float[F_Dim];

	srand(time(NULL));

	for (int i = 0; i < F_Dim; i++) {
		//F[i] = rand() / 10.0f;
		F[i] = 2.0f + (0.01f * (rand() / 100.0f));

		if (i % 9 < 3) {
			F[i] += 2;
		}
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

	//Apply Drift
	parallel_for_each(_F.extent,
		[=](index<3> idx) restrict(amp) {
			float v = _F[idx];

			index<3> nidx(idx[0] + _cxsy[idx[2]], idx[1] + _cxsy[idx[2]+ NL], idx[2]);

			_NF[nidx] = v;
		}
	);


	//Fluid Variables
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

			float sum = ux + uy;
		}
	);

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

			_NF[idx] += -(1.0f / tau) * (_NF[idx] - v);
		}
	);

	//Frame
	parallel_for_each(_frame.extent,
		[=](index<2>idx) restrict(amp) {
			Cell u = _XY[idx];

			_frame[idx].SetColor(u.x * 255, 0, u.y * 255);
		}
	);

	step++;

	fin[0] = _NF.synchronize_async(access_type_read_write);

	fin[1] = _XY.synchronize_async(access_type_read_write);

	fin[2] = _frame.synchronize_async(access_type_read_write);

	return fin;
}