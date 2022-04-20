#pragma once
#include "DDefines.h"
#include "vector"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#include "FRootForShader.h"

namespace Charalotte
{
	struct FDXShaderElement
	{
		std::shared_ptr<D3D_SHADER_MACRO> VSShaderMacroSharedPtr = nullptr;
		std::shared_ptr<D3D_SHADER_MACRO> PSShaderMacroSharedPtr = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
		std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout = {};
		std::shared_ptr<FRootForShader> RootSig = nullptr;
	};
}

#endif