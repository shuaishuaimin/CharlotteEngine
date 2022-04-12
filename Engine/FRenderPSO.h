#pragma once
#include "SEShaderElements.h"
#include "FVerticesAndIndicesBuffer.h"
#include "FShader.h"

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

	private:
		FVerticesAndIndicesBuffer* VBIBBuffer;
		FShader* ShaderPtr;
	};
}
