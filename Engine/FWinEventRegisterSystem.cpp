#include "stdafx.h"
#include "FWinEventRegisterSystem.h"

FWinEventRegisterSystem::FWinEventRegisterSystem()
{
	KeyBEvents = std::unordered_map<char, Charalotte::KeyInputType>();
	MouseEvents = std::unordered_map<Charalotte::WindowsAction, Charalotte::WindowsInputType>();
	IsExecuteKeyEvent = false;
	EventKeys = {};
	ActionMouses = {};
	IsExecuteMouseEvent = false;
}
FWinEventRegisterSystem::~FWinEventRegisterSystem()
{
	KeyBEvents.clear();
	MouseEvents.clear();
}

void FWinEventRegisterSystem::RegisterKeyInput(char Key, const std::function<void()>& Function)
{
	KeyBEvents.insert({ Key, Function });
	EventKeys.insert(Key);
}

void FWinEventRegisterSystem::UnKeyRegisterInput(char Key)
{
	KeyBEvents.erase(Key);
	EventKeys.erase(Key);
}

void FWinEventRegisterSystem::ExecuteKeyEvent(char Key)
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

std::set<char> FWinEventRegisterSystem::GetEventKeys()
{
	return EventKeys;
}

void FWinEventRegisterSystem::RegisterWindowsInput(Charalotte::WindowsAction Action, const Charalotte::WindowsInputType& Event)
{
	MouseEvents.insert({ Action, Event });
	ActionMouses.insert(Action);
}

void FWinEventRegisterSystem::UnRegisterWindowsInput(Charalotte::WindowsAction Action)
{
	MouseEvents.erase(Action);
	ActionMouses.erase(Action);
}

void FWinEventRegisterSystem::ExecuteWindowsEvent(const Charalotte::WindowsInputData& Input)
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


void FWinEventRegisterSystem::RegisterMapLoadEventForDender(Charalotte::MapLoadType Type, std::function<void(const std::string& MapName)> Function)
{
	LoadMapEvents.insert({ Type, Function });
	NowLoadType = Type;
}

void FWinEventRegisterSystem::ExecuteMapLoadEvent(const std::string& MapName)
{
	auto UpLoadEvent = LoadMapEvents.find(NowLoadType);
	if (UpLoadEvent != LoadMapEvents.end())
	{
		UpLoadEvent->second(MapName);
	}
}
void FWinEventRegisterSystem::UnRegisterMapLoadEventForDender(Charalotte::MapLoadType Type)
{
	LoadMapEvents.erase(Type);
}

