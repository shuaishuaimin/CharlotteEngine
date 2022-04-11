#pragma once
#include "STransformDef.h"

namespace Charalotte
{
	struct FConstantBuffer
	{
		glm::mat4 Test = glm::mat4(1.0f);
		glm::mat4 Tans = glm::mat4(1.0f);
		glm::mat4 World = glm::mat4(1.0f);
		glm::mat4 MVP = glm::mat4(1.0f);
		glm::mat4 Scale3D = glm::mat4(1.0f);
		glm::mat4 Rotate = glm::mat4(1.0f);
		FLightForShading LightInfo;
		float Offset = 0;
	};
	struct FSceneBounds
	{
		glm::vec3 Center;
		float Radius;
	};

	struct FPassConstants
	{
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Proj = glm::mat4(1.0f);
		glm::mat4 ViewProj = glm::mat4(1.0f);
		glm::mat4 ShadowTransform = glm::mat4(1.0f);

		glm::vec3 EyePosW = glm::vec3(0.0f);
		float cbPerObjectPad1 = 0.0f;
		glm::vec2 RenderTargetSize = { 0.0f, 0.0f };

		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		glm::vec4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		FLight Lights;
	};



}