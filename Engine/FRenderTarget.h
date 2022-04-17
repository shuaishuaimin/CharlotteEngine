#pragma once
#include "EHeapType.h"

namespace Charalotte
{
	class FRenderTarget
	{
	public:
		FRenderTarget() : Offest(0), Type(HeapType::CBVSRVUAVHeap)
		{

		}
		FRenderTarget(int Off, HeapType TP) : Offest(Off), Type(TP)
		{}

		virtual ~FRenderTarget() {}
		
	private:
		int Offest;
		HeapType Type;
	};
}
