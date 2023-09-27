#pragma once
#include <d3d11.h>
#include "../Common/GameApp.h"
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Vertex
{
    Vector3 Pos;
    Vector3 Nor;
    Vector2 Tex;
};

struct CB_Transform
{
    Matrix WorldMatrix;
    Matrix ViewMatrix;
    Matrix ProjectionMatrix;
};

struct CB_DirectionalLight
{
    Vector3 Direction = { 0.0f,0.0f,1.0f };
    Vector4 Ambient = { 0.1f,0.1f,0.1f,0.1f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    Vector3 EyePosition;
    float pad0;
    bool UseBlinnPhong = false;
};

static_assert((sizeof(CB_DirectionalLight) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

struct CB_Material
{
    Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    float SpecularPower = 2000;
    Vector3 dummy;
};

class DemoApp :
    public GameApp
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

    // 렌더링 파이프라인을 구성하는 필수 객체 인터페이스
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
    ID3D11ShaderResourceView* m_TextureRV = nullptr;        // 텍스처 리소스 뷰
    ID3D11SamplerState* m_SamplerLinear = nullptr;          // 샘플러 상태

    ID3D11Buffer* m_CBMaterial = nullptr;                   // 상수 버퍼: 변환행렬
    ID3D11Buffer* m_CBTransform = nullptr;                  // 상수 버퍼: 변환행렬
    ID3D11Buffer* m_CBDirectionalLight = nullptr;           // 상수 버퍼: 방향광
    ID3D11Buffer* m_CBRotation = nullptr;                   // 상수 버퍼: 회전

    Matrix m_WorldMatrix;                                   // 월드좌표계 공간으로 변환을 위한 행렬
    Matrix m_ViewMatrix;                                    // 카메라좌표계(뷰좌표계) 공간으로 변환을 위한 행렬
    Matrix m_ProjectionMatrix;                              // 단위장치좌표계(Normalized Device Coordinate) 공간으로 변환을 위한 행렬

    CB_Transform m_Transform;
    CB_DirectionalLight m_Material;
    CB_DirectionalLight m_Light;
    float m_MeshScale = 100.0f;

    Vector3 m_ClearColor = { 0.0f, 0.0f, 0.0f };
    Vector2 m_Rotation = { 0.0f, 0.0f };
    Vector3 m_CameraPos = { 0.0f, 0.0f, -1000.0f };

    bool m_IsCubePropertiesWindow = false;
    bool m_IsCameraPropertiesWindow = false;

    float m_CubeYaw = 1.0f;

    float m_CameraFovYRadius;
    float m_CameraNear;
    float m_CameraFar;

    XMVECTOR m_Eye;
    XMVECTOR m_At;
    XMVECTOR m_Up;

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

