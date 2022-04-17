#pragma once
#include "FResource.h"
#include "DDefines.h"
#include "SEShaderElements.h"
#include "SEResourceElement.h"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#include "FUtil.h"
#include "FHeapManager.h"
#endif

namespace Charalotte
{
	class FDXResource : public FResource
	{
	public:
		FDXResource(ID3D12Device* Device) : DevicePtr(Device)
		{

		}
		virtual ~FDXResource() {}
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource()
		{
			return mResource;
		}
		void BuildResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr);
		
	protected:
		void BuildRTResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr);
	
		void BuildDsResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr);
	
#endif
	private:
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
		ID3D12Device* DevicePtr;
#endif
	};
}
