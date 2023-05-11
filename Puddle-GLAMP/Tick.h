#pragma once
#include <amp.h>

#include "World.h"
#include <stdlib.h>

unsigned int step = 0;

//Lattice Speed
#define NL 9
#define tau 0.6f
#define rho0 100

#define scaledrndres 10000

float* cxsy, * weights, * F;

unsigned int F_Dim = world_x * world_y * NL;

float scaledrnd() {
	return rand() % scaledrndres / (float)scaledrndres;
}

void InitTick() {
	cxsy = new float[NL + NL] { 0, 0, 1, 1, 1, 0, -1, -1, -1, 0, 1, 1, 0, -1, -1, -1, 0, 1 };

	weights = new float[NL] {4 / 9.0f, 1 / 9.0f, 1 / 36.0f, 1 / 9.0f, 1 / 36.0f, 1 / 9.0f, 1 / 36.0f, 1 / 9.0f, 1 / 36.0f};

	F = new float[F_Dim];

	srand(time(NULL));

	//for (int i = 0; i < F_Dim; i++) {
	//	//F[i] = rand() / 10.0f;
	//	F[i] = 1 + (rand() % 100) / 1000.0f;

	//	if (i % NL < 3) {
	//		F[i] += 2 * (1+0.2 * cosf(2*3.14159*i));
	//	}
	//}

	for (int i = 0; i < world_x * world_y; i++) {
		float rho = 0;

		for (int j = 0; j < NL; j++) {
			float v = 1 + 0.01 * scaledrnd();

			if (j == 3) {
				//v += 2 * (1+0.2 * cosf(2*3.14159*i/world_x*4));
				v += 2 * (1 + 0.2 * cosf(i / world_x * 4));
			}

			rho += v;
			F[i * NL + j] = v;
		}

		for (int j = 0; j < NL; j++) {
			F[i * NL + j] *= rho0 / rho;
		}
	}
}

bool inWorldRange(unsigned int idx, unsigned int world_cells) restrict(amp, cpu) {
	return idx < world_cells&& idx >= 0;
}

void SetRectangleObstruction(index<2> tl, index<2> br, array_view<float, 3> _F) {
	parallel_for_each(extent<2>(br[0] - tl[0], br[1] - tl[1]),
		[=](index<2> idx) restrict(amp) {
			index<3> _FIdx(idx[0] + tl[0], idx[1] + tl[1], 0);

	_F[_FIdx] = 0;
	_F[_FIdx + 1] = 5;
	_F[_FIdx + 2] = 6;
	_F[_FIdx + 3] = 7;
	_F[_FIdx + 4] = 8;
	_F[_FIdx + 5] = 1;
	_F[_FIdx + 6] = 2;
	_F[_FIdx + 7] = 3;
	_F[_FIdx + 8] = 4;
		}
	);
}

void SetCircleObstruction(index<2> center, unsigned int r, array_view<float, 3> _F, array_view<Cell, 2> _XY) {
	//Not Quite A Circle
	index<2> tl(center[0], center[1]);
	unsigned int rr = r * r;
	parallel_for_each(
		extent<2>(r + r, r + r),
		[=](index<2> idx) restrict(amp) {
			idx -= r;
			if (idx[0] * idx[0] + idx[1] * idx[1] <= rr) {
				index<3> _FIdx(idx[0] + tl[0], idx[1] + tl[1], 0);
				_F[_FIdx] = 0;
				_F[_FIdx + 1] = 5;
				_F[_FIdx + 2] = 6;
				_F[_FIdx + 3] = 7;
				_F[_FIdx + 4] = 8;
				_F[_FIdx + 5] = 1;
				_F[_FIdx + 6] = 2;
				_F[_FIdx + 7] = 3;
				_F[_FIdx + 8] = 4;

				_XY[idx[0] + tl[0]][idx[1] + tl[1]].isSolid = true;
			}
		}
	);
}

#define futures_returned 3



inline bool isInRange(int i, int lim) restrict(amp) {
	return i < lim && i >= 0;
}

void DoTick() {
	//NX, NY
	const unsigned int wx = world_x;
	const unsigned int wy = world_y;
	const unsigned int wc = world_cells;

	//X, Y
	array_view<Cell, 2> _XY(wy, wx, world);

	//View
	array_view<Color, 2> _frame(w, h, Frame);

	//Shared
	array_view<float, 1> _cxsy(18, cxsy);
	array_view<float, 1> _W(9, weights);
	array_view<float, 3> _F(wx, wy, NL, F);
	array_view<float, 3> _NF(wx, wy, NL, F);

	//Temp
	array_view<float, 3> _Feq(wx, wy, NL);

	//Apply Drift
	parallel_for_each(_NF.extent,
		[=](index<3> idx) restrict(amp) {
			index<3> srcindex(idx[0] - _cxsy[idx[2]+NL], idx[1] - _cxsy[idx[2]], idx[2]);

			bool inRange = isInRange(srcindex[0], wy) && isInRange(srcindex[1],wx) && isInRange(srcindex[2], NL);

			if (inRange) {
				float v = _F[srcindex];
				_NF[idx] = v;
			}
			else {
				_NF[idx] = _F[idx];
			}
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
				uy += _NF[_FIdx] * _cxsy[i + NL];
			}

			_XY[idx].rho = rho;
			_XY[idx].x = ux / rho;
			_XY[idx].y = uy / rho;

			//float sum = ux + uy;
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

			float vb = (1 + 3 * cxuxcyuy + 9 * (cxuxcyuy * cxuxcyuy) / 2 - 3 * (u.x * u.x + u.y * u.y) / 2);

			v *= vb;

			_Feq[idx] = v;

			_NF[idx] += -(1.0f / tau) * (_NF[idx] - v);
		}
	);

	//SetRectangleObstruction(index<2>(wx / 2 - 20, wy / 2 - 20), index<2>(wx / 2 + 20, wy / 2 + 20), _NF);
	SetCircleObstruction(index<2>(wx / 2, wy / 2), 40, _NF, _XY);
	

	//Frame
	parallel_for_each(_frame.extent,
		[=](index<2>idx) restrict(amp) {
			Cell cl = _XY[idx - 1];
			Cell cr = _XY[idx + 1];
			Cell cu = _XY[idx + wx];
			Cell cd = _XY[idx - wx];
			Cell c = _XY[idx];

			Cell avg;

			avg.x = fabsf((cl.x + cr.x + cu.x + cd.x + c.x) / 5);
			avg.y = fabsf((cl.y + cr.y + cu.y + cd.y + c.y) / 5);
			avg.rho = fabsf((cl.rho + cr.rho + cu.rho + cd.rho + c.rho) / 5);

			//_frame[idx].SetColor(u.x + 1 * 127, 0/*u.rho * 255*/, u.y + 1 * 127);

			//Frame Colour Based On Difference Between Neighbours
			if (c.isSolid) {
				_frame[idx].SetColor(255, 255, 255);
			}
			else {
				//_frame[idx].SetColor(c.x * 255, c.rho * 255, c.y * 255);
				_frame[idx].SetColor(fabsf(cl.x - cr.x)/avg.x*127, 0, 0);
			}


			/*float f = _NF[idx[0]][idx[1]][0];

			_frame[idx].SetColor(f * 255, 0, 0);*/
		}
	);

	step++;

	_NF.synchronize(access_type_read_write);

	 _XY.synchronize(access_type_read_write);

	_frame.synchronize(access_type_read_write);
}