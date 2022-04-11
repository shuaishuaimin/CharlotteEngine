#pragma once
#include "DDefines.h"

#ifdef RENDER_PLATFORM_DX12
#include <windows.h>
#include <wrl.h>
#include "d3dx12.h"
#include "FUtil.h"
#include "EHeapType.h"

namespace Charalotte
{
	class FHeapManager
	{
	public:
		FHeapManager(ID3D12Device* Device, int RtvNum, int DsvNum, int USCVNum)
		{			
			CreateHeap(Device, USCVNum,HeapType::CBVSRVUAVHeap);
			CreateHeap(Device, DsvNum, HeapType::DSVHeap);
			CreateHeap(Device, RtvNum, HeapType::RTVHeap);
			NullHeap = nullptr;
		}
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(HeapType type)
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(Heap(type)->GetCPUDescriptorHandleForHeapStart());
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(HeapType type)
		{
			return CD3DX12_GPU_DESCRIPTOR_HANDLE(Heap(type)->GetGPUDescriptorHandleForHeapStart());
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

			case HeapType::CBVSRVUAVHeap:
				return CbvSrvUavHeap;

			default:
				return NullHeap;
			}
		}

		void CreateHeap(ID3D12Device* Device, int ViewNum, HeapType Type)
		{
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dsvHeapDesc.NodeMask = 0;
			dsvHeapDesc.NumDescriptors = ViewNum;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE(Type);
			ThrowIfFailed(Device->CreateDescriptorHeap(
				&dsvHeapDesc, IID_PPV_ARGS(Heap(Type).GetAddressOf())));
		}
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> NullHeap;
	};
}

#endif