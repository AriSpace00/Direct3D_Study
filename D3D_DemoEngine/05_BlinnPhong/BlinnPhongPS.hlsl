#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    float3 Normal = normalize(input.NorWorld);
    float fNDotL = max(dot(Normal, -LightDirection), 0);
    float4 Ambient = LightAmbient * MaterialAmbient;
    float3 LightReflection = reflect(LightDirection, Normal);
    float fRDotV = max(0.0f, dot(LightReflection, ));


    finalColor = saturate(dot((float3) LightDirection, input.NorWorld) * LightDiffuse);
    finalColor.a = 1;

    float4 textureColor = txDiffuse.Sample(samLinear, input.Texcoord);

    finalColor *= textureColor;

    return finalColor;
}