#pragma once
#include <d3d11.h>
#include "../Common/GameApp.h"
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
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;   // 깊이 값 처리를 위한 뎁스 스텐실 뷰

    // 렌더링 파이프라인에 적용하는 객체와 정보
    ID3D11VertexShader* m_VertexShader = nullptr;           // 정점 셰이더
    ID3D11PixelShader* m_PixelShader = nullptr;             // 픽셀 셰이더
    ID3D11PixelShader* m_PixelShaderSolid = nullptr;        // 픽셀 셰이더 라이트 표시용
    ID3D11InputLayout* m_InputLayout = nullptr;             // 입력 레이아웃
    ID3D11Buffer* m_VertexBuffer = nullptr;                 // 버텍스 버퍼
    UINT m_VertexBufferStride = 0;                          // 버텍스 하나의 크기
    UINT m_VertexBufferOffset = 0;                          // 버텍스 버퍼의 오프셋
    ID3D11Buffer* m_IndexBuffer = nullptr;                  // 인덱스 버퍼
    int m_Indices = 0;                                      // 인덱스 개수
    ID3D11Buffer* m_ConstantBuffer = nullptr;               // 상수 버퍼
    ID3D11ShaderResourceView* m_TextureRV = nullptr;        // 텍스처 리소스 뷰
    ID3D11SamplerState* m_SamplerLinear = nullptr;          // 샘플러 상태

    Vector4	m_MeshColor = { 0.7f, 0.7f, 0.7f, 1.0f };

    Matrix m_WorldMatrix;                                   // 월드좌표계 공간으로 변환을 위한 행렬
    Matrix m_ViewMatrix;                                    // 카메라좌표계(뷰좌표계) 공간으로 변환을 위한 행렬
    Matrix m_ProjectionMatrix;                              // 단위장치좌표계(Normalized Device Coordinate) 공간으로 변환을 위한 행렬

    bool m_IsCubePropertiesWindow = false;
    bool m_IsCameraPropertiesWindow = false;

    float m_CubeWorldXTM;
    float m_CubeWorldYTM;
    float m_CubeWorldZTM;
    float m_CubeXYaw = 0.0f;

    float m_CameraWorldXTM;
    float m_CameraWorldYTM;
    float m_CameraWorldZTM;

    float m_CameraFovYRadius;

    float m_CameraNear;
    float m_CameraFar;

    Vector3 m_CubeMatrix;


    XMVECTOR m_Eye;     // 카메라 좌표
    XMVECTOR m_At;      // 카메라가 바라보는 방향
    XMVECTOR m_Up;      // 카메라 상단의 방향

    float m_InitColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    // 라이트 색상, 라이트 위치, 최종 계산된 라이트 방향
    XMFLOAT4 m_LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    XMFLOAT4 m_LightDir = {0.0f, 0.0f, -1.0f, 1.0f};
    XMFLOAT4 m_LightDirsEvaluated = {};

    bool m_IsCubeRotation;

public:
    virtual bool Initialize(UINT width, UINT height);
    virtual void Update();
    virtual void Render();

    bool InitD3D();
    void UnInitD3D();

    bool InitScene(); // 쉐이더, 버텍스, 인덱스
    void UnInitScene();

    bool InitImGUI();
    void UnInitImGUI();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

