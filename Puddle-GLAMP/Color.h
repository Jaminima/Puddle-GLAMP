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

	unsigned int GetA() restrict(amp, cpu) {
		return (RGBA & 0xFF000000) >> 24;
	}
	unsigned int GetB() restrict(amp, cpu) {
		return (RGBA & 0x00FF0000) >> 16;
	}
	unsigned int GetG() restrict(amp, cpu) {
		return (RGBA & 0x0000FF00) >> 8;
	}
	unsigned int GetR() restrict(amp, cpu) {
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
		unsigned int R = (unsigned int)ceilf(GetR() / V);
		unsigned int G = (unsigned int)ceilf(GetG() / V);
		unsigned int B = (unsigned int)ceilf(GetB() / V);

		SetColor(R, G, B);
	}

	void operator+=(Color V) restrict(amp, cpu)
	{
		unsigned int R = GetR() + V.GetR();
		unsigned int G = GetG() + V.GetG();
		unsigned int B = GetB() + V.GetB();

		SetColor(R, G, B);
	}

	Color(unsigned int R, unsigned int G, unsigned int B) restrict(amp, cpu) {
		//RGBA = ((R & 0xFF) << 24) + ((G & 0xFF) << 16) + ((B & 0xFF) << 8) + 0xFF;
		SetColor(R, G, B);
	}

	void SetColor(unsigned int R, unsigned int G, unsigned int B) restrict(amp, cpu) {
		RGBA = ((B & 0xFF) << 16) + ((G & 0xFF) << 8) + ((R & 0xFF) << 0) + 0xFF000000;
	}

	bool IsBlack() restrict(amp, cpu) {
		return GetR() == 0 && GetG() == 0 && GetB() == 0;
	}
};
