#include "BloomUp.hlsli"

Texture2D gBloomUp : register(t0);
Texture2D gBloomDown : register(t1);

SamplerState gBloomInputSampler : register(s0);

float4 RenderTargetSize : register(b0);

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


[RootSignature(BloomUp_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float BloomWeightScalar = 1.0f / RenderTargetSize[2];
	float BloomWeightScalar1 = 1.0f / RenderTargetSize[3];

	float BloomUpScale = 2.0f;

	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);

	float2 Tex;
	Tex.x = 1.0f * X / RenderTargetSize[0];
	Tex.y = 1.0f * Y / RenderTargetSize[1];

	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];
	float2 DeltaUV = float2(DeltaU, DeltaV);

	float StartDinominator = 25.0;
	float Start = 2.0 / StartDinominator;
	float4 ColorsUp[25];
	float4 ColorsDown[25];
	float4 ColorUpTotal;
	float4 ColorDownTotal;
	for (int i = 0; i < StartDinominator; i++)
	{
		ColorsUp[i] = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, StartDinominator, (float)i));
		ColorsDown[i] = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, StartDinominator, (float)i));
	}
	ColorUpTotal = gBloomUp.Sample(gBloomInputSampler, Tex);
	ColorDownTotal = gBloomDown.Sample(gBloomInputSampler, Tex);

	float4 BloomWight = float4(BloomWeightScalar, BloomWeightScalar, BloomWeightScalar, 0.0f);
	float4 BloomWight1 = float4(BloomWeightScalar1, BloomWeightScalar1, BloomWeightScalar1, 0.0f);

	float4 ColorUpWight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 ColorDownWight1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int j = 0; j < StartDinominator; j++)
	{
		ColorUpWight = ColorUpWight + ColorsUp[j];
		ColorDownWight1 = ColorDownWight1 + ColorsDown[j];
	}
	OutColor = ColorUpWight * BloomWight + ColorDownWight1 * BloomWight1;
	OutColor.a = 0.0f;


	return OutColor;

	//float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	//float BloomWeightScalar = 1.0f / RenderTargetSize[2];
	//float BloomWeightScalar1 = 1.0f / RenderTargetSize[3];

	//float BloomUpScale = 4.0f;

	//int X = floor(pin.PosH.x);
	//int Y = floor(pin.PosH.y);


	//float2 Tex;
	//Tex.x = 1.0f * X / RenderTargetSize[0];
	//Tex.y = 1.0f * Y / RenderTargetSize[1];

	//float DeltaU = 1.0f / RenderTargetSize[0];
	//float DeltaV = 1.0f / RenderTargetSize[1];
	//float2 DeltaUV = float2(DeltaU, DeltaV);

	//float Start = 2.0 / 7.0;
	//float4 Color0 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 0.0f));
	//float4 Color1 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 1.0f));
	//float4 Color2 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 2.0f));
	//float4 Color3 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 3.0f));
	//float4 Color4 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 4.0f));
	//float4 Color5 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 5.0f));
	//float4 Color6 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 6.0f));
	//float4 Color7 = gBloomUp.Sample(gBloomInputSampler, Tex);

	//float4 Color8 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 0.0f));
	//float4 Color9 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 1.0f));
	//float4 Color10 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 2.0f));
	//float4 Color11 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 3.0f));
	//float4 Color12 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 4.0f));
	//float4 Color13 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 5.0f));
	//float4 Color14 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 6.0f));
	//float4 Color15 = gBloomDown.Sample(gBloomInputSampler, Tex);



	//float4 BloomWight = float4(BloomWeightScalar, BloomWeightScalar, BloomWeightScalar, 0.0f);
	//float4 BloomWight1 = float4(BloomWeightScalar1, BloomWeightScalar1, BloomWeightScalar1, 0.0f);

	//OutColor = (Color0 + Color1 + Color2 + Color3 + Color4 + Color5 + Color6 + Color7) * BloomWight +
	//	(Color8 + Color9 + Color10 + Color11 + Color12 + Color13 + Color14 + Color15) * BloomWight1;
	//OutColor.a = 0.0f;


	//return OutColor;
}


