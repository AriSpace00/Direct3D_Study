#include "pch.h"
#include "GameApp.h"

GameApp* GameApp::m_Instance = nullptr;
HWND GameApp::m_hWnd;

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return GameApp::m_Instance->WndProc(hWnd, message, wParam, lParam);
}

GameApp::GameApp(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_szTitle(L"D3D11 Tutorial Default Window Class")
    , m_szWindowClass(L"GameApp")
    , m_ClientWidth(1024)
    , m_ClientHeight(768)
{
    GameApp::m_Instance = this;

    // 윈도우 설정
    m_wcex.hInstance = hInstance;
    m_wcex.cbSize = sizeof(WNDCLASSEX);
    m_wcex.style = CS_HREDRAW | CS_VREDRAW;
    m_wcex.lpfnWndProc = DefaultWndProc;
    m_wcex.cbClsExtra = 0;
    m_wcex.cbWndExtra = 0;
    m_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    m_wcex.lpszClassName = m_szWindowClass;
}

GameApp::~GameApp()
{
}

bool GameApp::Initialize(UINT width, UINT height)
{
    m_ClientWidth = width;
    m_ClientHeight = height;

    // 윈도우 등록
    RegisterClassExW(&m_wcex);

    // 원하는 ClientRect 크기로 조정되어 리턴
    RECT rcClient = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

    int windowPosX = (m_ScreenWidth / 2) - ((rcClient.right - rcClient.left) / 2);
    int windowPosY = (m_ScreenHeight / 2) - ((rcClient.bottom - rcClient.top) / 2);

    // 윈도우 생성
    m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY,                                 // 창 시작 위치
        rcClient.right - rcClient.left,                         // 창 크기 - Wdith
        rcClient.bottom - rcClient.top,                         // 창 크기 - Height
        nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
        return false;

    // 윈도우 보이기
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

bool GameApp::Run()
{
    // PeekMessage 메세지가 있으면 true, 없으면 false
    while (TRUE)
    {
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (m_msg.message == WM_QUIT)
                break;

            // 윈도우 메세지 처리
            TranslateMessage(&m_msg);   // 키 입력 관련 메시지 변환 (ex. WM_KEYDOWN -> WM_CHAR)
            DispatchMessage(&m_msg);
        }
        else
        {
            Update();
            Render();
        }
    }
    return 0;
}

void GameApp::Update()
{
    m_Timer.Tick();
}

void GameApp::Render()
{
}

LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
