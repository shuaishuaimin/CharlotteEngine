#pragma once
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include <windows.h>
#include <wrl.h>
#include "d3dx12.h"
#endif

class FCommandList
{
public:
#ifdef RENDER_PLATFORM_DX12
	FCommandList(ID3D12GraphicsCommandList* Comptr)
	{
		CommandListPtr = Comptr;
	}
#else
	FCommandList()
	{

	}
#endif
	~FCommandList()
	{
#ifdef RENDER_PLATFORM_DX12
		CommandListPtr = nullptr;
#else
#endif
	}

#ifdef RENDER_PLATFORM_DX12
	ID3D12GraphicsCommandList* GetCommandListPtr()
	{
		return CommandListPtr;
	}
#else
#endif

private:
#ifdef RENDER_PLATFORM_DX12
	ID3D12GraphicsCommandList* CommandListPtr;
#else
#endif
};