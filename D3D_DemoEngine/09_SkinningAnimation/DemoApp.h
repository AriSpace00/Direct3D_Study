#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>
#include "../Common/GameApp.h"
#include "../Common/Model.h"
#include "../Common/Mesh.h"
#include "../Common/Material.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct CB_DirectionalLight
{
    Vector3 Direction = { 0.0f,0.0f,1.0f };
    float DL_pad0;
    Vector4 Ambient = { 0.1f,0.1f,0.1f,0.1f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    Vector3 EyePosition;
    float DL_pad1;
};

static_assert((sizeof(CB_DirectionalLight) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

class DemoApp :
    public GameApp
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

    // ������ ������������ �����ϴ� �ʼ� ��ü �������̽�
    ID3D11Device* m_Device = nullptr;                       // ����̽�
    ID3D11DeviceContext* m_DeviceContext = nullptr;         // ��� ����̽� ���ؽ�Ʈ
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;   // ������ Ÿ�� ��
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;   // ���� �� ó���� ���� ���� ���ٽ� ��
    IDXGISwapChain* m_SwapChain = nullptr;                  // ����ü��

    // ������ ���������ο� �����ϴ� ��ü�� ����
    ID3D11VertexShader* m_VertexShader = nullptr;           // ���� ���̴�
    ID3D11PixelShader* m_PixelShader = nullptr;             // �ȼ� ���̴�
    ID3D11InputLayout* m_InputLayout = nullptr;             // �Է� ���̾ƿ�
    ID3D11SamplerState* m_SamplerLinear = nullptr;          // ���÷� ����

    ID3D11Buffer* m_CBDirectionalLight = nullptr;           // ��� ����: ���Ɽ

    UINT m_VertexBufferStride = 0;						    // ���ؽ� �ϳ��� ũ��.
    UINT m_VertexBufferOffset = 0;						    // ���ؽ� ������ ������.

    //Matrix m_WorldMatrix;                                 // ������ǥ�� �������� ��ȯ�� ���� ���
    Matrix m_ViewMatrix;                                    // ī�޶���ǥ��(����ǥ��) �������� ��ȯ�� ���� ���
    Matrix m_ProjectionMatrix;                              // ������ġ��ǥ��(Normalized Device Coordinate) �������� ��ȯ�� ���� ���

    CB_DirectionalLight m_Light;
    float m_MeshScale = 1.0f;

    vector<Mesh> m_Meshes;
    vector<Material> m_Materials;

    Model* m_Model;

    const float m_ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };

    float m_CubeRotationX = 0.0f;
    float m_CubeRotationY = 0.0f;

    float m_CameraFovYRadius;
    float m_CameraNear;
    float m_CameraFar;

    XMVECTOR m_Eye;
    XMVECTOR m_At;
    XMVECTOR m_Up;

    std::wstring m_FBXFileName;

    int m_FBXModelIndex = 7;

    LARGE_INTEGER m_Frequency;
    LARGE_INTEGER m_PrevTime, m_CurrentTime;
    double m_DeltaTime;

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

