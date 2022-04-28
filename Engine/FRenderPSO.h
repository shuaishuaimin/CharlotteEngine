#pragma once
//#include "SEShaderElements.h"
#include "FDXVerticesAndIndicesBuffer.h"
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
		FRenderPSO()
		{
#ifdef RENDER_PLATFORM_DX12
			Pso = nullptr;
			ShaderPtr = nullptr;
#endif
		}
		FRenderPSO(FDXVerticesAndIndicesBuffer* VBIB) : VBIBBuffer(VBIB)
		{
#ifdef RENDER_PLATFORM_DX12
			Pso = nullptr;
#endif
		}

		void SetShader(FShader* Shader)
		{
			ShaderPtr = Shader;
		}

		FShader* GetShader()
		{
			return ShaderPtr;
		}
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPsoRef()
		{
			return Pso;
		}
#endif
	private:
		FDXVerticesAndIndicesBuffer* VBIBBuffer;
		FShader* ShaderPtr;
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12PipelineState> Pso;
#endif
	};
}
