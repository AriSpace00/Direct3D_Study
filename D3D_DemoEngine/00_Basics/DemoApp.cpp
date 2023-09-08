#include "DemoApp.h"
#include "../Common/Helper.h"
#include <directxtk/SimpleMath.h>

#pragma comment(lib, "d3d11.lib")

using namespace DirectX::SimpleMath;

#define USE_FLIPMODE 0              // ��� �޼����� ���ַ��� Flip ��带 ����Ѵ�.

DemoApp::DemoApp(HINSTANCE hInstance)
    : GameApp(hInstance)
{
}

DemoApp::~DemoApp()
{
    UnInitD3D();
}

bool DemoApp::Initialize(UINT width, UINT height)
{
    __super::Initialize(width, height);

    if (!InitD3D())
        return false;

    return true;
}

void DemoApp::Update()
{
}

void DemoApp::Render()
{
#if USE_FLIPMODE == 1
    // Flip Mode������ �� ������ ���� �ʿ�
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);
#endif
    Color color(0.8f, 1.0f, 1.0f, 1.0f);

    // ȭ�� ĥ�ϱ�
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);

    // ����ü�� ��ü
    m_SwapChain->Present(0, 0);
}

bool DemoApp::InitD3D()
{
    // �����
    HRESULT hr = 0;

    // ����ü�� �Ӽ� ���� ����ü ����
    DXGI_SWAP_CHAIN_DESC swapDesc = {};

#if USE_FLIPMODE == 1
    swapDesc.BufferCount = 2;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
    swapDesc.BufferCount = 1;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;    // �⺻�� 0
#endif
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hWnd;                         // ����ü�� ����� â �ڵ� ��
    swapDesc.Windowed = true;                               // â ��� ���� ����
    swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    // �� ����(�ؽ�ó)�� ����/���� ũ�� ����
    swapDesc.BufferDesc.Width = m_ClientWidth;
    swapDesc.BufferDesc.Height = m_ClientHeight;

    // ȭ�� �ֻ��� ����
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;

    // ���ø� ���� ����
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // 1. ��ġ ����, 2. ����ü�� ����, 3. ��ġ ���ؽ�Ʈ ����
    HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext));

    // 4. ���� Ÿ�� �� ����
    ID3D11Texture2D* BackBufferTexture = nullptr;

    HR_T(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture));
    HR_T(m_Device->CreateRenderTargetView(BackBufferTexture, NULL, &m_RenderTargetView));   // �ؽ�ó�� ���� ���� ����
    SAFE_RELEASE(BackBufferTexture);                                                              // �ܺ� ���� ī��Ʈ�� ���ҽ�Ų��


#if USE_FLIPMODE == 0
    // ���� Ÿ���� ���� ��� ���������ο� ���ε�
    // Flip Mode�� �ƴ� ������ ���� �ѹ��� �����ϸ� ��
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);
#endif
    return true;
}

void DemoApp::UnInitD3D()
{
    SAFE_RELEASE(m_RenderTargetView);
    SAFE_RELEASE(m_DeviceContext);
    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_Device);
}
