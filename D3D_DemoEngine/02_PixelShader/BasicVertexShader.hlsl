#include "Shared.fxh"
// ���� ���̴�.

PS_INPUT main(float4 pos : POSITION, float4 color : COLOR)
{
    PS_INPUT output;
    output.Pos = pos;
    output.color = color;
    return output;
}