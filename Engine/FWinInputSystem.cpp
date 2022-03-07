#include "stdafx.h"
#include "FWinInputSystem.h"

FWinInputSystem::FWinInputSystem()
{
	KeyBEvents = std::unordered_map<char, Charalotte::KeyInputType>();
	MouseEvents = std::unordered_map<Charalotte::MouseAction, Charalotte::MouseInputType>();
	IsExecuteKeyEvent = false;
	EventKeys = {};
	ActionMouses = {};
	IsExecuteMouseEvent = false;
}
FWinInputSystem::~FWinInputSystem()
{
	
}

void FWinInputSystem::RegisterKeyInput(char Key, const std::function<void(const FGameTimer& gt)>& Function)
{
	KeyBEvents.insert({ Key, Function });
	EventKeys.insert(Key);
}

void FWinInputSystem::UnKeyRegisterInput(char Key)
{
	KeyBEvents.erase(Key);
	EventKeys.erase(Key);
}

void FWinInputSystem::ExecuteKeyEvent(char Key, const FGameTimer& gt)
{
	IsExecuteKeyEvent = true;
	auto KeyIter = KeyBEvents.find(Key);
	if (KeyIter != KeyBEvents.end())
	{
		KeyIter->second(gt);
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

void FWinInputSystem::RegisterMouseInput(const Charalotte::MouseInputData& Action, const Charalotte::MouseInputType& Event)
{
	MouseEvents.insert({ Action.Action, Event });
	ActionMouses.insert(Action.Action);
}

void FWinInputSystem::UnRegisterMouseInput(const Charalotte::MouseInputData& Action)
{
	MouseEvents.erase(Action.Action);
	ActionMouses.erase(Action.Action);
}

void FWinInputSystem::ExecuteMouseEvent(const Charalotte::MouseInputData& Input)
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