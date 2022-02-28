#include "stdafx.h"
#include "FWin32Window.h"

FWin32Window::FWin32Window(WNDPROC MainWndProcs, HINSTANCE Instance) : MainWndProcIns(MainWndProcs),
				mhAppInst(Instance)
{
	assert(Win32Window == nullptr);
	Win32Window = this;
}

FWin32Window::~FWin32Window()
{

}

HWND FWin32Window::MainWnd()const
{
	return mhMainWnd;
}
float FWin32Window::AspectRatio()const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

bool FWin32Window::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProcIns;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"Register failed", 0, 0);
		return false;
	}

	// compute window rectangle dismensions based on requested client area dismensions
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);

	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow failed.", 0, 0);
		return false;
	}

	// show window must do
	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}


FWin32Window* FWin32Window::GetWindow()
{
	return Win32Window;
}
