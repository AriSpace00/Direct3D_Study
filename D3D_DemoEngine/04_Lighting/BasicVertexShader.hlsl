#include "Shared.fxh"
// ¡§¡° ºŒ¿Ã¥ı.

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.pos = mul(input.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.normal = mul(float4(input.normal, 1), World).xyz;

    return output;

}