#pragma once

#include "Cell.h"

Cell* world;

unsigned int world_x = 512, world_y = 512;
unsigned int world_cells = world_x * world_y;
float defaultValue = 0;

void InitWorld() {
	world = new Cell[world_cells];

	world[0].x = 1;
	world[0].y = 1;
}