#include "Shared.fxh"
// ���� ���̴�.

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Nor = mul(float4(input.Nor, 1), World).xyz;
    output.Tex = input.Tex;

    return output;

}