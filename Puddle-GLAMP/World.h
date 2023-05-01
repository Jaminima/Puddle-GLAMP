#pragma once

#include "Cell.h"

Cell* world;

float* sim_w;
float* sim_c;
float* sim_cu;
float* sim_feq;

unsigned int world_x = 512, world_y = 512;
unsigned int world_cells = world_x * world_y;
float defaultValue = 0;

void InitWorld() {
	world = new Cell[world_cells];

	world[0].x = 1;
	world[0].y = 1;

	sim_w = new float[9] {4 / 9.0f, 1 / 9.0f, 1 / 9.0f, 1 / 9.0f, 1 / 9.0f, 1 / 36.0f, 1 / 36.0f, 1 / 36.0f, 1 / 36.0f};
	sim_c = new float[18] { 0, 0 , 1, 0, 0, 1, -1, 0, 0, -1, 1, 1, -1, 1, -1, -1, 1, -1};
	sim_cu = new float[18];
	sim_feq = new float[9* world_cells];
}