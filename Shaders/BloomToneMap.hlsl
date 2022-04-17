#include "BloomToneMap.hlsli"

Texture2D gSceneColor : register(t0);
Texture2D gSunMergeColor : register(t1);

SamplerState gSceneColorSampler : register(s0);

float2 RenderTargetSize : register(b0);

float3 ACESToneMapping(float3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

struct VertexIn
{
	float3 PosL			: POSITION;
	float4 Tangent		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH			: SV_POSITION;
};


[RootSignature(BloomToneMap_BloomSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);

	float2 Tex;
	Tex.x = 1.0f * X / RenderTargetSize[0];
	Tex.y = 1.0f * Y / RenderTargetSize[1];

	float4 SceneColor = gSceneColor.Sample(gSceneColorSampler, Tex);
	float4 BloomColor = gSunMergeColor.Sample(gSceneColorSampler, Tex);

	half3 LinearColor = SceneColor.rgb + BloomColor.rgb;

	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	OutColor.rgb = ACESToneMapping(LinearColor, 1.0f);
	OutColor.a = SceneColor.a;

	return OutColor;
}


