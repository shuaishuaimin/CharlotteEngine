#pragma once
#include "DDefines.h"

#ifdef RENDER_PLATFORM_DX12
#include <windows.h>
#include <wrl.h>
#include <minwindef.h>
#include "d3dx12.h"
#include "FUtil.h"
#include "EHeapType.h"
#endif
namespace Charalotte
{
	class FHeapManager
	{
	public:
#ifdef RENDER_PLATFORM_DX12
		FHeapManager(ID3D12Device* Device, int RtvNum, int DsvNum, int USCVNum)
		{			
			CreateHeap(Device, USCVNum,HeapType::CBVSRVUAVHeap);
			CreateHeap(Device, DsvNum, HeapType::DSVHeap);
			CreateHeap(Device, RtvNum, HeapType::RTVHeap);
			SetSize(Device);
			CPURtvHandle = RtvHeap->GetCPUDescriptorHandleForHeapStart();
			CPUDsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
			CPUCbvSrvUavHandle = CbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();

			GPURtvHandle = RtvHeap->GetGPUDescriptorHandleForHeapStart();
			GPUDsvHandle = DsvHeap->GetGPUDescriptorHandleForHeapStart();
			GPUCbvSrvUavHandle = CbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
			NullHeap = nullptr;
		}
		
		

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
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleByTypeAndOffest(HeapType type, int Offset)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE THandle = GetCPUHandle(type);
			THandle.Offset(Offset, GetSize(type));
			return THandle;
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleByTypeAndOffest(HeapType type, int Offset)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE  THandle = GetGPUHandle(type);
			THandle.Offset(Offset, GetSize(type));
			return THandle;
		}

		void SetSize(ID3D12Device* Device)
		{
			mRtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			mDsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			mCbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		UINT GetSize(HeapType Type)
		{
			switch (Type)
			{
			case HeapType::DSVHeap:
				return mDsvDescriptorSize;

			case HeapType::RTVHeap:
				return mRtvDescriptorSize;

			case HeapType::CBVSRVUAVHeap:
				return mCbvSrvUavDescriptorSize;

			default:
				return UINT(0);
			}
		}
	protected:
		CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle(HeapType type)
		{
			switch (type)
			{
			case HeapType::DSVHeap:
				return CPUDsvHandle;

			case HeapType::RTVHeap:
				return CPURtvHandle;

			case HeapType::CBVSRVUAVHeap:
				return CPUCbvSrvUavHandle;

			default:
				return CPUDsvHandle;
			}
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(HeapType type)
		{
			switch (type)
			{
			case HeapType::DSVHeap:
				return GPUDsvHandle;

			case HeapType::RTVHeap:
				return GPURtvHandle;

			case HeapType::CBVSRVUAVHeap:
				return GPUCbvSrvUavHandle;

			default:
				return GPUDsvHandle;
			}
		}
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvSrvUavHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> NullHeap;

		UINT mRtvDescriptorSize;
		UINT mDsvDescriptorSize;
		UINT mCbvSrvUavDescriptorSize; 

		CD3DX12_CPU_DESCRIPTOR_HANDLE CPURtvHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDsvHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUCbvSrvUavHandle;

		CD3DX12_GPU_DESCRIPTOR_HANDLE GPURtvHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDsvHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUCbvSrvUavHandle;
#endif
	};
}

