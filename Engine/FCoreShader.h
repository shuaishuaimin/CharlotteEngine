#pragma once
#include "FShader.h"

namespace Charalotte
{
	class FShadowShader : public FShader
	{
	public:
		FShadowShader() {

		}
		virtual ~FShadowShader(){}
		virtual void SetRarameter(RHI* R) override
		{

		}
	};

	class FBassPassShader : public FShader
	{
	public:
		FBassPassShader() {

		}
		virtual void SetRarameter(RHI* R) override
		{

		}

}
