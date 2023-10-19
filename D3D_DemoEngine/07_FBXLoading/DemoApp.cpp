#include "DemoApp.h"
#include "../Common/Helper.h"
#include <d3dcompiler.h>
#include <Directxtk/WICTextureLoader.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX::SimpleMath;

DemoApp::DemoApp(HINSTANCE hInstance)
    : GameApp(hInstance)
    , m_CameraNear(0.01f)
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

    float t = GameTimer::m_Instance->TotalTime();

    // y축을 기준으로 큐브 회전
    Matrix mSpin;
    Matrix mSpinX = XMMatrixRotationX(m_CubeRotationX);
    Matrix mSpinY = XMMatrixRotationY(m_CubeRotationY);
    mSpin = mSpinX * mSpinY;

    // 크기 변경
    Matrix mScale = XMMatrixScaling(m_MeshScale, m_MeshScale, m_MeshScale);
    m_WorldMatrix = mScale * mSpin;

    m_Light.EyePosition = m_Eye;
}

void DemoApp::Render()
{
    // 화면 칠하기
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Draw 계열 함수를 호출하기 전에 렌더링 파이프라인에 필수 스테이지 설정을 해야한다.
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_DeviceContext->IASetInputLayout(m_InputLayout);
    m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    m_DeviceContext->VSSetConstantBuffers(0, 1, &m_CBTransform);
    m_DeviceContext->PSSetConstantBuffers(1, 1, &m_CBDirectionalLight);
    m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
    m_DeviceContext->PSSetConstantBuffers(0, 1, &m_CBTransform);
    m_DeviceContext->PSSetConstantBuffers(1, 1, &m_CBDirectionalLight);
    m_DeviceContext->PSSetConstantBuffers(2, 1, &m_CBMaterial);
    m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerLinear);

    // Cube, Lighting matrix 를 m_Transform 에 설정
    m_Transform.WorldMatrix = XMMatrixTranspose(m_WorldMatrix);
    m_Transform.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    m_Transform.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_CBTransform, 0, nullptr, &m_Transform, 0, 0);
    m_Light.Direction.Normalize();
    m_DeviceContext->UpdateSubresource(m_CBDirectionalLight, 0, nullptr, &m_Light, 0, 0);

    for (size_t i = 0; i < m_Meshes.size(); i++)
    {
        size_t mi = m_Meshes[i].m_MaterialIndex;

        m_DeviceContext->PSSetShaderResources(0, 1, &m_Materials[mi].m_DiffuseRV);
        m_DeviceContext->PSSetShaderResources(1, 1, &m_Materials[mi].m_NormalRV);
        m_DeviceContext->PSSetShaderResources(2, 1, &m_Materials[mi].m_SpecularRV);
        m_DeviceContext->PSSetShaderResources(3, 1, &m_Materials[mi].m_EmissiveRV);
        m_DeviceContext->PSSetShaderResources(4, 1, &m_Materials[mi].m_OpacityRV);

        m_Material.UseDiffuseMap = m_Materials[mi].m_DiffuseRV != nullptr ? true : false;
        m_Material.UseNormalMap = m_Materials[mi].m_NormalRV != nullptr ? true : false;
        m_Material.UseSpecularMap = m_Materials[mi].m_SpecularRV != nullptr ? true : false;
        m_Material.UseEmissiveMap = m_Materials[mi].m_EmissiveRV != nullptr ? true : false;
        m_Material.UseOpacityMap = m_Materials[mi].m_OpacityRV != nullptr ? true : false;

        if (m_Material.UseOpacityMap)
        {
            m_DeviceContext->OMSetBlendState(m_AlphaBlendState, nullptr, 0xffffffff);
        }
        else
        {
            m_DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        }

        m_DeviceContext->UpdateSubresource(m_CBMaterial, 0, nullptr, &m_Material, 0, 0);
        m_DeviceContext->IASetIndexBuffer(m_Meshes[i].m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_Meshes[i].m_VertexBuffer, &m_Meshes[i].m_VertexBufferStride, &m_Meshes[i].m_VertexBufferOffset);
        m_DeviceContext->DrawIndexed(m_Meshes[i].m_IndexCount, 0, 0);
    }

    // ImGui
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;           // 키보드 입력값 받기
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;            // 게임패드 입력값 받기

        // ImGUI 프레임 생성
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. 큐브 설정 윈도우
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

        // 2. 카메라 설정 윈도우
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
            ImGui::SliderFloat("##cwx", &x, -1000.0f, 1000.0f);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwy", &y, -1000.0f, 1000.0f);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##cwz", &z, -1000.0f, 0.0f);
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
            ImGui::SliderFloat("##cn", &m_CameraNear, 0.01f, 9999.9f);
            ImGui::Text("Far ");
            ImGui::SameLine();
            ImGui::SliderFloat("##cf", &m_CameraFar, 0.01f, 9999.9f);
            if (m_CameraNear < m_CameraFar)
            {
                m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadius, m_ClientWidth / (FLOAT)m_ClientHeight, m_CameraNear, m_CameraFar);
            }

            ImGui::End();
        }

        // 3. Light 설정 윈도우
        ImGui::SetNextWindowSize(ImVec2(350, 450));
        ImGui::SetNextWindowPos(ImVec2(0, 400));
        {
            ImGui::Begin("Light Properties");

            ImGui::Text("Use Normal Map");
            ImGui::SameLine();
            ImGui::Checkbox("##bNormalMap", (bool*)&m_Material.UseNormalMap);

            ImGui::Text("Use Specular Map");
            ImGui::SameLine();
            ImGui::Checkbox("##bSpecularMap", (bool*)&m_Material.UseSpecularMap);

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
            ImGui::ColorEdit4("##ma", (float*)&m_Material.Ambient);
            ImGui::Text("Material Diffuse");
            ImGui::ColorEdit4("##md", (float*)&m_Material.Diffuse);
            ImGui::Text("Material Specular");
            ImGui::ColorEdit4("##ms", (float*)&m_Material.Specular);
            ImGui::Text("Material Specular Power");
            ImGui::SliderFloat("##sp", &m_Material.SpecularPower, 2.0f, 4096.0f);

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    // 현재 백 버퍼에서 렌더된 정보를 프론트 버퍼에(스크린) 전달
    m_SwapChain->Present(0, 0);
}

