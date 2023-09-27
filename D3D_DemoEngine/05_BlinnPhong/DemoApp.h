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

    // ������ ������������ �����ϴ� �ʼ� ��ü �������̽�
    ID3D11Device* m_Device = nullptr;                       // ����̽�
    ID3D11DeviceContext* m_DeviceContext = nullptr;         // ��� ����̽� ���ؽ�Ʈ
    IDXGISwapChain* m_SwapChain = nullptr;                  // ����ü��
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;   // ������ Ÿ�� ��
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;   // ���� �� ó���� ���� ���� ���ٽ� ��

    // ������ ���������ο� �����ϴ� ��ü�� ����
    ID3D11VertexShader* m_VertexShader = nullptr;           // ���� ���̴�
    ID3D11PixelShader* m_PixelShader = nullptr;             // �ȼ� ���̴�
    ID3D11PixelShader* m_PixelShaderSolid = nullptr;        // �ȼ� ���̴� ����Ʈ ǥ�ÿ�
    ID3D11InputLayout* m_InputLayout = nullptr;             // �Է� ���̾ƿ�
    ID3D11Buffer* m_VertexBuffer = nullptr;                 // ���ؽ� ����
    UINT m_VertexBufferStride = 0;                          // ���ؽ� �ϳ��� ũ��
    UINT m_VertexBufferOffset = 0;                          // ���ؽ� ������ ������
    ID3D11Buffer* m_IndexBuffer = nullptr;                  // �ε��� ����
    int m_Indices = 0;                                      // �ε��� ����
    ID3D11ShaderResourceView* m_TextureRV = nullptr;        // �ؽ�ó ���ҽ� ��
    ID3D11SamplerState* m_SamplerLinear = nullptr;          // ���÷� ����

    ID3D11Buffer* m_CBMaterial = nullptr;                   // ��� ����: ��ȯ���
    ID3D11Buffer* m_CBTransform = nullptr;                  // ��� ����: ��ȯ���
    ID3D11Buffer* m_CBDirectionalLight = nullptr;           // ��� ����: ���Ɽ
    ID3D11Buffer* m_CBRotation = nullptr;                   // ��� ����: ȸ��

    Matrix m_WorldMatrix;                                   // ������ǥ�� �������� ��ȯ�� ���� ���
    Matrix m_ViewMatrix;                                    // ī�޶���ǥ��(����ǥ��) �������� ��ȯ�� ���� ���
    Matrix m_ProjectionMatrix;                              // ������ġ��ǥ��(Normalized Device Coordinate) �������� ��ȯ�� ���� ���

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

    bool InitScene(); // ���̴�, ���ؽ�, �ε���
    void UnInitScene();

    bool InitImGUI();
    void UnInitImGUI();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

