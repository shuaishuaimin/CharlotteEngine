#pragma once
#include "stdafx.h"
#include "BaseStructAllPlatform.h"

namespace Charalotte
{

	enum WindowsAction
	{
		Move,
		MouseDown,
		MouseUp
	};
	struct WindowsInputData
	{
		WPARAM btnState;
		int x;
		int y;
		WindowsAction Action;
	};
}

