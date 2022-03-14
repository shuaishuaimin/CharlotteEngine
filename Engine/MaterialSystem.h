#pragma once
#include <memory>
#include <windows.h>
#include "DDSTextureLoader.h"
#include "WinMaterialData.h"



class MaterialSystem
{
public:
	MaterialSystem(){
		
	}

private:
	// Unique material name for lookup.
	std::string Name;

	// Index into constant buffer corresponding to this material.
	int MatCBIndex = -1;

	// Index into SRV heap for diffuse texture.
	int DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int NormalSrvHeapIndex = -1;

	// Dirty flag indicating the material has changed and we need to update the constant buffer.
	// Because we have a material constant buffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify a material we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty;

	// Material constant buffer data used for shading.
	glm::vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;
	glm::mat4 MatTransform = FMathHelper::Identity4x4();
};