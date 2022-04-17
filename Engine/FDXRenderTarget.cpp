#include "stdafx.h"
#include "FDXRenderTarget.h"

#ifdef RENDER_PLATFORM_DX12

namespace Charalotte
{
	FDXRenderTarget::FDXRenderTarget(
		UINT width, UINT height, FDevice* Device, FResourceElement Element) : mWidth(width), mHeight(height), Device(Device), ResourceElement(Element)
				 
	{
	}
	void FDXRenderTarget::Init(FHeapManager* HMgr)
	{
		FDXDevice* DxDevice = dynamic_cast<FDXDevice*>(Device);
		auto DevicePtr = DxDevice->GetDevice();
	}

	void FDXRenderTarget::OnResize(UINT newWidth, UINT newHeight)
	{
		if ((mWidth != newWidth) || (mHeight != newHeight))
		{
			mWidth = newWidth;
			mHeight = newHeight;
		}
	}

	ID3D12Resource* FDXRenderTarget::GetRtResource()
	{
		return mRtResource.Get();
	}

	ID3D12Resource* FDXRenderTarget::GetDsResource()
	{
		return mDsvResource.Get();
	}
}
#endif 