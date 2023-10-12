// 픽셀 셰이더
float4 main(float4 input : SV_POSITION) : SV_TARGET
{
    // 정점 버퍼에 바인딩된 정점 정보(위치)에서 출력할 색상 반환
    return float4(0.8f, 0.8f, 1.0f, 1.0f); 
}