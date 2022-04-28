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
#include "FEventRegisterSystem.h"
#endif

namespace Charalotte
{
using OffsetsType = std::unordered_map<HeapType, int>;
	class FDXResource : public FResource
	{
	public:
#ifdef RENDER_PLATFORM_DX12
		FDXResource(ID3D12Device* Device) : DevicePtr(Device), HeapMgrPtr(nullptr)
		{

		}
		virtual ~FDXResource();
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource()
		{
			return mResource;
		}
		void BuildResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr);
		
		OffsetsType& GetHeapOffsets()
		{
			return Offsets;
		}
	protected:
		void BuildRTResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr);
	
		void BuildDsResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr);
#else
		FDXResource() {}
		virtual ~FDXResource(){}
#endif
	private:
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
		FResourceAttributes ResourceAttributes;
		ID3D12Device* DevicePtr;
		FHeapManager* HeapMgrPtr;
		OffsetsType Offsets;
#endif
	};
}
