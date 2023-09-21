#include "DemoApp.h"
#include "../Common/Helper.h"
#include <d3dcompiler.h>
#include <cmath>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX::SimpleMath;

struct Vertex
{
    Vector3 Pos;
    Vector3 Normal;
};

struct ConstantBuffer
{
    Matrix WorldMatrix;
    Matrix ViewMatrix;
    Matrix ProjectionMatrix;

    Vector4 vLightDir[2];
    Vector4 vLightColor[2];
    Vector4 vOutputColor;
};

DemoApp::DemoApp(HINSTANCE hInstance)
    : GameApp(hInstance)
    , m_CameraNear(0.01f)
    , m_CameraFar(100.0f)
    , m_CameraFovYRadius(90.0f)
{

}

DemoApp::~DemoApp()
{
    UnInitImGUI();
    UnInitScene();
    UnInitD3D();
}

bool DemoApp::Initialize(UINT width, UINT height)
{
    __super::Initialize(width, height);

    if (!InitD3D())
        return false;

    if (!InitImGUI())
        return false;

    if (!InitScene())
        return false;

    return true;
}

void DemoApp::Update()
{
    __super::Update();

    float t = GameTimer::m_Instance->TotalTime();

    // ù��° ť��: Y������ ����
    Matrix mSpin = XMMatrixRotationY(t);
    Matrix mTranslate = XMMatrixTranslation(m_CubeMatrix[0].x + m_ParentWorldXTM, m_CubeMatrix[0].y + m_ParentWorldYTM, m_CubeMatrix[0].z + m_ParentWorldZTM);
    m_WorldMatrix = mSpin * mTranslate;

    // Rotate the second light around the origin
    m_LightDirsEvaluated[0] = m_InitialLightDirs[0];
    /*XMMATRIX mRotate = XMMatrixRotationY(-2.0f * t);
    XMVECTOR vLightDir = XMLoadFloat4(&m_InitialLightDirs[1]);
    vLightDir = XMVector3Transform(vLightDir, mRotate);
    XMStoreFloat4(&m_LightDirsEvaluated[1], vLightDir);*/

    // �ι�° ť�� : ù��° ť�긦 �߽����� Y������ ����
    Matrix mSpin1 = XMMatrixRotationX(-t);
    Matrix mOrbit1 = XMMatrixRotationY(-t * 1.5f);
    Matrix mTranslate1 = XMMatrixTranslation(m_CubeMatrix[1].x + m_ChildRelativeXTM1, m_CubeMatrix[1].y + m_ChildRelativeYTM1, m_CubeMatrix[1].z + m_ChildRelativeZTM1);
    Matrix mScale1 = XMMatrixScaling(0.3f, 0.3f, 0.3f);

    m_WorldMatrix2 = mScale1 * mSpin1 * mTranslate1 * mOrbit1 * m_WorldMatrix; // ������ ���� -> R(���ڸ� Yȸ��) -> �������� �̵� -> �˵� ȸ��

    // ����° ť�� : �ι�° ť�긦 �߽����� Y������ ����
    Matrix mSpin2 = XMMatrixRotationZ(-t);
    Matrix mOrbit2 = XMMatrixRotationY(-t * 8.0f);
    Matrix mTranslate2 = XMMatrixTranslation(m_CubeMatrix[2].x + m_ChildRelativeXTM2, m_CubeMatrix[2].y + m_ChildRelativeYTM2, m_CubeMatrix[2].z + m_ChildRelativeZTM2);
    Matrix mScale2 = XMMatrixScaling(0.5f, 0.5f, 0.5f);

    m_WorldMatrix3 = mScale2 * mSpin2 * mTranslate2 * mOrbit2 * m_WorldMatrix2; // ������ ���� -> R(���ڸ� Yȸ��) -> �������� �̵� -> �˵� ȸ��
}

