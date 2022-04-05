#pragma once
#include <memory>
#include "SEShaderElements.h"
#include "RHI.h"

class FShader
{
public:
	virtual void SetRarameter(RHI* R) = 0;
private:
	std::unique_ptr<Charalotte::FShaderInfo> ShaderInfo;
};

class FShadowShader : public FShader
{
public:
	virtual void SetRarameter(RHI* R) override
	{
		
	}
};

class FBassPassShader : public FShader
{
public:
	virtual void SetRarameter(RHI* R) override
	{

	}
};

