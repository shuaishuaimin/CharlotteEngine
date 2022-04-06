#pragma once
#include "DDefines.h"

#ifdef RENDER_PLATFORM_DX12
#include <windows.h>
#include <wrl.h>
#include "d3dx12.h"
#include "EHeapType.h"

namespace Charalotte
{
	class FHeapManager
	{
	public:
		FHeapManager()
		{

		}
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetHandle(HeapType type)
		{
			
		}
		void CreateHeap(HeapType type)
		{
			
		}
protected:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Heap(HeapType type)
	{
		switch (type)
		{
		case HeapType::DSVHeap:
			return DsvHeap;

		case HeapType::RTVHeap:
			return RtvHeap;

		case HeapType::UAVSRVCBVHeap:
			return UavCbvSrvHeap;

		default:
			return NullHeap;
		}
	}
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> UavCbvSrvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> NullHeap;
	};
}

#endif