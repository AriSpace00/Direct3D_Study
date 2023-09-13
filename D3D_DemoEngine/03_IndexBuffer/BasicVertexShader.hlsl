#include "Shared.fxh"
// ¡§¡° ºŒ¿Ã¥ı.

PS_INPUT main(float4 pos : POSITION, float4 color : COLOR)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.pos = mul(pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.color = color;
    return output;
}