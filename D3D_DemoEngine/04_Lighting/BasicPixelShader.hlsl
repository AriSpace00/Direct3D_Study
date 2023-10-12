#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    float3 vLightDirection = normalize(vLightDir.xyz);
    finalColor = saturate(dot((float3) vLightDirection, input.Nor) * vLightColor);
    finalColor.a = 1;

    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);

    finalColor *= textureColor;

    return finalColor;
}