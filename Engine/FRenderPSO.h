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
		FRenderPSO(FVerticesAndIndicesBuffer* VBIB) : VBIBBuffer(VBIB)
		{
			
		}

		FShader* GetShader()
		{
			return ShaderPtr.get();
		}

		void CreateShader(std::shared_ptr<FShader>&& Shader)
		{
			ShaderPtr = Shader;
		}
#ifdef RENDER_PLATFORM_DX12
		ID3D12PipelineState* GetPso()
		{
			return Pso.Get();
		}
#endif
	private:
		FVerticesAndIndicesBuffer* VBIBBuffer;
		std::shared_ptr<FShader> ShaderPtr;
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12PipelineState> Pso;
#endif
	};
}
