#pragma once
#include "SEShaderElements.h"

//class SEResourceElement
//{
//
//};
namespace Charalotte
{
	struct FResourceElement
	{
		int DsDsvOffest = 0;
		int DsRtvOffest = 0;
		int RtDsvOffest = 0;
		int RtRtvOffest = 0;
	};
	enum class E_RESOURCE_TYPE
	{
		RESOURCE_DEP,
		RESOURCE_SHADER,
		RESOURCE_RT
	};

	struct FResourceAttributes
	{
		int RtO = 0;
		int SrO = 0;
		int DsO = 0;
		E_GRAPHIC_FORMAT Format;
		unsigned int Width = 0;
		unsigned int Height = 0;
		E_RESOURCE_TYPE ResourceType;
	};

	
}