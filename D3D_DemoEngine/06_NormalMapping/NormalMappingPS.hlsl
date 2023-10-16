#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    // 물체로부터 뻗어나가는 노멀 방향만 남기기 위해 노멀라이즈
    float3 Normal = normalize(input.NorWorld);

    // 빛의 방향만 남기기 위해 노멀라이즈
    float3 vLightDirection = normalize(LightDirection.xyz);

    // View : 시선벡터
    // 내 시선 끝의 물체 픽셀로부터 카메라를 바라보는 방향만 남기기 위해 노멀라이즈
    float3 View = normalize(EyePosition - input.PosWorld);

    // Ambient Light
    // 모든 물체에 적용하는 바탕 색 * 물체가 Ambient를 조절하는 색
    float4 Ambient = LightAmbient * MaterialAmbient;

    // 텍스처
    float4 Texture = txDiffuse.Sample(samLinear, input.Texcoord);

    // Diffuse Light(Lambertian Lighting)
    float4 DiffuseColor = LightDiffuse * MaterialDiffuse * Texture;
    // 방향이 다른 두 벡터(Normal, vLightDirection)을 내적하면 음수가 도출된다.
    // 따라서 부호를 맞춰주기 위해 vLightDirection에 -를 곱해준다.
    float4 Diffuse = dot(Normal, -vLightDirection) * DiffuseColor;

    // Specular Light
    float4 Specular;
    if (UseBlinnPhong)
    {
        // Blinn Phong
        float3 HalfVector = normalize(-vLightDirection + View);
        float fHDotN = max(0.0f, dot(HalfVector, Normal));
        float4 BlinnPhong = pow(fHDotN, MaterialSpecularPower) * MaterialSpecular * LightSpecular;
        Specular = BlinnPhong;
    }
    else
    {
        // Phong
        // Light Reflection : 점으로부터 물체에 반사된 빛
        float3 LightReflection = reflect(vLightDirection, Normal);
        // 빛과 시선벡터의 내적(각도)에 따라 점의 색상이 밝아지도록 함 
        float fRDotV = max(0.0f, dot(LightReflection, View));
        float4 Phong = pow(fRDotV, MaterialSpecularPower) * saturate(MaterialSpecular) * saturate(LightSpecular);
        Specular = Phong;
    }

    // 구성요소를 곱하면 값이 조절되는 것이지 합쳐지지 않음
    // 따라서 곱셈이 아닌 덧셈으로 각 구성요소를 결합함
    finalColor = saturate(Ambient + Diffuse + Specular);

    return finalColor;
}