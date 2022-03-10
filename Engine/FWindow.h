#pragma once

class FWindow
{
public:
	FWindow() : Width(0.0f) , Height(0.0f)
	{}
	~FWindow() {}

	virtual bool InitMainWindow() = 0;

	virtual void Update() = 0;

private:
	float Width;
	float Height;
};