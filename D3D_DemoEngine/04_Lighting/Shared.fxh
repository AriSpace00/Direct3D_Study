Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer_MJ : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 vLightDir;
    float4 vLightColor;
    float4 vOutputColor;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Nor : TEXCOORD0;
    float2 Tex : TEXCOORD1;
};

