#pragma once
#include "FTexture.h"
#include "SEShaderElements.h"
namespace Charalotte
{
	struct FMaterialAttributes
	{
		float Roughness = 0.0f;
		FTexture* BaseColor = nullptr;
		FTexture* Normal = nullptr;
	};

	struct FPSOAttributes
	{
		bool IsUseBias = false;
		float DepthBias = 0.0f;
		float DepthBiasClamp = 0.0f;
		float SlopeScaledDepthBias = 0.0f;
		int NumRenderTargets = 1;
		E_GRAPHIC_FORMAT RTVFormat = E_GRAPHIC_FORMAT::FORMAT_UNKNOWN;
		E_GRAPHIC_FORMAT DSFormat = E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT;
	};
}