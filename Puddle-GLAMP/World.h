#pragma once

#include "Cell.h"

Cell* world;

unsigned int world_x = 512, world_y = 512, world_z = 1;
unsigned int world_cells = world_x * world_y * world_z;
float defaultValue = 0;

void InitWorld() {
	world = new Cell[world_cells];

	for (int i = 0; i < world_cells; i++) {
		world[i].f = defaultValue;
	}
}