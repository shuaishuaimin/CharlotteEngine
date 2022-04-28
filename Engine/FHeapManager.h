#pragma once
#include "DDefines.h"
#include "FEventRegisterSystem.h"
#include <set>

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
#ifdef RENDER_PLATFORM_DX12
	struct HeapSet
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandleStart;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandleStart;
		UINT DescriptorSize = 0;
		std::set<int> AvailableOffsetsBeforeEnd = {};
		int NowAvailableOffset = 0;
		int NowEndAvailableOffset = 0;
	};

	struct HeapAttributes
	{

	};
#endif

	class FHeapManager
	{
#ifdef RENDER_PLATFORM_DX12
	public:

		FHeapManager(ID3D12Device* Device, int RtvNum, int DsvNum, int USCVNum)
		{
			Heaps = {};
			CreateHeap(Device, USCVNum, HeapType::CBVSRVUAVHeap);
			CreateHeap(Device, DsvNum, HeapType::DSVHeap);
			CreateHeap(Device, RtvNum, HeapType::RTVHeap);
			SetSize(Device);
			NullHeap = nullptr;

			FEventRegisterSystem::GetInstance().RegisterFunctionWithInt("HeapOffsetRelease", [this](const std::vector<int>& vec) {
				// now we use heap params list, we someone use this function ,
				// we must give a int vector with 2 params ,the param[0] is heap type, the param[1] is offset value,
				if (vec.size() < 2)
				{
					return;
				}
				this->ReleaseOffset(HeapType(vec[0]), vec[1]);
				});
		}
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Heap(HeapType type)
		{
			
			if (Heaps.find(type) != Heaps.end())
			{
				return Heaps.find(type)->second->Heap;
			}
			else
			{
				return NullHeap;
			}
		}

		// this function will give the available offset to out
		int GetAvailableOffsetAndUseIt(HeapType Type)
		{
			const auto& Iter = Heaps.find(Type);
			int Result = -1;
			if (Iter == Heaps.end())
			{
				return Result;
			}
			int& AOffset = Iter->second->NowAvailableOffset;
			int& EOffest = Iter->second->NowEndAvailableOffset;
			Result = AOffset;
			auto& Sets = Iter->second->AvailableOffsetsBeforeEnd;
			if (!Sets.empty())
			{
				AOffset = *(Sets.begin());
				Result = AOffset;
				Sets.erase(*(Sets.begin()));
			}
			else
			{
				if (EOffest < (AOffset + 1))
				{
					AOffset ++;
					EOffest = AOffset;
				}
				else
				{
					AOffset = EOffest;
					Result = AOffset;
					AOffset ++;
					EOffest ++;
				}
			}
			return Result;
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleByTypeAndOffest(HeapType type, int Offset)
		{
			if (Offset <= 0)
			{
				return GetCPUHandle(type);
			}
			CD3DX12_CPU_DESCRIPTOR_HANDLE THandle = GetCPUHandle(type);
			THandle.Offset(Offset, GetSize(type));
			return THandle;
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleByTypeAndOffest(HeapType type, int Offset)
		{
			if (Offset <= 0)
			{
				return GetGPUHandle(type);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE  THandle = GetGPUHandle(type);
			THandle.Offset(Offset, GetSize(type));
			return THandle;
		}
		void ReleaseOffset(HeapType Type, int Offset)
		{
			const auto& Iter = Heaps.find(Type);
			if (Iter != Heaps.end())
			{
				Iter->second->AvailableOffsetsBeforeEnd.insert(Offset);
			}
		}
		
	protected:
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

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(HeapType type)
		{
			if (Heaps.find(type) != Heaps.end())
			{
				return Heaps.find(type)->second->CPUHandleStart;
			}
			else
			{
				return Heaps[HeapType::CBVSRVUAVHeap]-> CPUHandleStart;
			}
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(HeapType type)
		{
			if (Heaps.find(type) != Heaps.end())
			{
				return Heaps.find(type)->second->GPUHandleStart;
			}
			else
			{
				return Heaps[HeapType::CBVSRVUAVHeap]->GPUHandleStart;
			}
		}

		void CreateHeap(ID3D12Device* Device, int ViewNum, HeapType Type)
		{
			std::shared_ptr<HeapSet> TempHeap = std::make_shared<HeapSet>();
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dsvHeapDesc.NodeMask = 0;
			dsvHeapDesc.NumDescriptors = ViewNum;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE(Type);
			ThrowIfFailed(Device->CreateDescriptorHeap(
				&dsvHeapDesc, IID_PPV_ARGS(TempHeap->Heap.GetAddressOf())));
			TempHeap->CPUHandleStart = TempHeap->Heap->GetCPUDescriptorHandleForHeapStart();
			TempHeap->GPUHandleStart = TempHeap->Heap->GetGPUDescriptorHandleForHeapStart();
			TempHeap->DescriptorSize = GetSize(Type);
			Heaps.insert({Type, TempHeap});
		}

		
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> NullHeap;

		UINT mRtvDescriptorSize;
		UINT mDsvDescriptorSize;
		UINT mCbvSrvUavDescriptorSize; 

		std::unordered_map<HeapType, std::shared_ptr<HeapSet>> Heaps;
#endif
	};
}

