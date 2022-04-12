#pragma once
#include "FTexture.h"
namespace Charalotte
{
	struct FMaterialAttributes
	{
		float Roughness = 0.0f;
		FTexture* BaseColor = nullptr;
		FTexture* Normal = nullptr;
	};
}