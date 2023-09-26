#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    finalColor += saturate(dot((float3) vLightDir, input.normal) * vLightColor);

    finalColor.a = 1;
    return finalColor;
}