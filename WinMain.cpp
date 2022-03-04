#include "stdafx.h"
#include "FEngineCore.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.

// ??
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		std::shared_ptr<FApp> theApp = std::make_shared<FEngineCore>(hInstance);
		
		return FPlatformAPI::InitResult(theApp);
	}
	catch (DxException& e)
	{
		{
			MessageBox(nullptr, e.ToString().c_str(), L"Init failed!", MB_OK);
			return 0;
		}
	}

}