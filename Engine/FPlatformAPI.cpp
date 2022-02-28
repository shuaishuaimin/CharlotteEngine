#include "stdafx.h"
#include "FPlatformAPI.h"

int FPlatformAPI::InitResult(std::shared_ptr<FApp> App)
{
	if (!App->Initialize())
		return 0;

	return App->Run();
}

