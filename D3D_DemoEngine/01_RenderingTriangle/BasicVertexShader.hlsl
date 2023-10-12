// 정점 셰이더
float4 main(float4 pos : POSITION) : SV_POSITION
{
    // 정점 버퍼에 바인딩된 정점 정보(위치) 반환
    return pos; 
}