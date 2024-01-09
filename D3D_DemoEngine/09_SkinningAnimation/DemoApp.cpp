#include "DemoApp.h"
#include "../Common/Helper.h"
#include <d3dcompiler.h>
#include <Directxtk/WICTextureLoader.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "../Common/Node.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX::SimpleMath;

D3D_SHADER_MACRO defines[] =
{
    {"VERTEX_SKINNING", ""},
    {nullptr, nullptr}
};

DemoApp::DemoApp(HINSTANCE hInstance)
    : GameApp(hInstance)
    , m_CameraNear(1.0f)
    , m_CameraFar(9999.9f)
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

    //float deltaTime = GameTimer::m_Instance->DeltaTime();
    QueryPerformanceCounter(&m_CurrentTime);

    // Calculate delta time in seconds
    m_DeltaTime = static_cast<double>(m_CurrentTime.QuadPart - m_PrevTime.QuadPart) / m_Frequency.QuadPart;

    // Update the previous time for the next iteration
    m_PrevTime = m_CurrentTime;

    m_Model->Update(m_DeltaTime);

    Matrix mSpin;
    Matrix mSpinX = XMMatrixRotationX(m_CubeRotationX);
    Matrix mSpinY = XMMatrixRotationY(m_CubeRotationY);
    mSpin = mSpinY * mSpinX;

    Matrix mScale = Matrix::CreateScale(m_MeshScale, m_MeshScale, m_MeshScale);

    m_Model->SetTransform(DirectX::XMMatrixIdentity(), mSpin, mScale);

    m_Light.EyePosition = m_Eye;
}

