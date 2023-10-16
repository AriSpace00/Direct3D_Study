#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    // ��ü�κ��� ������� ��� ���⸸ ����� ���� ��ֶ�����
    float3 Normal = normalize(input.NorWorld);

    // ���� ���⸸ ����� ���� ��ֶ�����
    float3 vLightDirection = normalize(LightDirection.xyz);

    // View : �ü�����
    // �� �ü� ���� ��ü �ȼ��κ��� ī�޶� �ٶ󺸴� ���⸸ ����� ���� ��ֶ�����
    float3 View = normalize(EyePosition - input.PosWorld);

    // Ambient Light
    // ��� ��ü�� �����ϴ� ���� �� * ��ü�� Ambient�� �����ϴ� ��
    float4 Ambient = LightAmbient * MaterialAmbient;

    // �ؽ�ó
    float4 Texture = txDiffuse.Sample(samLinear, input.Texcoord);

    // Diffuse Light(Lambertian Lighting)
    float4 DiffuseColor = LightDiffuse * MaterialDiffuse * Texture;
    // ������ �ٸ� �� ����(Normal, vLightDirection)�� �����ϸ� ������ ����ȴ�.
    // ���� ��ȣ�� �����ֱ� ���� vLightDirection�� -�� �����ش�.
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
        // Light Reflection : �����κ��� ��ü�� �ݻ�� ��
        float3 LightReflection = reflect(vLightDirection, Normal);
        // ���� �ü������� ����(����)�� ���� ���� ������ ��������� �� 
        float fRDotV = max(0.0f, dot(LightReflection, View));
        float4 Phong = pow(fRDotV, MaterialSpecularPower) * saturate(MaterialSpecular) * saturate(LightSpecular);
        Specular = Phong;
    }

    // ������Ҹ� ���ϸ� ���� �����Ǵ� ������ �������� ����
    // ���� ������ �ƴ� �������� �� ������Ҹ� ������
    finalColor = saturate(Ambient + Diffuse + Specular);

    return finalColor;
}