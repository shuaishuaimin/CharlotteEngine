#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include "FGameTimer.h"
#include "Singleton.h"
#include "FInputSystem.h"


namespace Charalotte
{
	using KeyInputType = std::function<void()>;
	using WindowsInputType = std::function<void(const WindowsInputData& Mouse)>;

}

class FWinEventRegisterSystem : public Singleton<FWinEventRegisterSystem>, public FInputSystem
{
public:
	FWinEventRegisterSystem();
	~FWinEventRegisterSystem();

	void RegisterKeyInput(char Key, const std::function<void()>& Function);

	void UnKeyRegisterInput(char Key);

	void ExecuteKeyEvent(char Key);

	void RegisterWindowsInput(Charalotte::WindowsAction Action, const Charalotte::WindowsInputType& Event);

	void UnRegisterWindowsInput(Charalotte::WindowsAction Action);

	void ExecuteWindowsEvent(const Charalotte::WindowsInputData& Input);

	std::set<char> GetEventKeys();

	// when new map loaded, execute load event
	void RegisterMapLoadEventForDender(Charalotte::MapLoadType Type, std::function<void(const std::string& MapName)> Function);
	
	void ExecuteMapLoadEvent(const std::string& MapName);

	void UnRegisterMapLoadEventForDender(Charalotte::MapLoadType Type);

	void RegisterOnResize(Charalotte::ResizeType Type, const std::function<void()>& OnResize);

	void ExecuteOnResizeEvent(Charalotte::ResizeType ResizeType);

	void UnRegisterOnResizeEvent(Charalotte::ResizeType ResizeType);
private:
	// keyb event
	std::unordered_map<char, Charalotte::KeyInputType> KeyBEvents;
	bool IsExecuteKeyEvent;
	std::set<char> EventKeys;

	// mouse event
	std::unordered_map<Charalotte::WindowsAction, Charalotte::WindowsInputType> MouseEvents;
	bool IsExecuteMouseEvent;
	std::set<Charalotte::WindowsAction> ActionMouses;

	// load map event
	std::unordered_map<Charalotte::MapLoadType, std::function<void(const std::string& MapName)>> LoadMapEvents;
	Charalotte::MapLoadType NowLoadType;

	// OnResize
	std::unordered_map<Charalotte::ResizeType, std::function<void()>> OnResizeEvents;
};

