#pragma once
#include <d3d11.h>
#include "../Common/GameApp.h"
//#include <d3d11.h>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

class DemoApp :
    public GameApp
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

    // 렌더링 파이프라인을 구성하는 필수 객체 인터페이스 (Depth Stencil View도 있지만 아직 사용하지 않음)
    ID3D11Device* m_Device = nullptr;                       // 디바이스
    ID3D11DeviceContext* m_DeviceContext = nullptr;         // 즉시 디바이스 컨텍스트
    IDXGISwapChain* m_SwapChain = nullptr;                  // 스왑체인
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;   // 렌더링 타겟 뷰

    // 렌더링 파이프라인에 적용하는 객체와 정보
    ID3D11VertexShader* m_VertexShader = nullptr;           // 정점 셰이더
    ID3D11PixelShader* m_PixelShader = nullptr;             // 픽셀 셰이더
    ID3D11InputLayout* m_InputLayout = nullptr;             // 입력 레이아웃
    ID3D11Buffer* m_VertexBuffer = nullptr;                 // 버텍스 버퍼
    UINT m_VertexBufferStride = 0;                          // 버텍스 하나의 크기
    UINT m_VertexBufferOffset = 0;                          // 버텍스 버퍼의 오프셋
    ID3D11Buffer* m_IndexBuffer = nullptr;                  // 인덱스 버퍼
    int m_Indices = 0;                                      // 인덱스 개수
    ID3D11Buffer* m_ConstantBuffer = nullptr;               // 상수 버퍼

    Matrix m_WorldMatrix;                                   // 월드좌표게 공간으로 변환을 위한 행렬
    Matrix m_ViewMatrix;                                    // 카메라좌표계 공간으로 변환을 위한 행렬
    Matrix m_ProjectionMatrix;                              // 단위장치좌표계(Normalized Device Coordinate) 공간으로 변환을 위한 행렬

    virtual bool Initialize(UINT width, UINT height);
    virtual void Update();
    virtual void Render();

    bool InitD3D();
    void UnInitD3D();

    bool InitScene(); // 쉐이더, 버텍스, 인덱스
    void UnInitScene();
};

