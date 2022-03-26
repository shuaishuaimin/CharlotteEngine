//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
#include "Engine/ShaderH.hlsli"

SamplerState gsamLiner : register(s0);
SamplerComparisonState gsamShadow : register(s1);


float4 CameraLoc : register(b0);

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorldViewProj; 
	float4x4 gRotate;
	float4 IsShadow;
	float4x4 World;
	float4x4 ShadowVP;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gShadowMap : register(t2);

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float4 Normal : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float2 TexC : TEXCOORD;
	float4 ShadowPos : POSITION;
	//float4 Normal : NORMAL;
};


float CalcShadowFactor(float4 shadowPosH)
{
	shadowPosH.xyz /= shadowPosH.w;

	float depth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	//	float2 PixelPos = shadowPosH.xy*width;
	//	float depthInMap = gShadowMap.SampleLevel(gsamAnisotropicWrap, shadowPosH.xy, 0).r;
	//	return depth > depthInMap? 0:1;

	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		//percentLit += gShadowMap.SampleLevel(gsamAnisotropicWrap, shadowPosH.xy + offsets[i], 0).r - depth > 0 ? 1 : 0;
		percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow, shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}


[RootSignature(TreeStump_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.ShadowPos = mul(float4(vin.PosL, 1.0f), World);
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	float4 ResultNormal = vin.Normal ;

	ResultNormal = mul(ResultNormal, gRotate);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = ResultNormal;

	vout.TexC = vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 Result;
	if (IsShadow.x > 0.0f)
	{
		float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLiner, pin.TexC);
		Result = diffuseAlbedo;
	}
	else
	{
		float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLiner, pin.TexC);
		float4 TShadowVP = mul(pin.ShadowPos, ShadowVP);
		float4 RealShadowWithVp = (TShadowVP.x * 0.5+0.5, TShadowVP.y*0.5+0.5, TShadowVP.z, 1.0f);
		float4 Depth = gShadowMap.Sample(gsamLiner, RealShadowWithVp.xy);
		if (Depth.z > RealShadowWithVp.z)
		{
			Result = 0.1 * diffuseAlbedo;
		}
		else
		{
			Result = diffuseAlbedo;
		}
	}
    return Result;
}
