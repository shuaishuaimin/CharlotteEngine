//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

//Texture2D gDiffuseMap : register(t0);
//
//SamplerState gsamLinerWrap : register(s0);

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gRotate;
};

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

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	float4 ResultNormal = vin.Normal;

	ResultNormal = mul(ResultNormal, gRotate);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = ResultNormal;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	//float4 diffuseAlbedo = gDiffuseMap.Sample;
    return pow((pin.Color + 1) * 0.5, 1/2.2f);
}


