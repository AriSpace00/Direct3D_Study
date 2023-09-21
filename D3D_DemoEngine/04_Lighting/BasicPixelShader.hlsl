#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    for (int i = 0; i < 2; i++)
    {
        finalColor += saturate(dot((float3) vLightDir[i], input.normal) * vLightColor[i]);
    }
    finalColor.a = 1;
    return finalColor;
}