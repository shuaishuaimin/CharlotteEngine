#pragma once

namespace Charalotte
{
	class FResource;
	class FShadowMap
	{
	public:
		FShadowMap(){}
		virtual ~FShadowMap(){}

		virtual FResource* GetResourcePtr() = 0;

	};
}