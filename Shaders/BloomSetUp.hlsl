#include "BloomSetUp.hlsli"

Texture2D gSceneColor : register(t0);
SamplerState gSceneColorSampler : register(s0);

float2 RenderTargetSize : register(b0);

float Luminance(float3 InColor)
{
	return dot(InColor, float3(0.3f, 0.59f, 0.11f));
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


[RootSignature(SetUp_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	const float BloomThreshold = 1.0f;

	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);

	float Width = RenderTargetSize[0] * 0.25f;
	float Height = RenderTargetSize[1] * 0.25f;

	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];

	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;

	float4 Color0 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(-DeltaU, -DeltaV));
	float4 Color1 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+DeltaU, -DeltaV));
	float4 Color2 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(-DeltaU, +DeltaV));
	float4 Color3 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+DeltaU, +DeltaV));

	float4 AvailableColor = Color0 * 0.25f + Color1 * 0.25f + Color2 * 0.25f + Color3 * 0.25f;

	float TotalLuminance = Luminance(AvailableColor.rgb);
	float BloomLuminance = TotalLuminance > 1 ? TotalLuminance : 0;
	float Amount = saturate(BloomLuminance * 0.5f);

	float4 OutColor;
	OutColor.rgb = AvailableColor.rgb;
	OutColor.rgb *= Amount;
	OutColor.a = 1.0f;

	return OutColor;
}


