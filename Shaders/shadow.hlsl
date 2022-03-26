#include "Engine/ShadowH.hlsli"

SamplerState gsamLiner : register(s0);

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorldViewProj;
	float4x4 gRotate;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);

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
	float4 Normal : NORMAL;
};

[RootSignature(Shadow_RootSig)]
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
	float4 Result = float4(diffuseAlbedo.x * normal.x, diffuseAlbedo.y * normal.y, diffuseAlbedo.z * normal.z, 1.0f);
	return Result;
}
