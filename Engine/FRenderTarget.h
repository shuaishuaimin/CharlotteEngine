#pragma once
#include "DDefines.h"
#include "EHeapType.h"
#ifdef RENDER_PLATFORM_DX12


#endif

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
		~FRenderTarget() {}
		
	private:
		int Offest;
		HeapType Type;
	};
}
