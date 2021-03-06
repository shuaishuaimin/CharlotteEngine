#include "stdafx.h"
#include <thread>
#include <WindowsX.h>
#include <locale>
#include <codecvt>
#include "FWin32Window.h"
#include "FScene.h"
#include "FWinEventRegisterSystem.h"
#include "CharlotteEngine.h"

namespace Charalotte
{
	LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// forward hwnd on because we can get message (e.g: WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return FWin32Window::GetMainWindow()->MsgProc(hwnd, msg, wParam, lParam);
	}

	FWin32Window::FWin32Window() :
		mhAppInst(GetModuleHandle(0)), IsPaused(false), IsExit(false)
	{
		assert(MainWindow == nullptr);
		MainWindow = this;
		msg = { 0 };
		WinText = mMainWndCaption;
	}
	FWin32Window* FWin32Window::MainWindow = nullptr;

	FWin32Window::~FWin32Window()
	{
		IsRunning = false;
	}

	FWin32Window* FWin32Window::GetMainWindow()
	{
		return MainWindow;
	}

	void FWin32Window::Update()
	{
		if (msg.message != WM_QUIT)
		{
			// if there are Window messages then process them
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// otherwise, do animation/game stuff
			else
			{
				CharalotteEngine::GetInstance().GetTimer()->Tick();

				// if game pause sleep for wait
				// else calculate frame states and update timer, draw timer to screen
				if (!IsPaused)
				{
					CalculateFrameStats();
					OnKeyBoardInput();
				}
				else
				{
					Sleep(100);
				}
			}
		}
		else
		{
			IsExit = true;
		}

	}
	HWND FWin32Window::MainWnd()const
	{
		return mhMainWnd;
	}
	float FWin32Window::AspectRatio()const
	{
		return static_cast<float>(mClientWidth) / mClientHeight;
	}

	MSG FWin32Window::GetMsg()
	{
		return msg;
	}

	bool FWin32Window::GetIsExit()
	{
		return IsExit;
	}

	void FWin32Window::SetCameraSen(float SenNum)
	{
		CameraSen = SenNum;
	}

	bool FWin32Window::InitMainWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
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

	LRESULT FWin32Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// WM_ACTIVATE is sent when the window is activated or deactivated.  
			// We pause the game when the window is deactivated and unpause it 
			// when it becomes active.  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				IsPaused = true;
				CharalotteEngine::GetInstance().GetTimer()->Stop();
			}
			else
			{
				IsPaused = false;
				CharalotteEngine::GetInstance().GetTimer()->Start();
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			// Save the new client area dimensions.
			mClientWidth = LOWORD(lParam);
			mClientHeight = HIWORD(lParam);
			if (CharalotteEngine::GetInstance().GetRenderPtr()->GetIsDevicedSucceed())
			{
				if (wParam == SIZE_MINIMIZED)
				{
					IsPaused = true;
					mMinimized = true;
					mMaximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					IsPaused = false;
					mMinimized = false;
					mMaximized = true;
					FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (mMinimized)
					{
						IsPaused = false;
						mMinimized = false;
						FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);
					}

					// Restoring from maximized state?
					else if (mMaximized)
					{
						IsPaused = false;
						mMaximized = false;
						FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);
					}
					else if (mResizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					{
						FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);
					}
				}
			}
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			IsPaused = true;
			mResizing = true;
			CharalotteEngine::GetInstance().GetTimer()->Stop();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			IsPaused = false;
			mResizing = false;
			CharalotteEngine::GetInstance().GetTimer()->Start();
			FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_KEYDOWN:
			OnKeyBoardInput();
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if ((int)wParam == VK_F2)
			{
			}


			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void FWin32Window::CalculateFrameStats()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((CharalotteEngine::GetInstance().GetTimer()->TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsstr = std::to_wstring(fps);
			std::wstring mspfstr = std::to_wstring(mspf);
			std::wstring senstr = std::to_wstring(CameraSen);

			std::wstring windowText = mMainWndCaption +
				L" FPS : " + fpsstr +
				L" MSPF: " + mspfstr +
				L" Camera sensitivity: " + senstr;
			WinText = windowText;

			SetWindowText(MainWnd(), windowText.c_str());

			// reset
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	void FWin32Window::OnMouseDown(WPARAM btnState, int x, int y)
	{
		Charalotte::WindowsInputData MouseInput = { btnState , x, y, Charalotte::MouseDown };
		FWinEventRegisterSystem::GetInstance().ExecuteWindowsEvent(MouseInput);
		SetCapture(mhMainWnd);
	}
	void FWin32Window::OnMouseUp(WPARAM btnState, int x, int y)
	{
		ReleaseCapture();
	}
	void FWin32Window::OnMouseMove(WPARAM btnState, int x, int y)
	{
		Charalotte::WindowsInputData MouseInput = { btnState , x, y, Charalotte::Move };
		FWinEventRegisterSystem::GetInstance().ExecuteWindowsEvent(MouseInput);
	}

	void FWin32Window::OnKeyBoardInput()
	{
		for (const auto& KeyIter : FWinEventRegisterSystem::GetInstance().GetEventKeys())
		{
			if (GetAsyncKeyState(KeyIter) & 0x8000)
			{
				FWinEventRegisterSystem::GetInstance().ExecuteKeyEvent(KeyIter);
			}
		}
	}

	bool FWin32Window::GetIsPaused()
	{
		return IsPaused;
	}
}
