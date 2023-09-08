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
    HINSTANCE m_hInstance;                  // 현재 인스턴스
    WCHAR m_szTitle[MAX_LOADSTRING];        // 제목 표시줄 텍스트
    WCHAR m_szWindowClass[MAX_LOADSTRING];  // 기본 창 클래스 이름
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
    // 윈도우 정보 등록, 생성, 보이기
    virtual bool Initialize(UINT width, UINT height);
    virtual bool Run();
    virtual void Update();
    virtual void Render() = 0;

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

