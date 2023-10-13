#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    // 물체로부터 뻗어나가는 노멀 방향만 남기기 위해 노멀라이즈
    float3 Normal = normalize(input.NorWorld);

    // 빛의 방향만 남기기 위해 노멀라이즈
    float3 vLightDirection = normalize(LightDirection.xyz);

    // Ambient Light
    // 모든 물체에 적용하는 바탕 색 * 물체가 Ambient를 조절하는 색
    float4 Ambient = LightAmbient * MaterialAmbient;

    // Diffuse Light(Lambertian Lighting)
    float4 Diffuse = saturate(dot((float3) vLightDirection, Normal)* LightSpecular);
    
    /// Specular Light
    // Light Reflection : 점으로부터 물체에 반사된 빛
    float3 LightReflection = reflect(vLightDirection, Normal);
    // View : 시선벡터
    // 카메라가 물체를 바라보는 방향만 남기기 위해 노멀라이즈
    float3 View = normalize(input.PosWorld - EyePosition);
    // 빛과 시선벡터의 내적(각도)에 따라 점의 색상이 밝아지도록 함 
    float fRDotV = max(0.0f, dot(LightReflection, View));
    float4 Specular = pow(fRDotV, MaterialSpecularPower)* MaterialSpecularPower* LightSpecular;
    
    // Blinn Phong
    float3 HalfVector = normalize(-vLightDirection + View);
    float fHDotN = max(0.0f, dot(HalfVector, Normal));
    float4 Phong = pow(fHDotN, MaterialSpecularPower) * MaterialSpecular * LightSpecular;

    // 텍스처
    float4 Texture = txDiffuse.Sample(samLinear, input.Texcoord);

    // 구성요소를 곱하면 값이 조절되는 것이지 합쳐지지 않음
    // 따라서 곱셈이 아닌 덧셈으로 각 구성요소를 결합함
    finalColor = Ambient + Diffuse + Specular + Texture;


    return finalColor;
}