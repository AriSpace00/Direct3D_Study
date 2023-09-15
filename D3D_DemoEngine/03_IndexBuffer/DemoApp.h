#pragma once
#include <d3d11.h>
#include "../Common/GameApp.h"
#include <directxtk/SimpleMath.h>
#include <vector>

using namespace DirectX::SimpleMath;
using namespace DirectX;

class DemoApp :
    public GameApp
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

    // ������ ������������ �����ϴ� �ʼ� ��ü �������̽� (Depth Stencil View�� ������ ���� ������� ����)
    ID3D11Device* m_Device = nullptr;                       // ����̽�
    ID3D11DeviceContext* m_DeviceContext = nullptr;         // ��� ����̽� ���ؽ�Ʈ
    IDXGISwapChain* m_SwapChain = nullptr;                  // ����ü��
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;   // ������ Ÿ�� ��
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;   // ���� �� ó���� ���� ���� ���ٽ� ��

    // ������ ���������ο� �����ϴ� ��ü�� ����
    ID3D11VertexShader* m_VertexShader = nullptr;           // ���� ���̴�
    ID3D11PixelShader* m_PixelShader = nullptr;             // �ȼ� ���̴�
    ID3D11InputLayout* m_InputLayout = nullptr;             // �Է� ���̾ƿ�
    ID3D11Buffer* m_VertexBuffer = nullptr;                 // ���ؽ� ����
    UINT m_VertexBufferStride = 0;                          // ���ؽ� �ϳ��� ũ��
    UINT m_VertexBufferOffset = 0;                          // ���ؽ� ������ ������
    ID3D11Buffer* m_IndexBuffer = nullptr;                  // �ε��� ����
    int m_Indices = 0;                                      // �ε��� ����
    ID3D11Buffer* m_ConstantBuffer = nullptr;               // ��� ����

    Matrix m_WorldMatrix;                                   // ������ǥ�� �������� ��ȯ�� ���� ���
    Matrix m_WorldMatrix2;                                  // ������ǥ�� �������� ��ȯ�� ���� ���
    Matrix m_WorldMatrix3;                                  // ������ǥ�� �������� ��ȯ�� ���� ���
    Matrix m_ViewMatrix;                                    // ī�޶���ǥ�� �������� ��ȯ�� ���� ���
    Matrix m_ProjectionMatrix;                              // ������ġ��ǥ��(Normalized Device Coordinate) �������� ��ȯ�� ���� ���

    bool m_IsCubePropertiesWindow = false;
    bool m_IsCameraPropertiesWindow = false;

    float m_ParentWorldXTM;
    float m_ParentWorldYTM;
    float m_ParentWorldZTM;

    float m_ChildRelativeXTM1;
    float m_ChildRelativeYTM1;
    float m_ChildRelativeZTM1;

    float m_ChildRelativeXTM2;
    float m_ChildRelativeYTM2;
    float m_ChildRelativeZTM2;

    float m_CameraWorldXTM;
    float m_CameraWorldYTM;
    float m_CameraWorldZTM;

    float m_CameraFovYRadius;

    float m_CameraNear;
    float m_CameraFar;

    std::vector<Vector3> m_CubeMatrix;

    XMVECTOR m_Eye;
    XMVECTOR m_At;
    XMVECTOR m_Up;

    float m_InitColor[4] = { 0.8f, 1.0f, 1.0f, 1.0f };

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

