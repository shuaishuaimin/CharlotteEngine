#pragma once
#include <memory>
#include "FRenderTarget.h"
#include "DDefines.h"
#include "FDevice.h"
#include "SResourceElement.h"
#ifdef RENDER_PLATFORM_DX12
#include "basetsd.h"
#include "d3dx12.h"
#include "wrl.h"
#include "FHeapManager.h"
#endif

namespace Charalotte
{
#ifdef RENDER_PLATFORM_DX12
	class FDXRenderTarget : public FRenderTarget
	{
	public:
		FDXRenderTarget(UINT width, UINT height, FDevice* Device, FResourceElement Element);
		virtual ~FDXRenderTarget(){}
		void Init(FHeapManager* HMgr);
		void SetRTBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> ryb)
		{
			mRtResource = ryb;
		}
		void SetDSBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> dsb)
		{
			mDsvResource = dsb;
		}
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetRTBuffer() { return mRtResource; }
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetDSBuffer() { return mDsvResource; }
		void OnResize(UINT newWidth, UINT newHeight);
		UINT getWight()
		{
			return mWidth;
		}
		UINT GetHeight()
		{
			return mHeight;
		}

		FResourceElement GetElement()
		{
			return ResourceElement;
		}
	private:
		ID3D12Resource* GetRtResource();

		ID3D12Resource* GetDsResource();
		
	protected:

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mRtResource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> mDsvResource = nullptr;
		FDevice* Device;

		UINT mWidth;
		UINT mHeight;

		FResourceElement ResourceElement;
	};
#endif
}
