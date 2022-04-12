//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#include "Engine/CommonH.hlsli"

struct Light
{
	float3 Strength;
	float FalloffStart; // point/spot light only
	float3 Direction;   // directional/spot light only
	float FalloffEnd;   // point/spot light only
	float3 Position;    // point light only
	float SpotPower;    // spot light only
};

struct LightForShading
{
	float4 LightVec;
	float LightStrength;
	//float EnviroumentLightStrength;
};

struct ShadingParams
{
	float3 VecPos;
	float4 Normal;
	float LightStrength;
	float4 LightVec;
};

Texture2D	gDiffuseMap	:	register(t0);


Texture2D	gShadowMap	:	register(t1);

SamplerState	gSamLinear	:	register(s0);
SamplerComparisonState gsamShadow : register(s1);

float4	CameraLoc	:	register(b0);

cbuffer cbPerObject : register(b1)
{
	float4x4	Test;
	float4x4	gTrans;
	float4x4	gWorld;
	float4x4	gWorldViewProj; 
	float4x4	gScale3D;
	float4x4	gRotate;
	LightForShading gLightInfo;
	float		gOffset;
};

cbuffer	cbPass	:	register(b2)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gShadowTransform;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;
	Light gLight;
}

struct VertexIn
{
	float3 PosL			: POSITION;
    float4 Color		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH			: SV_POSITION;
    float4 Color		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
	float4 ShadowPos	: SHADOWPOS;
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


float VecMulVec(float4 Vec1, float4 Vec2)
{
	return (Vec1.x * Vec2.x + Vec1.y * Vec2.y + Vec1.z * Vec2.z);
}

float CalcDiffuseShadingLevel(ShadingParams Params)
{
	float ReflectionKd = 1.0f;
	float Reflection = ReflectionKd * ( Params.LightStrength) * max(VecMulVec(normalize(Params.Normal),  normalize(Params.LightVec)) , 0.0f );
	float BlingPhongKs = 100.0;
	float4 bisector = normalize(normalize(Params.LightVec) + normalize(CameraLoc - float4(Params.VecPos, 1.0f)));
	float BlingPhongP = 4.0f;
	float BlingPhong = BlingPhongKs * (Params.LightStrength) * max(0.0f, pow(VecMulVec(bisector, Params.Normal), BlingPhongP));
	float Enviroument = 0.4;
	return Reflection + BlingPhong + Enviroument;
}

float4 NumMulVec4(float value, float4 vec4)
{
	return float4(value * vec4.x, value * vec4.y, value * vec4.z, 1.0f);
}

[RootSignature(Test_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    vout.Color = vin.Color;
	vout.Normal = mul(vin.Normal,gRotate);
	vout.TexCoord = vin.TexCoord;

	vout.ShadowPos = mul(mul(float4(vin.PosL, 1.0f), gWorld), gTrans);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear  , pin.TexCoord);

	float Shadow = CalcShadowFactor(pin.ShadowPos);

	ShadingParams Params;
	Params.VecPos = float3(pin.ShadowPos.x, pin.ShadowPos.y, pin.ShadowPos.z);
	Params.Normal = pin.Normal;
	Params.LightStrength = gLightInfo.LightStrength;
	Params.LightVec = gLightInfo.LightVec;
	float ShadingDegree = CalcDiffuseShadingLevel(Params);

	return  NumMulVec4(ShadingDegree, pow(diffuseAlbedo * (Shadow + 0.1), 1 / 2.2f));
}


