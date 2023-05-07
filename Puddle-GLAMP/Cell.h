#pragma once

class Cell {
public:
	float x = 0;
	float y = 0;
	float rho = 1;

	bool isSolid = false;

	Cell() restrict(amp, cpu) {
	}
};