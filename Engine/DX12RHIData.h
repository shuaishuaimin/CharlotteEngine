#pragma once
#include <windows.h>
#include <wrl.h>
#include "d3dx12.h"

namespace Charalotte
{
	struct PSO
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout = {};
	};
}