#include "BloomDown.hlsli"

Texture2D gSceneColor : register(t0);
SamplerState gSceneColorSampler : register(s0);

float2 RenderTargetSize : register(b0);

float2 Circle(float Start, float Points, float Point)
{
	float Radians = (2.0f * 3.141592f * (1.0f / Points)) * (Start + Point);
	return float2(cos(Radians), sin(Radians));
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


[RootSignature(BloomDown_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	float BloomDownScale = 1.3f;

	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);

	float Width = RenderTargetSize[0] * 0.5f;
	float Height = RenderTargetSize[1] * 0.5f;

	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];

	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;


	float2 DeltaUV = float2(DeltaU, DeltaV);

	float DoubleRadias = 14.0f;
	float StartRaduas = 2.0f / DoubleRadias;
	float4 Colors[14];
	float4 Color = gSceneColor.Sample(gSceneColorSampler, Tex);
	for (int i = 0; i < DoubleRadias; i++)
	{
		Colors[i] = gSceneColor.Sample(gSceneColorSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, DoubleRadias, float(i)));
		Color = Color + Colors[i];
	}

	float Weight = 1.0f / (DoubleRadias + 1) * 2.0f;
	Color = Weight * Color;

	return Color;
}


