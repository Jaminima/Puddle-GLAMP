#pragma once
#include <amp.h>
#include <amp_math.h>
#include <iostream>
using namespace concurrency;
using namespace fast_math;

class Color
{
public:
	unsigned int RGBA;

	unsigned int GetR() restrict(amp, cpu) {
		return (RGBA & 0xFF000000) >> 6;
	}
	unsigned int GetG() restrict(amp, cpu) {
		return (RGBA & 0x00FF0000) >> 4;
	}
	unsigned int GetB() restrict(amp, cpu) {
		return (RGBA & 0x0000FF00) >> 2;
	}
	unsigned int GetA() restrict(amp, cpu) {
		return (RGBA & 0x000000FF);
	}

	Color() restrict(amp, cpu) {
		RGBA = 0xFFFFFFFF;
	};

	Color operator+(Color V)restrict(amp, cpu)
	{
		Color N(GetR() + V.GetR(), GetG() + V.GetG(), GetB() + V.GetB());
		return N;
	}

	void operator/=(float V) restrict(amp, cpu)
	{
		unsigned int R = (unsigned int)ceilf(R / V);
		unsigned int G = (unsigned int)ceilf(G / V);
		unsigned int B = (unsigned int)ceilf(B / V);

		RGBA = ((R & 0xFF) << 6) + ((G & 0xFF) << 4) + ((B & 0xFF) << 2) + 0xFF;
	}

	void operator+=(Color V) restrict(amp, cpu)
	{
		unsigned int R = GetR() + V.GetR();
		unsigned int G = GetG() + V.GetG();
		unsigned int B = GetB() + V.GetB();

		RGBA = ((R & 0xFF) << 6) + ((G & 0xFF) << 4) + ((B & 0xFF) << 2) + 0xFF;
	}

	Color(unsigned int R, unsigned int G, unsigned int B) restrict(amp, cpu) {
		RGBA = ((R & 0xFF) << 6) + ((G & 0xFF) << 4) + ((B & 0xFF) << 2) + 0xFF;
	}

	bool IsBlack() restrict(amp, cpu) {
		return GetR() == 0 && GetG() == 0 && GetB() == 0;
	}
};
