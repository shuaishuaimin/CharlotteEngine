#include "stdafx.h"
#include "FPCRenderTarget.h"

#ifdef RENDER_PLATFORM_DX12

namespace Charalotte
{
	FPCRenderTarget::FPCRenderTarget()			 
	{
	}
	void FPCRenderTarget::Init(FHeapManager* HMgr)
	{
		
	}

	void FPCRenderTarget::OnResize(UINT newWidth, UINT newHeight)
	{
		if ((mWidth != newWidth) || (mHeight != newHeight))
		{
			mWidth = newWidth;
			mHeight = newHeight;
		}
	}

}
#endif 