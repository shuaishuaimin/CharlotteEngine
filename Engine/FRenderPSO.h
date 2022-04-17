#pragma once
//#include "SEShaderElements.h"
#include "FVerticesAndIndicesBuffer.h"
#include "FShader.h"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#endif

namespace Charalotte
{
	class FRenderPSO
	{
	public:
		FRenderPSO(FVerticesAndIndicesBuffer* VBIB, FShader* Shader) : VBIBBuffer(VBIB), ShaderPtr(Shader)
		{

		}

		FShader* GetShader()
		{
			return ShaderPtr;
		}

#ifdef RENDER_PLATFORM_DX12
		ID3D12PipelineState* GetPso()
		{
			return Pso.Get();
		}
#endif
	private:
		FVerticesAndIndicesBuffer* VBIBBuffer;
		FShader* ShaderPtr;
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12PipelineState> Pso;
#endif
	};
}
