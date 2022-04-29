#pragma once
#include "FShadowMap.h"
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include "wrl.h"
#include "d3dx12.h"
#include "windows.h"
#include "FUtil.h"
#include "FDXDevice.h"
#endif // 

namespace Charalotte
{
	class FHeapManager;
	class FDXShadowMap : public FShadowMap
	{
	public:
		FDXShadowMap(UINT width, UINT height, FDevice* Device, FHeapManager* HeapPtr);

		virtual ~FDXShadowMap();

		virtual FResource* GetResourcePtr()override
		{
			if (ResourcePtr != nullptr)
			{
				return ResourcePtr.get();
			}
			else
			{
				return nullptr;
			}
		}

#ifdef RENDER_PLATFORM_DX12
		void Init();

		void OnResize(UINT newWidth, UINT newHeight);

		void BuildShadowMapResource(FDevice* Device);

		D3D12_DEPTH_STENCIL_VIEW_DESC GetDsv() const
		{
			return mDsv;
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC GetSrv() const {
			return mSrv;
		}

		ID3D12Resource* GetResource();

		int GetSrvOffset(){ return SrvHeapOffset; }
		int GetDsvOffset(){ return DsvHeapOffset; }

		void DestoryScene()
		{
			HeapMgrPtr = nullptr;
		}

	protected:
		void CreateSRVAndDSV(ID3D12Device* device);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
		FDevice* Device;

		D3D12_DEPTH_STENCIL_VIEW_DESC mDsv;
		D3D12_SHADER_RESOURCE_VIEW_DESC mSrv;

		DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;

		UINT mWidth;
		UINT mHeight;

		int SrvHeapOffset = -1;
		int DsvHeapOffset = -1;
#endif
	private:
		std::shared_ptr<FResource> ResourcePtr;
		FHeapManager* HeapMgrPtr;
	};
}
