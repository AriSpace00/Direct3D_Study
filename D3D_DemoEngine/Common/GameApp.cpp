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

    // ������ ����
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

    // ������ ���
    RegisterClassExW(&m_wcex);

    // ���ϴ� ClientRect ũ��� �����Ǿ� ����
    RECT rcClient = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

    int windowPosX = (m_ScreenWidth / 2) - ((rcClient.right - rcClient.left) / 2);
    int windowPosY = (m_ScreenHeight / 2) - ((rcClient.bottom - rcClient.top) / 2);

    // ������ ����
    m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY,                                 // â ���� ��ġ
        rcClient.right - rcClient.left,                         // â ũ�� - Wdith
        rcClient.bottom - rcClient.top,                         // â ũ�� - Height
        nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
        return false;

    // ������ ���̱�
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

bool GameApp::Run()
{
    // PeekMessage �޼����� ������ true, ������ false
    while (TRUE)
    {
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (m_msg.message == WM_QUIT)
                break;

            // ������ �޼��� ó��
            TranslateMessage(&m_msg);   // Ű �Է� ���� �޽��� ��ȯ (ex. WM_KEYDOWN -> WM_CHAR)
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
