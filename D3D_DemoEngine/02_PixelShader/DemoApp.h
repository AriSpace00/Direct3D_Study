#pragma once
#include <d3d11.h>
#include "../Common/GameApp.h"

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

    // ������ ���������ο� �����ϴ� ��ü�� ����
    ID3D11VertexShader* m_VertexShader = nullptr;           // ���� ���̴�
    ID3D11PixelShader* m_PixelShader = nullptr;             // �ȼ� ���̴�
    ID3D11InputLayout* m_InputLayout = nullptr;             // �Է� ���̾ƿ�
    ID3D11Buffer* m_VertexBuffer = nullptr;                 // ���ؽ� ����
    UINT m_VertexBufferStride = 0;                          // ���ؽ� �ϳ��� ũ��
    UINT m_VertexBufferOffset = 0;                          // ���ؽ� ������ ������
    UINT m_VertexCount = 0;                                 // ���ؽ� ����

    virtual bool Initialize(UINT width, UINT height);
    virtual void Update();
    virtual void Render();

    bool InitD3D();
    void UnInitD3D();

    bool InitScene(); // ���̴�, ���ؽ�, �ε���
    void UnInitScene();
};

