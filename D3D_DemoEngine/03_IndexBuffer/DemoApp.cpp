#include "DemoApp.h"
#include "../Common/Helper.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX::SimpleMath;

struct Vertex
{
    Vector3 position;   // ���� ��ġ ����
    Vector4 color;      // ���� ���� ����

    Vertex(float x, float y, float z) : position(x, y, z) {}
    Vertex(Vector3 position) : position(position) {}

    Vertex(Vector3 position, Vector4 color) : position(position), color(color) {}
};

struct ConstantBuffer
{
    Matrix WorldMatrix;
    Matrix ViewMatrix;
    Matrix ProjectionMatrix;
};

DemoApp::DemoApp(HINSTANCE hInstance)
    : GameApp(hInstance)
{
}

DemoApp::~DemoApp()
{
    UnInitScene();
    UnInitD3D();
}

bool DemoApp::Initialize(UINT width, UINT height)
{
    __super::Initialize(width, height);

    if (!InitD3D())
        return false;

    if (!InitScene())
        return false;

    return true;
}

void DemoApp::Update()
{
    __super::Update();
    m_WorldMatrix = XMMatrixRotationY(GameTimer::m_Instance->TotalTime());
}

void DemoApp::Render()
{
    Color initColor(0.8f, 1.0f, 1.0f, 1.0f);

    // ȭ�� ĥ�ϱ�
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, initColor);

    // Update Variables
    ConstantBuffer cb;
    cb.WorldMatrix = XMMatrixTranspose(m_WorldMatrix);
    cb.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

    // Draw �迭 �Լ��� ȣ���ϱ� ���� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �̾ �׸� ��� ����
    m_DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
    m_DeviceContext->IASetInputLayout(m_InputLayout);
    m_DeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
    m_DeviceContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);

    // �ﰢ�� ����ϱ�
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

    // ���� �� ���ۿ��� ������ ������ ����Ʈ ���ۿ�(��ũ��) ����
    m_SwapChain->Present(0, 0);
}

bool DemoApp::InitD3D()
{
    // �����
    HRESULT hr = 0;

    // ����ü�� �Ӽ� ���� ����ü ����
    DXGI_SWAP_CHAIN_DESC swapDesc = {};

    swapDesc.BufferCount = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hWnd;                         // ����ü�� ����� â �ڵ� ��
    swapDesc.Windowed = true;                               // â ��� ���� ����
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

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

    // 4. ���� Ÿ�� �� ���� (�� ���۸� �̿��ϴ� ���� Ÿ�� ��)
    ID3D11Texture2D* BackBufferTexture = nullptr;

    HR_T(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture));
    HR_T(m_Device->CreateRenderTargetView(BackBufferTexture, NULL, &m_RenderTargetView));   // �ؽ�ó�� ���� ���� ����
    SAFE_RELEASE(BackBufferTexture);                                                              // �ܺ� ���� ī��Ʈ�� ���ҽ�Ų��

    // ���� Ÿ���� ���� ��� ���������ο� ���ε�
    // Flip Mode�� �ƴ� ������ ���� �ѹ��� �����ϸ� ��
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);

    // ����Ʈ ����
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)m_ClientWidth;
    viewport.Height = (float)m_ClientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_DeviceContext->RSSetViewports(1, &viewport);

    return true;
}

void DemoApp::UnInitD3D()
{
    SAFE_RELEASE(m_Device);
    SAFE_RELEASE(m_DeviceContext);
    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_RenderTargetView);
}

bool DemoApp::InitScene()
{
    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr; // ������ ���� �޽����� ����� ����

    // 1. Render() ���� ���������ο� ���ε��� ���ؽ� ���� �� ���� ���� �غ�
    Vertex vertices[] =
    {
        Vertex(Vector3(-1.0f, 1.0f, -1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),       
        Vertex(Vector3(1.0f, 1.0f, -1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),       
        Vertex(Vector3(1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),       
        Vertex(Vector3(-1.0f, 1.0f, 1.0f), Vector4(0.1f, 0.1f, 1.0f, 1.0f)),
        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(1.0f, -1.0f, -1.0f), Vector4(0.1f, 0.1f, 1.0f, 1.0f)),
        Vertex(Vector3(1.0f, -1.0f, 1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(-1.0f, -1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f))
    };

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // ���ؽ� ���� ����
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;
    HR_T(hr = m_Device->CreateBuffer(&bd, &vbData, &m_VertexBuffer));

    // ���ؽ� ���� ����
    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;

    // 2. Render() ���� ���������ο� ���ε��� InputLayout ����
    // Input Layout�� ���ؽ� ���̴��� �Է¹��� �������� ������ �����Ѵ�.
    // SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate
    ID3DBlob* vertexShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Vertex Shader ����", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }

    // ���ؽ� ���̴��� �������� ������ �� Input Layout ����
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    HR_T(hr = m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout));

    // 3. Render() ���� ���������ο� ���ε��� ���ؽ� ���̴� ����
    HR_T(m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader));
    SAFE_RELEASE(vertexShaderBuffer);

    // 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����
    WORD indices[] =
    {
        3,1,0, 2,1,3,
        0,5,4, 1,5,0,
        3,4,7, 0,4,3,
        1,6,5, 2,6,1,
        2,7,6, 3,7,2,
        6,4,5, 7,4,6,
    };
    m_Indices = ARRAYSIZE(indices);         // �ε��� ���� ����
    bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;
    HR_T(m_Device->CreateBuffer(&bd, &ibData, &m_IndexBuffer));

    // 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
    ID3DBlob* pixelShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Pixel Shader ���� ����", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }
    HR_T(m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader));
    SAFE_RELEASE(pixelShaderBuffer);

    // 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer));

    // ���̴��� ������ ������ ����
    m_WorldMatrix = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_ViewMatrix = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);
    return true;

    return true;
}

void DemoApp::UnInitScene()
{
    SAFE_RELEASE(m_VertexBuffer);
    SAFE_RELEASE(m_IndexBuffer);
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_InputLayout);
    SAFE_RELEASE(m_ConstantBuffer);
}
