// WinMain.cpp : ���ø����̼ǿ� ���� �������� �����մϴ�.
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
    if (!App.Initialize(1024, 768))
        return -1;

    return App.Run();
}

