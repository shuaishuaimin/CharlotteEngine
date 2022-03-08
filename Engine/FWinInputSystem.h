#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <windows.h>
#include "FGameTimer.h"
#include "Singleton.h"
#include "FInputSystem.h"
#include "BaseData.h"

namespace Charalotte
{
	using KeyInputType = std::function<void()>;
	using WindowsInputType = std::function<void(const WindowsInputData& Mouse)>;

}

class FWinInputSystem : public Singleton<FWinInputSystem>, public FInputSystem
{
public:
	FWinInputSystem();
	~FWinInputSystem();

	void RegisterKeyInput(char Key, const std::function<void()>& Function);

	void UnKeyRegisterInput(char Key);

	void ExecuteKeyEvent(char Key);

	void RegisterWindowsInput(Charalotte::WindowsAction Action, const Charalotte::WindowsInputType& Event);

	void UnRegisterWindowsInput(Charalotte::WindowsAction Action);

	void ExecuteWindowsEvent(const Charalotte::WindowsInputData& Input);

	std::set<char> GetEventKeys();

private:
	std::unordered_map<char, Charalotte::KeyInputType> KeyBEvents;
	bool IsExecuteKeyEvent;
	std::set<char> EventKeys;

	std::unordered_map<Charalotte::WindowsAction, Charalotte::WindowsInputType> MouseEvents;
	bool IsExecuteMouseEvent;
	std::set<Charalotte::WindowsAction> ActionMouses;
};

