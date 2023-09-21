cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 vLightDir[2];
    float4 vLightColor[2];
    float4 vOutputColor;
}

struct VS_INPUT
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : TEXCOORD0;
};

