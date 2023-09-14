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
    Vector3 position;   // 정점 위치 정보
    Vector4 color;      // 정점 색상 정보

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
    UnInitImGUI();
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

    float t = GameTimer::m_Instance->TotalTime();

    // 첫번째 큐브: Y축으로 돌기
    m_WorldMatrix = XMMatrixRotationY(t);

    // 두번째 큐브 : 첫번째 큐브를 중심으로 Y축으로 돌기
    //XMMATRIX mSpin = XMMatrixRotationZ(-t);
    XMMATRIX mOrbit = XMMatrixRotationY(-t * 1.2f);
    XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
    XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

    m_WorldMatrix2 = mScale * mTranslate * mOrbit; // 스케일 적용 -> R(제자리 Y회전) -> 왼쪽으로 이동 -> 궤도 회전

    // 세번째 큐브 : 두번째 큐브를 중심으로 Y축으로 돌기
    XMMATRIX mSpin2 = XMMatrixRotationZ(-t);
    XMMATRIX mOrbit2 = XMMatrixRotationY(-t * 8.0f);
    XMMATRIX mTranslate2 = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
    XMMATRIX mScale2 = XMMatrixScaling(0.3f, 0.3f, 0.3f);

    m_WorldMatrix3 = mScale2 * mSpin2 * mTranslate2 * mOrbit2 * (mScale * mTranslate * mOrbit); // 스케일 적용 -> R(제자리 Y회전) -> 왼쪽으로 이동 -> 궤도 회전
} 

void DemoApp::Render()
{
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);

    // 화면 칠하기
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_InitColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;           // 키보드 입력값 받기
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;            // 게임패드 입력값 받기

    // ImGUI 프레임 생성
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. 데모 윈도우 보이기
    if (m_IsDemoWindow)
        ImGui::ShowDemoWindow(&m_IsDemoWindow);

    // 2. 커스텀한 윈도우 보이기
    ImGui::Begin("ImGUI Custom Window");
    ImGui::Text("Display Custom Text");
    ImGui::Checkbox("Demo Window", &m_IsDemoWindow);
    ImGui::Checkbox("Another Window", &m_IsAnotherWindow);

    








    // Draw 계열 함수를 호출하기 전에 렌더링 파이프라인에 필수 스테이지 설정을 해야한다.
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정점을 이어서 그릴 방식 설정
    m_DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
    m_DeviceContext->IASetInputLayout(m_InputLayout);
    m_DeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    m_DeviceContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

    // 첫번째 큐브 상수 버퍼 설정
    ConstantBuffer cb1;
    cb1.WorldMatrix = XMMatrixTranspose(m_WorldMatrix);
    cb1.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb1.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb1, 0, 0);

    // 출력하기
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

    // 두번째 큐브 상수 버퍼 설정
    ConstantBuffer cb2;
    cb2.WorldMatrix = XMMatrixTranspose(m_WorldMatrix2);
    cb2.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb2.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb2, 0, 0);

    // 출력하기
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

    // 세번째 큐브 상수 버퍼 설정
    ConstantBuffer cb3;
    cb3.WorldMatrix = XMMatrixTranspose(m_WorldMatrix3);
    cb3.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    cb3.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);
    m_DeviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb3, 0, 0);

    // 출력하기
    m_DeviceContext->DrawIndexed(m_Indices, 0, 0);

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
    ID3D10Blob* errorMessage = nullptr; // 컴파일 에러 메시지가 저장될 버퍼

    // 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼 및 버퍼 정보 준비

    // 정육면체
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

    // 삼각기둥
    /*Vertex vertices[] =
    {
        Vertex(Vector3(-1.0f, 1.0f, -1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(1.0f, 1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(0.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(0.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f))

    };*/

    // 삼각뿔
    /*Vertex vertices[] =
    {
        Vertex(Vector3(0.0f, 0.5f, (float)((std::sqrt(3)/2) - 1)), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(1.0f, -1.0f, (float)(std::sqrt(3) - 1)), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
    };*/

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // 버텍스 버퍼 생성
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;
    HR_T(hr = m_Device->CreateBuffer(&bd, &vbData, &m_VertexBuffer));

    // 버텍스 버퍼 정보
    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;

    // 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성
    // Input Layout은 버텍스 셰이더가 입력받을 데이터의 형식을 지정한다.
    // SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate

    // 버텍스 셰이더가 문제없이 생성된 후 Input Layout 생성
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ID3DBlob* vertexShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Vertex Shader 오류", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }

    HR_T(hr = m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout));

    // 3. Render() 에서 파이프라인에 바인딩할 버텍스 셰이더 생성
    HR_T(m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader));
    SAFE_RELEASE(vertexShaderBuffer);

    // 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성

    // 정육면체
    WORD indices[] =
    {
        3,1,0, 2,1,3,
        0,5,4, 1,5,0,
        3,4,7, 0,4,3,
        1,6,5, 2,6,1,
        2,7,6, 3,7,2,
        6,4,5, 7,4,6,
    };

    // 삼각기둥
    /*WORD indices[] =
    {
        0,2,1,
        3,4,5,
        2,3,5,
        0,3,2,
        0,4,3,
        1,4,0,
        1,5,4,
        2,5,1
    };*/

    // 삼각뿔
    /*WORD indices[] =
    {
        1,2,3,
        0,2,1,
        0,3,2,
        0,1,3
    };*/

    m_Indices = ARRAYSIZE(indices);         // 인덱스 개수 저장

    bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;
    HR_T(m_Device->CreateBuffer(&bd, &ibData, &m_IndexBuffer));

    // 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
    ID3DBlob* pixelShaderBuffer = nullptr;
    HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
    if (FAILED(hr))
    {
        MessageBoxA(m_hWnd, (char*)errorMessage->GetBufferPointer(), "Pixel Shader 생성 오류", MB_OK);
        SAFE_RELEASE(errorMessage);
        return false;
    }
    HR_T(m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader));
    SAFE_RELEASE(pixelShaderBuffer);

    // 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
    bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR_T(m_Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer));

    // 쉐이더에 전달할 데이터 설정
    m_WorldMatrix = XMMatrixIdentity();
    m_WorldMatrix2 = XMMatrixIdentity();
    m_WorldMatrix3 = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_ViewMatrix = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);
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
    return true;
}

void DemoApp::UnInitImGUI()
{
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return GameApp::WndProc(hWnd, message, wParam, lParam);
}
