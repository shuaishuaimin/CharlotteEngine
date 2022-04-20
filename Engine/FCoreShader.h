#pragma once
#include "FShader.h"

namespace Charalotte
{
	class FShadowShader : public FShader
	{
	public:
		FShadowShader(const std::string& Path) : FShader(Path)
		{

		}
		virtual ~FShadowShader(){}
		virtual void SetRarameter(RHI* R) override
		{

		}
	};

	class FBassPassShader : public FShader
	{
	public:
		FBassPassShader(const std::string& Path) : FShader(Path)
		{

		}
		virtual void SetRarameter(RHI* R) override
		{

		}
	};
}
