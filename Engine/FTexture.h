#pragma once
#include <string>

class FTexture
{
public:
	FTexture(const std::string& Path) : TexturePath(Path)
	{

	}

	~FTexture(){}
	std::string GetTexturePath()
	{
		return TexturePath;
	}

	void SetTexturePath(const std::string& Path)
	{
		TexturePath = Path;
	}

	void BuildResource()
	{

	}
private:
	std::string TexturePath;
};