void DemoApp::Render()
{
    // ȭ�� ĥ�ϱ�
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_InitColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // ImGui
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;           // Ű���� �Է°� �ޱ�
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;            // �����е� �Է°� �ޱ�

        // ImGUI ������ ����
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. ť�� ���� ������
        ImGui::SetNextWindowSize(ImVec2(250, 300));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        {
            ImGui::Begin("Cube Properties");

            ImGui::Text("Parent Mesh World Transform");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##px", &m_ParentWorldXTM, -10.0f, 10.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##py", &m_ParentWorldYTM, -10.0f, 10.0f);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##pz", &m_ParentWorldZTM, -10.0f, 10.0f);

            ImGui::Text("Child1 World Transform");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##cx1", &m_ChildRelativeXTM1, -10.0f, 10.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cy1", &m_ChildRelativeYTM1, -10.0f, 10.0f);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##cz1", &m_ChildRelativeZTM1, -10.0f, 10.0f);

            ImGui::Text("Child2 World Transform");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##cx2", &m_ChildRelativeXTM2, -10.0f, 10.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cy2", &m_ChildRelativeYTM2, -10.0f, 10.0f);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##cz3", &m_ChildRelativeZTM2, -10.0f, 10.0f);

            ImGui::End();
        }

        // 2. ī�޶� ���� ������
        ImGui::SetNextWindowSize(ImVec2(250, 300));
        ImGui::SetNextWindowPos(ImVec2(0, 300));
        {
            ImGui::Begin("Camera Properties");

            ImGui::Text("Camera World Transform");
            float x = XMVectorGetX(m_Eye);
            float y = XMVectorGetY(m_Eye);
            float z = XMVectorGetZ(m_Eye);
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwx", &x, -10.0f, 10.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwy", &y, -10.0f, 10.0f);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwz", &z, -10.0f, 10.0f);
            m_Eye = DirectX::XMVectorSet(x, y, z, 0.0f);
            m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);

            ImGui::Text("Camera FOV Degree");
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cfx", &m_CameraFovYRadius, 0.01f, 180.0f);
            float fovRadius = m_CameraFovYRadius * (DirectX::XM_PI / 180.0f);

            ImGui::Text("Camera Near / Far");
            ImGui::Text("Near");
            ImGui::SameLine();
            ImGui::SliderFloat("##cn", &m_CameraNear, 0.01f, 99.9f);
            ImGui::Text("Far ");
            ImGui::SameLine();
            ImGui::SliderFloat("##cf", &m_CameraFar, 0.01f, 99.9f);
            if (m_CameraNear < m_CameraFar)
            {
                m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadius, m_ClientWidth / (FLOAT)m_ClientHeight, m_CameraNear, m_CameraFar);
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    // Draw �迭 �Լ��� ȣ���ϱ� ���� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �̾ �׸� ��� ����
    m_DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
    m_DeviceContext->IASetInputLayout(m_InputLayout);
    m_DeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    m_DeviceContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
    m_DeviceContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);

    // ù��° ť�� ��� ���� ����
    ConstantBuffer cb1;
    cb1.WorldMatrix = XMMatrixTranspose(m_WorldMatrix);
    cb1.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb1.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    cb1.vLightDir[0] = m_LightDirsEvaluated[0];
    cb1.vLightDir[1] = m_LightDirsEvaluated[1];
    cb1.vLightColor[0] = m_LightColors[0];
    cb1.vLightColor[1] = m_LightColors[1];
    cb1.vOutputColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb1, 0, 0);

    // ����ϱ�
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

    // �ι�° ť�� ��� ���� ����
    ConstantBuffer cb2;
    cb2.WorldMatrix = XMMatrixTranspose(m_WorldMatrix2);
    cb2.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb2.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb2, 0, 0);

    // ����ϱ�
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

    // ����° ť�� ��� ���� ����
    ConstantBuffer cb3;
    cb3.WorldMatrix = XMMatrixTranspose(m_WorldMatrix3);
    cb3.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb3.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb3, 0, 0);

    // ����ϱ�
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

    // ������ ����
    for (int m = 0; m < 2; m++)
    {
        XMMATRIX mLight = XMMatrixTranslationFromVector(5.0f * XMLoadFloat4(&m_LightDirsEvaluated[m]));
        XMMATRIX mLightScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
        mLight = mLightScale * mLight;

        // ���� ����Ʈ�� ����
        cb1.WorldMatrix = XMMatrixTranspose(mLight);
        cb1.vOutputColor = m_LightColors[m];
        m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb1, 0, 0);
        m_DeviceContext->PSSetShader(m_PixelShaderSolid, nullptr, 0);
        m_DeviceContext->DrawIndexed(m_Indices, 0, 0);
    }

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

    // 5. ����Ʈ ����
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)m_ClientWidth;
    viewport.Height = (float)m_ClientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_DeviceContext->RSSetViewports(1, &viewport);

    // 6. ���� & ���ٽ� �� ����
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = m_ClientWidth;
    descDepth.Height = m_ClientHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ID3D11Texture2D* textureDepthStencil = nullptr;
    HR_T(m_Device->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    HR_T(m_Device->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_DepthStencilView));
    SAFE_RELEASE(textureDepthStencil);

    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

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

    // ������ü
    Vertex vertices[] =
    {
        // ���� Normal Y +
        {Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f)},
        {Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f)},
        {Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f)},
        {Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f)},

        // �ظ� Normal Y -	
        {Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f)},
        {Vector3(1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f)},
        {Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)},
        {Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)},

        // ���ʸ� Normal X -
        {Vector3(-1.0f, -1.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f)},
        {Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f)},
        {Vector3(-1.0f, 1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f)},
        {Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f)},

        // �����ʸ� Normal X +
        {Vector3(1.0f, -1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f)},
        {Vector3(1.0f, -1.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f)},
        {Vector3(1.0f, 1.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f)},
        {Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f)},

        // �ո� Normal Z -
        {Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f)},
        {Vector3(1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f)},
        {Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f)},
        {Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f)},

        // �޸� Normal Z +
        {Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)},
        {Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)},
        {Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)},
        {Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)}
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

    // ���ؽ� ���̴��� �������� ������ �� Input Layout ����
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ID3DBlob* vertexShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Vertex Shader ����", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }

    HR_T(m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout));

    // 3. Render() ���� ���������ο� ���ε��� ���ؽ� ���̴� ����
    HR_T(m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader));
    SAFE_RELEASE(vertexShaderBuffer);

    // 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����

    // ������ü
    WORD indices[] =
    {
        3,1,0, 2,1,3,
        6,4,5, 7,4,6,
        11,9,8, 10,9,11,
        14,12,13, 15,12,14,
        19,17,16, 18,17,19,
        22,20,21, 23,20,22
    };

    m_Indices = ARRAYSIZE(indices);         // �ε��� ���� ����

    bd = {};
    bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
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

    HR_T(CompileShaderFromFile(L"SolidPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Solid Pixel Shader ���� ����", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }
    HR_T(m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShaderSolid));
    SAFE_RELEASE(pixelShaderBuffer);

    // 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
    bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer));

    // ���̴��� ������ ������ ����
    m_WorldMatrix = XMMatrixIdentity();
    m_WorldMatrix2 = XMMatrixIdentity();
    m_WorldMatrix3 = XMMatrixIdentity();

    // ť�� Ʈ�������� ���Ϳ� �߰�
    m_CubeMatrix.push_back(Vector3{ 0.0f, 0.0f, 0.0f });
    m_CubeMatrix.push_back(Vector3{ -4.0f, 0.0f, 0.0f });
    m_CubeMatrix.push_back(Vector3{ -4.0f, 0.0f, 0.0f });

    // Initialize the view matrix
    m_Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    m_At = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);
    m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);
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
    SAFE_RELEASE(m_DepthStencilView);
}

bool DemoApp::InitImGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_Device, m_DeviceContext);

    return true;
}

void DemoApp::UnInitImGUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
    return __super::WndProc(hWnd, message, wParam, lParam);
}
