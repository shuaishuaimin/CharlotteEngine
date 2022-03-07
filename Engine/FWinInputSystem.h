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

namespace Charalotte
{
	struct MouseInputData;
	using KeyInputType = std::function<void(const FGameTimer& gt)>;
	using MouseInputType = std::function<void(const MouseInputData& Mouse)>;
	
	enum MouseAction
	{
		Move,
		LMouseDown,
		RMouseDown
	};
	struct MouseInputData
	{
		WPARAM btnState;
		int x;
		int y;
		MouseAction Action;

		bool operator == (const MouseInputData& A)
		{
			if (this->Action == A.Action)
			{
				return true;
			}
			return false;
		}
	};
}

class FWinInputSystem : public Singleton<FWinInputSystem>, public FInputSystem
{
public:
	FWinInputSystem();
	~FWinInputSystem();

	void RegisterKeyInput(char Key, const std::function<void(const FGameTimer& gt)>& Function);

	void UnKeyRegisterInput(char Key);

	void ExecuteKeyEvent(char Key, const FGameTimer& gt);

	void RegisterMouseInput(const Charalotte::MouseInputData& Action, const Charalotte::MouseInputType& Event);

	void UnRegisterMouseInput(const Charalotte::MouseInputData& Input);

	void ExecuteMouseEvent(const Charalotte::MouseInputData& Input);

	std::set<char> GetEventKeys();

private:
	std::unordered_map<char, Charalotte::KeyInputType> KeyBEvents;
	bool IsExecuteKeyEvent;
	std::set<char> EventKeys;

	std::unordered_map<Charalotte::MouseAction, Charalotte::MouseInputType> MouseEvents;
	bool IsExecuteMouseEvent;
	std::set<Charalotte::MouseAction> ActionMouses;
};