void DemoApp::Render()
{
    // ȭ�� ĥ�ϱ�
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Draw �迭 �Լ��� ȣ���ϱ� ���� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_DeviceContext->IASetInputLayout(m_InputLayout);

    m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

    m_DeviceContext->PSSetConstantBuffers(1, 1, &m_CBDirectionalLight);

    m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerLinear);

    // ViewMatrix, ProjectionMatrix ����
    m_Model->m_Transform.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    m_Model->m_Transform.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);

    m_DeviceContext->UpdateSubresource(m_Model->m_CBTransform, 0, nullptr, &m_Model->m_Transform, 0, 0);
    m_DeviceContext->VSSetConstantBuffers(0, 1, &m_Model->m_CBTransform);
    m_DeviceContext->PSSetConstantBuffers(0, 1, &m_Model->m_CBTransform);

    // Light ����
    m_Light.Direction.Normalize();
    m_DeviceContext->UpdateSubresource(m_CBDirectionalLight, 0, nullptr, &m_Light, 0, 0);

    // Model Render
    m_Model->Render(m_DeviceContext);

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
        ImGui::SetNextWindowSize(ImVec2(350, 150));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        {
            ImGui::Begin("Mesh Properties");

            ImGui::Text("Rotation");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##cx", &m_CubeRotationX, 0.0f, 90.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cy", &m_CubeRotationY, 0.0f, 90.0f);
            ImGui::Text("Scale");
            ImGui::SameLine();
            ImGui::SliderFloat("##cs", &m_MeshScale, 0.0f, 100.0f);

            ImGui::End();
        }

        // 2. ī�޶� ���� ������
        ImGui::SetNextWindowSize(ImVec2(350, 250));
        ImGui::SetNextWindowPos(ImVec2(0, 150));
        {
            ImGui::Begin("Camera Properties");

            ImGui::Text("World Transform");
            float x = XMVectorGetX(m_Eye);
            float y = XMVectorGetY(m_Eye);
            float z = XMVectorGetZ(m_Eye);
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwx", &x, -1000.0f, 10000.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwy", &y, -1000.0f, 10000.0f);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwz", &z, -10000.0f, 0.0f);
            m_Eye = DirectX::XMVectorSet(x, y, z, 0.0f);
            m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);


            ImGui::Text("FOV Degree");
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cfx", &m_CameraFovYRadius, 0.01f, 180.0f);
            float fovRadius = m_CameraFovYRadius * (DirectX::XM_PI / 180.0f);

            ImGui::Text("Near / Far");
            ImGui::Text("Near");
            ImGui::SameLine();
            ImGui::SliderFloat("##cn", &m_CameraNear, 1.0f, 9999.9f);
            ImGui::Text("Far ");
            ImGui::SameLine();
            ImGui::SliderFloat("##cf", &m_CameraFar, 0.01f, 9999.9f);
            if (m_CameraNear < m_CameraFar)
            {
                m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadius, m_ClientWidth / (FLOAT)m_ClientHeight, m_CameraNear, m_CameraFar);
            }

            ImGui::End();
        }

        // 3. Light ���� ������
        ImGui::SetNextWindowSize(ImVec2(350, 450));
        ImGui::SetNextWindowPos(ImVec2(0, 400));
        {
            ImGui::Begin("Light Properties");

            /*if(IsDiffuseExist)
            {
                ImGui::Text("Use Diffuse Map");
                ImGui::SameLine();
                ImGui::Checkbox("##bDiffuseMap", &m_Material.UseDiffuseMap);
            }

            if(IsNormalExist)
            {
                ImGui::Text("Use Normal Map");
                ImGui::SameLine();
                ImGui::Checkbox("##bNormalMap", &m_Material.UseNormalMap);
            }

            if(IsSpecularExist)
            {
                ImGui::Text("Use Specular Map");
                ImGui::SameLine();
                ImGui::Checkbox("##bSpecularMap", &m_Material.UseSpecularMap);
            }

            if(IsEmissiveExist)
            {
                ImGui::Text("Use Emissive Map");
                ImGui::SameLine();
                ImGui::Checkbox("##bEmissiveMap", &m_Material.UseEmissiveMap);
            }

            if(IsOpacityExist)
            {
                ImGui::Text("Use Opacity Map");
                ImGui::SameLine();
                ImGui::Checkbox("##bOpacityMap", &m_Material.UseOpacityMap);
            }*/

            ImGui::Text("[Directional Light]");
            ImGui::Text("Light Direction");
            ImGui::SliderFloat3("##ld", (float*)&m_Light.Direction, -1.0f, 1.0f);
            ImGui::Text("Light Ambient");
            ImGui::ColorEdit4("##la", (float*)&m_Light.Ambient);
            ImGui::Text("Light Diffuse");
            ImGui::ColorEdit4("##ld", (float*)&m_Light.Diffuse);
            ImGui::Text("Light Specular");
            ImGui::ColorEdit4("##ls", (float*)&m_Light.Specular);

            ImGui::Text("[Material]");
            ImGui::Text("Material Ambient");
            ImGui::ColorEdit4("##ma", (float*)&m_Model->m_Material.Ambient);
            ImGui::Text("Material Diffuse");
            ImGui::ColorEdit4("##md", (float*)&m_Model->m_Material.Diffuse);
            ImGui::Text("Material Specular");
            ImGui::ColorEdit4("##ms", (float*)&m_Model->m_Material.Specular);
            ImGui::Text("Material Specular Power");
            ImGui::SliderFloat("##sp", &m_Model->m_Material.SpecularPower, 2.0f, 4096.0f);

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
    SAFE_RELEASE(m_RenderTargetView);
    SAFE_RELEASE(m_DepthStencilView);
    SAFE_RELEASE(m_Device);
    SAFE_RELEASE(m_DeviceContext);
    SAFE_RELEASE(m_SwapChain);
}

