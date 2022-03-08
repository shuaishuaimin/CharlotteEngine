#pragma once
#include "windows.h"

namespace Charalotte
{
	struct FVector
	{
	public:

		/** Vector's X component. */
		float x = 0.0f;

		/** Vector's Y component. */
		float y = 0.0f;

		/** Vector's Z component. */
		float z = 0.0f;

	};
	struct FVector4
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;
	};

	struct FQuaternion
	{
		float Y = 0.0f;
		float Z = 0.0f;
		float X = 0.0f;
		float W = 1.0f;
	};

	struct FTransform
	{
		Charalotte::FQuaternion Rotation;
		Charalotte::FVector Translation;
		Charalotte::FVector Scale3D;
	};

	struct FPoint2D
	{
		float x;
		float y;
	};
}
