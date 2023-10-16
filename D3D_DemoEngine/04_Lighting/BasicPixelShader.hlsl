#include "Shared.fxh"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    // ���� ���⸸�� ����� ���� normalize ����
    float3 vLightDirection = normalize(vLightDir.xyz); 
    // ���� ����� �ȼ��� ���� ���͸� �����ϰ�, ���� ����� ������ �� 0-1 ���̷� Ŭ������
    finalColor = saturate(dot((float3)vLightDirection, input.Nor) * vLightColor);
    // ���İ� 1�� �����Ͽ� ������ ����
    finalColor.a = 1;
    // �ؽ�ó ������ ���÷� �Ӽ��� ���� ����
    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);

    finalColor *= textureColor;

    return finalColor;
}