bool DemoApp::InitScene()
{
    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr; // ������ ���� �޽����� ����� ����

    // FBX Loading
    // FBX ���� ��� ����, ���� �̸� �и�
    std::string box = "../Resource/FBXLoad_Test/fbx/box.fbx";
    std::string charcter = "../Resource/FBXLoad_Test/fbx/Character.fbx";
    std::string icosphere = "../Resource/FBXLoad_Test/fbx/IcoSphere.fbx";
    std::string monkey = "../Resource/FBXLoad_Test/fbx/Monkey.fbx";
    std::string torus = "../Resource/FBXLoad_Test/fbx/Torus.fbx";
    std::string tree = "../Resource/FBXLoad_Test/fbx/Tree.fbx";
    std::string zelda = "../Resource/FBXLoad_Test/fbx/zeldaPosed001.fbx";
    std::string cerberus2 = "../Resource/FBXLoad_Test/fbx/cerberus2.fbx";
    std::string materialTest = "../Resource/FBXLoad_Test/fbx/BoneDummyWithMaterial.fbx";
    std::string skinningFBX = "../Resource/FBXLoad_Test/fbx/SkinningTest.fbx";

    std::string filePath;

    switch (m_FBXModelIndex)
    {
    case 0:
    {
        filePath = box;
        break;
    }
    case 1:
    {
        filePath = charcter;
        break;
    }
    case 2:
    {
        filePath = icosphere;
        break;
    }
    case 3:
    {
        filePath = monkey;
        break;
    }
    case 4:
    {
        filePath = torus;
        break;
    }
    case 5:
    {
        filePath = tree;
        break;
    }
    case 6:
    {
        filePath = zelda;
        break;
    }
    case 7:
    {
        filePath = cerberus2;
        break;
    }
    case 8:
    {
        filePath = materialTest;
        break;
    }
    case 9:
    {
        filePath = skinningFBX;
        break;
    }
    }

    // Model Ŭ������ FBX Load
    m_Model = new Model();
    m_Model->ReadFile(m_Device, filePath);

    // 1. Render() ���� ���������ο� ���ε��� ���ؽ� ���� �� ���� ���� �غ�

    // 2. Render() ���� ���������ο� ���ε��� InputLayout ����
    // Input Layout�� ���ؽ� ���̴��� �Է¹��� �������� ������ �����Ѵ�.
    // SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate

    // ���ؽ� ���̴��� �������� ������ �� Input Layout ����
    ID3DBlob* vertexShaderBuffer = nullptr;
    HR_T(CompileShaderFromFileWithMacro(L"SkinningVS.hlsl", defines, "main", "vs_5_0", &vertexShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Vertex Shader ����", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BlendIndices",    0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BlendWeights",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HR_T(m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout));

    // 3. Render() ���� ���������ο� ���ε��� ���ؽ� ���̴� ����
    HR_T(m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader));
    SAFE_RELEASE(vertexShaderBuffer);

    // 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����

    // 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
    ID3DBlob* pixelShaderBuffer = nullptr;
    HR_T(CompileShaderFromFileWithMacro(L"SkinningPS.hlsl", defines, "main", "ps_5_0", &pixelShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Pixel Shader ���� ����", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }
    HR_T(m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader));
    SAFE_RELEASE(pixelShaderBuffer);

    // 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Transform);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_Model->m_CBTransform));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_DirectionalLight);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_CBDirectionalLight));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Material);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_Model->m_CBMaterial));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_MatrixPalette);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_Model->m_CBMatrixPalette));

    // Sample state ����
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR_T(m_Device->CreateSamplerState(&sampDesc, &m_SamplerLinear));

    // 7. ���̴��� ������ ������ ����

    // �� ��Ʈ���� �ʱ�ȭ
    m_Eye = XMVectorSet(0.0f, 300.0f, -500.0f, 0.0f);
    m_At = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);
    m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);

    // �������� ��Ʈ���� �ʱ�ȭ
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ClientWidth / (FLOAT)m_ClientHeight, 1.0f, 20000.0f);

    // 7. ���� ó���� ���� ������ ���� ����
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HR_T(m_Device->CreateBlendState(&blendDesc, &m_Model->m_AlphaBlendState));

    // Get the frequency of the performance counter
    QueryPerformanceFrequency(&m_Frequency);

    // Get the initial time
    QueryPerformanceCounter(&m_PrevTime);

    return true;
}

void DemoApp::UnInitScene()
{
    m_Meshes.clear();
    m_Materials.clear();

    SAFE_RELEASE(m_CBDirectionalLight);

    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_InputLayout);
}

bool DemoApp::InitImGUI()
{
    // ImGui �ʱ�ȭ
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui ��Ÿ�� ����
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // �÷���, ������ ����
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