#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    // ��ü�κ��� ������� ��� ���⸸ ����� ���� ��ֶ�����
    float3 Normal = normalize(input.NorWorld);

    // �븻�� ������ ���� ź��Ʈ����, ����ź��Ʈ���� ���⸸ ����� ���� ��ֶ�����
    float3 Tangent = normalize(input.TanWorld);
    float3 BiTangent = cross(Normal, Tangent);

    if (UseNormalMap)
    {
        float3 NormalTangentSpace = txNormal.Sample(samLinear, input.Texcoord).rgb * 2.0 - 1.0f;
        float3x3 WorldTransform = float3x3(Tangent, BiTangent, Normal);
        Normal = mul(NormalTangentSpace, WorldTransform);
        Normal = normalize(Normal);
    }

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

    // Specular Map
    // Specular Map�� �̹������� ��ü�� ���� ������ ������ ������
    float SpecularIntensity = 1;
    if (UseSpecularMap)
    {
        SpecularIntensity = txSpecular.Sample(samLinear, input.Texcoord).r;
    }

    // Specular Light
    // Blinn Phong
    float4 Specular;
    float3 HalfVector = normalize(-vLightDirection + View);
    float fHDotN = max(0.0f, dot(HalfVector, Normal));
    float4 BlinnPhong = pow(fHDotN, MaterialSpecularPower) * MaterialSpecular * LightSpecular * SpecularIntensity;
    Specular = BlinnPhong;

    // ������Ҹ� ���ϸ� ���� �����Ǵ� ������ �������� ����
    // ���� ������ �ƴ� �������� �� ������Ҹ� ������
    finalColor = saturate(Ambient + Diffuse + Specular);

    return finalColor;
}