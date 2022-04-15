#pragma once
#include <memory>
#include "FVerticesAndIndicesBuffer.h"
#include "FMaterial.h"
#include "STransformDef.h"
#ifdef RENDER_PLATFORM_DX12
#include "DXPrimitives.h"
#endif 


namespace Charalotte
{
	class FRenderMesh
	{
	public:
		FRenderMesh()
		{
			VBIB = nullptr;
			Material = nullptr;
		}
		inline void SetMaterial(FMaterial* Mat)
		{
			Material = Mat;
		}
		inline void SetBufferResource(FVerticesAndIndicesBuffer* Buffer)
		{
			VBIB = Buffer;
		}
		inline void SetTransformData(const FTransform& Trans)
		{
			TransData = Trans;
		}
		FMaterial* GetMaterialPtr()
		{
			return Material;
		}
#ifdef RENDER_PLATFORM_DX12
		void SetBoundingBox(FDXBoundingBox Bd)
		{
			DrawArg = Bd;
		}
#endif 
	private:
		FVerticesAndIndicesBuffer* VBIB;
#ifdef RENDER_PLATFORM_DX12
		FDXBoundingBox DrawArg;
#endif
		FTransform TransData;
		FMaterial* Material; 
	};
}
