#pragma once

class Cell {
public:
	float x = 0;
	float y = 0;
	float rho = 1;

	float sim_usq = 0;


	Cell() restrict(amp, cpu) {

	}
};