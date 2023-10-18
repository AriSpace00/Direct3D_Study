Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
SamplerState samLinear : register(s0);

cbuffer Transform : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer DirectionLight : register(b1)
{
    float3 LightDirection;
    float DirectionLight_Pad0;
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
    float3 EyePosition;
    int UseNormalMap;
    int UseSpecularMap;
}

cbuffer Material : register(b2)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float MaterialSpecularPower;
    float3 Material_Pad0;
}

struct VS_INPUT
{
    float4 PosModel : POSITION;
    float2 Texcoord : TEXCOORD0;
    float3 NorModel : NORMAL;
    float3 TanModel : TANGENT;
};

struct PS_INPUT
{
    float4 PosProjection : SV_POSITION;
    float3 PosWorld : POSITION;
    float2 Texcoord : TEXCOORD0;
    float3 NorWorld : NORMAL;
    float3 TanWorld : TANGENT;
};

