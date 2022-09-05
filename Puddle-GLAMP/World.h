#pragma once

#include "Cell.h"

Cell* world;

unsigned int world_x = 512, world_y = 512, world_z = 512;

void InitWorld() {
	world = new Cell[world_x * world_y * world_z];
}