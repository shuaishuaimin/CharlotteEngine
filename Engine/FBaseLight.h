#pragma once
#include "FMathHelper.h"

namespace Charalotte
{
	class FBaseLight
	{
	public:
		FBaseLight(FLightForShading LightPass) : mLightPass(LightPass)
		{

		}
	private:
		FLightForShading mLightPass;
	};
}