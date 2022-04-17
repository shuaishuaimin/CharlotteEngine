#pragma once
#include "FResource.h"
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#endif

class FDXResource : public FResource
{
public:
	FDXResource(){}
	virtual ~FDXResource(){}
#ifdef RENDER_PLATFORM_DX12
	
#endif
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mRtResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDsResource = nullptr;
};