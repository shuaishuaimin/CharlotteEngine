#pragma once
#include <memory>
#include "FDXVerticesAndIndicesBuffer.h"
#include "FMaterial.h"
#include "STransformDef.h"
#include "FRenderMesh.h"
#ifdef RENDER_PLATFORM_DX12
#include "DXPrimitives.h"
#include "FHeapManager.h"
#endif 


namespace Charalotte
{
	class FDXRenderMesh : public FRenderMesh
	{
	public:
		FDXRenderMesh(FHeapManager* Ptr)
		{
			VBIB = nullptr;
			Material = nullptr;
			HeapMgrPtr = Ptr;
			if (HeapMgrPtr != nullptr)
			{
				SrvHeapOffset = HeapMgrPtr->GetAvailableOffsetAndUseIt(HeapType::CBVSRVUAVHeap);
				CbvHeapOffset = HeapMgrPtr->GetAvailableOffsetAndUseIt(HeapType::CBVSRVUAVHeap);
			}
			else
			{
				SrvHeapOffset = -1;
				CbvHeapOffset = -1;
			}
		}

		virtual ~FDXRenderMesh(){
			if (HeapMgrPtr != nullptr)
			{
				HeapMgrPtr->ReleaseOffset(HeapType::CBVSRVUAVHeap, CbvHeapOffset);
				HeapMgrPtr->ReleaseOffset(HeapType::CBVSRVUAVHeap, SrvHeapOffset);
			}
		}

		inline void SetMaterial(FMaterial* Mat)
		{
			Material = Mat;
		}
		inline void SetBufferResource(FDXVerticesAndIndicesBuffer* Buffer)
		{
			VBIB = Buffer;
		}
		inline void SetTransformData(const FTransform& Trans)
		{
			TransData = Trans;
		}
		virtual FMaterial* GetMaterialPtr()
		{
			return Material;
		}

		inline void SetRenderMesh(const std::string& Name)
		{
			RenderMeshName = Name;
		}

		int GetSrvOffset()
		{
			return SrvHeapOffset;
		}

		int GetCbvOffset()
		{
			return CbvHeapOffset;   
		}
#ifdef RENDER_PLATFORM_DX12
		inline void SetBoundingBox(FDXBoundingBox Bd)
		{
			DrawArg = Bd;
		}

		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Srvh()
		{
			return SrvHeap;
		}

		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvH()
		{
			return CbvHeap;
		}

		inline std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>>& OCB()
		{
			return ObjectCB;
		}
#endif 
	private:
		FDXVerticesAndIndicesBuffer* VBIB;

		std::string RenderMeshName;
#ifdef RENDER_PLATFORM_DX12
		FDXBoundingBox DrawArg;
		std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>> ObjectCB = nullptr;
		int CbvHeapOffset;
		int SrvHeapOffset;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap = nullptr;
#endif
		FTransform TransData;
		FMaterial* Material; 
		FHeapManager* HeapMgrPtr;
	};
}
