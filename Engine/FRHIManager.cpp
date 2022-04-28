#include "stdafx.h"
#include "FRHIManager.h"
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include "DX12RHI.h"
#endif

namespace Charalotte
{
	FRHIManager::FRHIManager()
	{
#ifdef RENDER_PLATFORM_DX12
		RHIIns = std::make_unique<DX12RHI>();
#endif
	}

	FRHIManager::~FRHIManager()
	{

	}

	RHI* FRHIManager::GetRHIPtr()
	{
		return RHIIns.get();
	}
}
