#include "stdafx.h"
#include "FPlatformAPI.h"

int FPlatformAPI::InitResult(std::shared_ptr<FApp> App)
{
	App->CreateMainWindow();
	if (!App->Initialize())
		return 0;

	return App->Run();
}

