//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
#include "Engine/Shadow.hlsli"

SamplerState gsamLiner : register(s0);

float4 CameraLoc : register(b0);

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorldViewProj; 
	float4x4 gRotate;
};

Texture2D ShadowTexture : register(t0);

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
	//float4 Normal : NORMAL;
};

[RootSignature(TreeStump_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	float4 ResultNormal = vin.Normal;

	ResultNormal = mul(ResultNormal, gRotate);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = ResultNormal;

	vout.TexC = vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLiner, pin.TexC);
	float4 normal = gNormalMap.Sample(gsamLiner, pin.TexC);
	float4 Result = float4( diffuseAlbedo.x*normal.x, diffuseAlbedo.y * normal.y, diffuseAlbedo.z * normal.z, 1.0f);
    return Result;
}


