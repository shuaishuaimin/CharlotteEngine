#include "stdafx.h"
#include "DXRender.h"
#include "FGame.h"

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
		std::shared_ptr<FApp> theApp = std::make_shared<DXRender>(hInstance);
		std::shared_ptr<FGame> theGame = std::make_shared<FGame>();
		theGame->Init();
		theGame->Run();

		if (theApp->CreateMainWindow() == nullptr)
		{
			return 0;
		}
		if (!theApp->Initialize())
			return 0;

		return theApp->Run();

	}
	catch (DxException& e)
	{
		{
			MessageBox(nullptr, e.ToString().c_str(), L"Init failed!", MB_OK);
			return 0;
		}
	}

}