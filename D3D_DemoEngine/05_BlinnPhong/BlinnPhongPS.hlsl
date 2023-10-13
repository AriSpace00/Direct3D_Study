#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    // ��ü�κ��� ������� ��� ���⸸ ����� ���� ��ֶ�����
    float3 Normal = normalize(input.NorWorld);

    // ���� ���⸸ ����� ���� ��ֶ�����
    float3 vLightDirection = normalize(LightDirection.xyz);

    // Ambient Light
    // ��� ��ü�� �����ϴ� ���� �� * ��ü�� Ambient�� �����ϴ� ��
    float4 Ambient = LightAmbient * MaterialAmbient;

    // Diffuse Light(Lambertian Lighting)
    float4 Diffuse = saturate(dot((float3) vLightDirection, Normal)* LightSpecular);
    
    /// Specular Light
    // Light Reflection : �����κ��� ��ü�� �ݻ�� ��
    float3 LightReflection = reflect(vLightDirection, Normal);
    // View : �ü�����
    // ī�޶� ��ü�� �ٶ󺸴� ���⸸ ����� ���� ��ֶ�����
    float3 View = normalize(input.PosWorld - EyePosition);
    // ���� �ü������� ����(����)�� ���� ���� ������ ��������� �� 
    float fRDotV = max(0.0f, dot(LightReflection, View));
    float4 Specular = pow(fRDotV, MaterialSpecularPower)* MaterialSpecularPower* LightSpecular;
    
    // Blinn Phong
    float3 HalfVector = normalize(-vLightDirection + View);
    float fHDotN = max(0.0f, dot(HalfVector, Normal));
    float4 Phong = pow(fHDotN, MaterialSpecularPower) * MaterialSpecular * LightSpecular;

    // �ؽ�ó
    float4 Texture = txDiffuse.Sample(samLinear, input.Texcoord);

    // ������Ҹ� ���ϸ� ���� �����Ǵ� ������ �������� ����
    // ���� ������ �ƴ� �������� �� ������Ҹ� ������
    finalColor = Ambient + Diffuse + Specular + Texture;


    return finalColor;
}