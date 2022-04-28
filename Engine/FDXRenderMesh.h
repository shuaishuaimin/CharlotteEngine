#pragma once
#include <memory>
#include "FDXVerticesAndIndicesBuffer.h"
#include "FMaterial.h"
#include "STransformDef.h"
#include "FRenderMesh.h"
#ifdef RENDER_PLATFORM_DX12
#include "DXPrimitives.h"
#endif 


namespace Charalotte
{
	class FDXRenderMesh : public FRenderMesh
	{
	public:
		FDXRenderMesh()
		{
			VBIB = nullptr;
			Material = nullptr;
		}

		virtual ~FDXRenderMesh(){}

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
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap = nullptr;
#endif
		FTransform TransData;
		FMaterial* Material; 
	};
}
