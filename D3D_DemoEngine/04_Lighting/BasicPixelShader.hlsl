#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    // 빛의 방향만을 남기기 위해 normalize 해줌
    float3 vLightDirection = normalize(vLightDir.xyz); 
    // 빛의 방향과 픽셀의 법선 벡터를 내적하고, 빛의 색상과 곱해준 후 0-1 사이로 클램핑함
    finalColor = saturate(dot((float3)vLightDirection, input.Nor) * vLightColor);
    // 알파값 1로 조정하여 불투명도 조절
    finalColor.a = 1;
    // 텍스처 색상을 샘플러 속성에 따라 설정
    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);

    finalColor *= textureColor;

    return finalColor;
}