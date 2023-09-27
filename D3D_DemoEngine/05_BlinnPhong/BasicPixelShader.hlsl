#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    finalColor = saturate(dot((float3) vLightDir, input.Nor) * vLightColor);
    finalColor.a = 1;

    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);

    finalColor *= textureColor;

    return finalColor;
}