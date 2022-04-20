#pragma once
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#endif

namespace Charalotte
{
	class FRootForShader
	{
	public:
#ifdef RENDER_PLATFORM_DX12
		FRootForShader()
		{
			RootSignature = nullptr;
		}
		~FRootForShader()
		{
			RootSignature = nullptr;
		}
		Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignatureRef()
		{
			return RootSignature;
		}
#else
		FRootForShader() {}
#endif
	private:
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
#endif
	};

}
