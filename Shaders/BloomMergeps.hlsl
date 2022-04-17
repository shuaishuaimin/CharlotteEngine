#include "BloomMergeps.hlsli"

Texture2D gBloomUp : register(t0);
Texture2D gBloomDown : register(t1);

SamplerState gBloomInputSampler : register(s0);

float4 RenderTargetSize : register(b0);

float2 Circle(float Start, float Points, float Point)
{
	float Radians = (2.0f * 3.141592f * (1.0f / Points)) * (Start + Point);
	return float2(cos(Radians), sin(Radians));
}

float2 SunShaftPosToUV(float2 Pos)
{

	return Pos.xy * float2(0.5, -0.5) + 0.5;
}

float2 SunPos()
{
	float4 LightShaftCenter = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return LightShaftCenter.xy;
}

float2 SunShaftRect(float2 InPosition, float amount)
{
	float2 center = SunPos();
	return SunShaftPosToUV(lerp(center, InPosition, amount));
}

float2 VignetteSpace(float2 Pos, float AspectRatio)
{

	float Scale = sqrt(2.0) / sqrt(1.0 + AspectRatio * AspectRatio);
	return Pos * float2(1.0, AspectRatio) * Scale;
}

float Square(float x)
{
	return x * x;
}

float ComputeVignetteMask(float2 VignetteCircleSpacePos, float Intensity)
{
	VignetteCircleSpacePos *= Intensity;
	float Tan2Angle = dot(VignetteCircleSpacePos, VignetteCircleSpacePos);
	float Cos4Angle = Square(rcp(Tan2Angle + 1));
	return Cos4Angle;
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


[RootSignature(BloomMergeps_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float ScaleUV = 0.66f / 2.0f;
	float4 BloomColor1 = float4(0.5016f, 0.5016f, 0.5016f, 0.0f);

	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);


	float2 Tex;
	Tex.x = 1.0f * X / RenderTargetSize[0];
	Tex.y = 1.0f * Y / RenderTargetSize[1];


	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];
	float2 DeltaUV = float2(DeltaU, DeltaV);

	float Start = 2.0f / 6.0f;
	float4 Color0 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * ScaleUV * Circle(Start, 6.0, 0.0f));
	float4 Color1 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * ScaleUV * Circle(Start, 6.0, 1.0f));
	float4 Color2 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * ScaleUV * Circle(Start, 6.0, 2.0f));
	float4 Color3 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * ScaleUV * Circle(Start, 6.0, 3.0f));
	float4 Color4 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * ScaleUV * Circle(Start, 6.0, 4.0f));
	float4 Color5 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * ScaleUV * Circle(Start, 6.0, 5.0f));
	float4 Color6 = gBloomDown.Sample(gBloomInputSampler, Tex);

	float ScaleColor1 = 1.0f / 7.0f;
	float ScaleColor2 = 1.0f / 7.0f;
	float4 BloomColor = Color6 * ScaleColor1 +
		Color0 * ScaleColor2 +
		Color1 * ScaleColor2 +
		Color2 * ScaleColor2 +
		Color3 * ScaleColor2 +
		Color4 * ScaleColor2 +
		Color4 * ScaleColor2 * rcp(ScaleColor1 * 1.0f + ScaleColor2 * 6.0f);

	OutColor.rgb = gBloomUp.Sample(gBloomInputSampler, Tex).rgb;

	float ScaleColor3 = 1.0f / 5.0f;
	OutColor.rgb *= ScaleColor3;


	OutColor.rgb += (BloomColor * ScaleColor3 * BloomColor1).rgb;
	OutColor.a = 1.0f;
	return OutColor;
}


