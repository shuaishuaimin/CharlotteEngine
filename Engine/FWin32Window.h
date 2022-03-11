#pragma once
#include "stdafx.h"
#include "FWindow.h"

class FWin32Window : public FWindow
{
public: 

	FWin32Window();
	FWin32Window(const FWin32Window& rhs) = delete;
	FWin32Window& operator=(const FWin32Window& rhs) = delete;
	virtual ~FWin32Window();

	HWND MainWnd()const;
	float AspectRatio()const;

	virtual bool InitMainWindow()override;

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static FWin32Window* GetMainWindow();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnKeyBoardInput();

	virtual void Update()override;
	virtual bool GetIsPaused() override;

	MSG GetMsg();

	virtual bool GetIsExit() override;
protected:
	static FWin32Window* MainWindow;

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

	bool IsRunning;

	bool IsPaused;

	MSG msg;

	bool IsExit;
};