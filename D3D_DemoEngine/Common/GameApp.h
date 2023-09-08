#pragma once
#include <windows.h>
#include "TimeSystem.h"
#define MAX_LOADSTRING 100

class GameApp
{
public:
    GameApp(HINSTANCE hInstance);
    virtual ~GameApp();

    static HWND m_hWnd;         
    static GameApp* m_Instance;

public:
    HACCEL m_hAccelTable;
    MSG m_msg;
    HINSTANCE m_hInstance;                  // ���� �ν��Ͻ�
    WCHAR m_szTitle[MAX_LOADSTRING];        // ���� ǥ���� �ؽ�Ʈ
    WCHAR m_szWindowClass[MAX_LOADSTRING];  // �⺻ â Ŭ���� �̸�
    WNDCLASSEXW m_wcex;

    int m_nCmdShow;

    UINT m_ClientWidth;
    UINT m_ClientHeight;

    int m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    float m_previousTime;
    float m_currentTime;

    GameTimer m_Timer;

public:
    // ������ ���� ���, ����, ���̱�
    virtual bool Initialize(UINT width, UINT height);
    virtual bool Run();
    virtual void Update();
    virtual void Render() = 0;

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

