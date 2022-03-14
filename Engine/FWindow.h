#pragma once
#include <string>

class FWindow
{
public:
	FWindow() : Width(0.0f) , Height(0.0f)
	{}
	~FWindow() {}

	virtual bool InitMainWindow() = 0;

	virtual void Update() = 0;

	virtual bool GetIsPaused() = 0;

	virtual bool GetIsExit() = 0;

	virtual void SetCameraSen(float SenNum) = 0;
private:
	float Width;
	float Height;
};