bool DemoApp::InitD3D()
{
    // 결과값
    HRESULT hr = 0;

    // 스왑체인 속성 설정 구조체 생성
    DXGI_SWAP_CHAIN_DESC swapDesc = {};

    swapDesc.BufferCount = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hWnd;                         // 스왑체인 출력할 창 핸들 값
    swapDesc.Windowed = true;                               // 창 모드 여부 설정
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // 백 버퍼(텍스처)의 가로/세로 크기 설정
    swapDesc.BufferDesc.Width = m_ClientWidth;
    swapDesc.BufferDesc.Height = m_ClientHeight;

    // 화면 주사율 설정
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;

    // 샘플링 관련 설정
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // 1. 장치 생성, 2. 스왑체인 생성, 3. 장치 컨텍스트 생성
    HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext));

    // 4. 렌더 타겟 뷰 생성 (백 버퍼를 이용하는 렌더 타겟 뷰)
    ID3D11Texture2D* BackBufferTexture = nullptr;

    HR_T(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture));
    HR_T(m_Device->CreateRenderTargetView(BackBufferTexture, NULL, &m_RenderTargetView));   // 텍스처는 내부 참조 증가
    SAFE_RELEASE(BackBufferTexture);                                                              // 외부 참조 카운트를 감소시킨다

    // 렌더 타겟을 최종 출력 파이프라인에 바인딩
    // Flip Mode가 아닐 때에는 최초 한번만 설정하면 됨
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);

    // 5. 뷰포트 설정
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)m_ClientWidth;
    viewport.Height = (float)m_ClientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_DeviceContext->RSSetViewports(1, &viewport);

    // 6. 뎁스 & 스텐실 뷰 생성
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

    // 7. 투명 처리를 위한 블렌드 상태 생성
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

    HR_T(m_Device->CreateBlendState(&blendDesc, &m_AlphaBlendState));

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
    ID3D10Blob* errorMessage = nullptr; // 컴파일 에러 메시지가 저장될 버퍼

    // 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼 및 버퍼 정보 준비

    // 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성
    // Input Layout은 버텍스 셰이더가 입력받을 데이터의 형식을 지정한다.
    // SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate

    // 버텍스 셰이더가 문제없이 생성된 후 Input Layout 생성
    ID3DBlob* vertexShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"FBXLoadVS.hlsl", "main", "vs_4_0", &vertexShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Vertex Shader 오류", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HR_T(m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout));

    // 3. Render() 에서 파이프라인에 바인딩할 버텍스 셰이더 생성
    HR_T(m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader));
    SAFE_RELEASE(vertexShaderBuffer);

    // 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성

    // 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
    ID3DBlob* pixelShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"FBXLoadPS.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Pixel Shader 생성 오류", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }
    HR_T(m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader));
    SAFE_RELEASE(pixelShaderBuffer);

    // 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Transform);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_CBTransform));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_DirectionalLight);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_CBDirectionalLight));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Material);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_CBMaterial));

    // Sample state 생성
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR_T(m_Device->CreateSamplerState(&sampDesc, &m_SamplerLinear));

    // 7. 쉐이더에 전달할 데이터 설정

    // 월드 매트릭스 초기화
    m_WorldMatrix = XMMatrixIdentity();

    // 뷰 매트릭스 초기화
    m_Eye = XMVectorSet(0.0f, 0.0f, -200.0f, 0.0f);
    m_At = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);
    m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);

    // 프로젝션 매트릭스 초기화
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 20000.0f);

    // 8. FBX Loading
    Assimp::Importer importer;
    unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace |
        aiProcess_ConvertToLeftHanded;

    const aiScene* scene = importer.ReadFile("../Resource/ZeldaPosed001.fbx", importFlags);

    if (!scene) {
        LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
        return false;
    }

    m_Materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        m_Materials[i].Create(m_Device, scene->mMaterials[i]);
    }

    m_Meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        m_Meshes[i].Create(m_Device, scene->mMeshes[i]);
    }
    importer.FreeScene();

    return true;
}

void DemoApp::UnInitScene()
{
    m_Meshes.clear();
    m_Materials.clear();

    SAFE_RELEASE(m_CBTransform);
    SAFE_RELEASE(m_CBDirectionalLight);
    SAFE_RELEASE(m_CBMaterial);

    SAFE_RELEASE(m_AlphaBlendState);
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_InputLayout);
    SAFE_RELEASE(m_DepthStencilView);
}

bool DemoApp::InitImGUI()
{
    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui 스타일 설정
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // 플랫폼, 렌더러 설정
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
