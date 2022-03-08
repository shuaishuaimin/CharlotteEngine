#include "stdafx.h"
#include "FWinInputSystem.h"

FWinInputSystem::FWinInputSystem()
{
	KeyBEvents = std::unordered_map<char, Charalotte::KeyInputType>();
	MouseEvents = std::unordered_map<Charalotte::WindowsAction, Charalotte::WindowsInputType>();
	IsExecuteKeyEvent = false;
	EventKeys = {};
	ActionMouses = {};
	IsExecuteMouseEvent = false;
}
FWinInputSystem::~FWinInputSystem()
{
	KeyBEvents.clear();
	MouseEvents.clear();
}

void FWinInputSystem::RegisterKeyInput(char Key, const std::function<void()>& Function)
{
	KeyBEvents.insert({ Key, Function });
	EventKeys.insert(Key);
}

void FWinInputSystem::UnKeyRegisterInput(char Key)
{
	KeyBEvents.erase(Key);
	EventKeys.erase(Key);
}

void FWinInputSystem::ExecuteKeyEvent(char Key)
{
	IsExecuteKeyEvent = true;
	auto KeyIter = KeyBEvents.find(Key);
	if (KeyIter != KeyBEvents.end())
	{
		KeyIter->second();
		IsExecuteKeyEvent = false;
		return;
	}
	IsExecuteKeyEvent = false;
	return;
}

std::set<char> FWinInputSystem::GetEventKeys()
{
	return EventKeys;
}

void FWinInputSystem::RegisterWindowsInput(Charalotte::WindowsAction Action, const Charalotte::WindowsInputType& Event)
{
	MouseEvents.insert({ Action, Event });
	ActionMouses.insert(Action);
}

void FWinInputSystem::UnRegisterWindowsInput(Charalotte::WindowsAction Action)
{
	MouseEvents.erase(Action);
	ActionMouses.erase(Action);
}

void FWinInputSystem::ExecuteWindowsEvent(const Charalotte::WindowsInputData& Input)
{
	IsExecuteMouseEvent = true;
	auto KeyIter = MouseEvents.find(Input.Action);
	if (KeyIter != MouseEvents.end())
	{
		KeyIter->second(Input);
		IsExecuteKeyEvent = false;
		return;
	}
	IsExecuteKeyEvent = false;
	return;
}

