
#include "Engine/ShadowH.hlsli"


Texture2D	ShadowMap	:	register(t0);

Texture2D	SomeTex	:	register(t1);


struct Light
{
	float3 Strength;
	float FalloffStart; // point/spot light only
	float3 Direction;   // directional/spot light only
	float FalloffEnd;   // point/spot light only
	float3 Position;    // point light only
	float SpotPower;    // spot light only
};



cbuffer cbPerObject : register(b0)
{
	float4x4	Test;
	float4x4	gTrans;
	float4x4	gWorld;
	float4x4	gWorldViewProj;
	float4x4	gScale3D;
	float4x4	gRotate;
	float		gOffset;
};


cbuffer cbPass : register(b1)
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
};

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
};





[RootSignature(Shadow_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.PosL, 1.0f), gTrans);

	vout.Color = vin.Color;
	vout.Normal = vin.Normal;
	vout.TexCoord = vin.TexCoord ;
	return vout;
}

void PS(VertexOut pin) 
{
	/*return pin.Normal;*/
}

