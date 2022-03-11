#include "stdafx.h"
#include "GameInstance.h"

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
		std::unique_ptr<GameInstance> GameIns = std::make_unique<GameInstance>();
		if (!GameIns->Init())
		{
			return 0;
		}
		int r = GameIns->Update();
		GameIns->Destory();
		return r;
	}
	catch (DxException& e)
	{
		{
			MessageBox(nullptr, e.ToString().c_str(), L"Init failed!", MB_OK);
			return 0;
		}
	}

}
