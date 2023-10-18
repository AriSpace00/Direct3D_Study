// WinMain.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "DemoApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    DemoApp App(hInstance);
    if (!App.Initialize(1920, 1080))
        return -1;

    return App.Run();
}


