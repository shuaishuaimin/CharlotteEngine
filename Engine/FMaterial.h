#pragma once
#include <memory>
#include <vector>
#include <string>
class FMaterial
{
public:
	FMaterial() : mTextureName("bricks"), mNormalName("bricks")
	{	
	}

	~FMaterial(){}
	void LoadMaterial(){}

	inline void SetTexture(const std::string& TextureName)
	{
		mTextureName = TextureName;
	}

	std::string GetTexture() const
	{
		return mTextureName;
	}

	inline void SetNormal(const std::string& NormalName)
	{
		mNormalName = NormalName;
	}

	std::string GetNormal() const 
	{
		return mNormalName;
	}
private:
	std::string mTextureName;
	std::string mNormalName;
};