#pragma once
#include "stdafx.h"
#include "FWindow.h"

class FWin32Window : public FWindow
{
public: 

	FWin32Window(WNDPROC MainWndProcs, HINSTANCE Instance);
	FWin32Window(const FWin32Window& rhs) = delete;
	FWin32Window& operator=(const FWin32Window& rhs) = delete;
	virtual ~FWin32Window();

	HWND MainWnd()const;
	float AspectRatio()const;

	virtual bool InitMainWindow()override;

protected:
	WNDPROC MainWndProcIns;
	HINSTANCE mhAppInst = nullptr; // application instance handle
	HWND	  mhMainWnd = nullptr; // main window handle
	bool	  mMinimized = false; // is the application min?
	bool	  mMaximized = false; // is the application max?
	bool	  mResizing = false; // are the resize bars being dragged?
	bool	  mFullscreenState = false; // full screen enabled

	std::wstring mMainWndCaption = L"Charlotte Engine"; // name
	int mClientWidth = 1980;
	int mClientHeight = 1280;
};