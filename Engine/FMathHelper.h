#pragma once
#include <Windows.h>
#include <cstdint>
#include "BaseStructAllPlatform.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"

class FMathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	static int Rand(int a, int b)
	{
		return a + rand() % ((b - a) + 1);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);


	static glm::mat4 Identity4x4()
	{
		static glm::mat4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}
	static glm::vec4 Vec4MultipyMat(const glm::vec4& vector, const glm::mat4& Matrix);

	static glm::mat4 GetWorldTransMatrix(const Charalotte::FTransform& Transform);

	static glm::mat4 GetRotateMatrix(float Pitch, 
					float Yaw, float Roll, const glm::vec4& Target, const glm::vec4& Up, const glm::vec4& Location);

	static glm::mat4 GetRotateMatrix(const glm::vec4& Qua);

	static const float Infinity;
	static const float Pi;


};

