#include "Shared.fxh"
// 정점 셰이더.

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    // 정점의 로컬 공간에서 계산된 Normal 벡터를 월드 공간으로 변환
    output.Nor = mul(float4(input.Nor, 0), World).xyz;

    output.Tex = input.Tex;

    return output;

}