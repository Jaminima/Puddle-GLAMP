#pragma once
#include <iostream>
#include <amp.h>
#include <amp_math.h>
#include <iostream>
using namespace concurrency;
using namespace fast_math;

class Vec3
{
public:

	union
	{
		struct
		{
			float x, y, z;
		};

		float Data[3];
	};

	Vec3() restrict(amp, cpu)
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vec3(float x, float y, float z) restrict(amp, cpu)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3 operator-(Vec3 V)restrict(amp, cpu)
	{
		Vec3 N(x - V.x, y - V.y, z - V.z);
		return N;
	}

	Vec3 operator+(Vec3 V)restrict(amp, cpu)
	{
		Vec3 N(x + V.x, y + V.y, z + V.z);
		return N;
	}

	void operator+=(Vec3 V)restrict(amp, cpu)
	{
		x += V.x;
		y += V.y;
		z += V.z;
	}

	Vec3 operator*(int M)restrict(amp, cpu)
	{
		Vec3 N(x * M, y * M, z * M);

		return N;
	}

	Vec3 operator*(float M)restrict(amp, cpu)
	{
		Vec3 N(x * M, y * M, z * M);

		return N;
	}

	Vec3 operator/(Vec3 V)restrict(amp, cpu)
	{
		Vec3 N(x / V.x, y / V.y, z / V.z);
		return N;
	}

	Vec3 operator*(Vec3 V)restrict(amp, cpu)
	{
		Vec3 N(V.x * x, V.y * y, V.z * z);
		return N;
	}

	float dot(Vec3 V)restrict(amp, cpu)
	{
		return V.x * x + V.y * y + V.z * z;
	}

	Vec3 cross(Vec3 V)restrict(amp, cpu)
	{
		Vec3 N(
			y * V.z - z * V.y,
			z * V.x - x * V.z,
			x * V.y - y * V.x);
		return N;
	}

	void normalise()restrict(amp, cpu)
	{
		float n = sqrt(x * x + y * y + z * z);
		x /= n;
		y /= n;
		z /= n;
	}

	float norm2() restrict(amp, cpu) { return dot(*this); }
	float norm() restrict(amp, cpu) { return sqrt(norm2()); }
};
