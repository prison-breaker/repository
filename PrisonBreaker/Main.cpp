#include "stdafx.h"
#include "Framework.h"

HINSTANCE hInst;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    srand(unsigned int(time(nullptr)));

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg{};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            CFramework::GetInstance()->FrameAdvance();
        }
    }

    CFramework::GetInstance()->OnDestroy();

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = TEXT("WndClass");
    wcex.hIconSm = nullptr;

    return RegisterClassExA(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    RECT Rect{ 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
    DWORD Style{ WS_POPUP };
    
    AdjustWindowRect(&Rect, Style, FALSE);
    
    HWND hWnd{ CreateWindowEx(WS_EX_APPWINDOW, TEXT("WndClass"), TEXT("PRISON BREAKER"), Style, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, nullptr, nullptr, hInstance, nullptr) };

    if (!hWnd)
    {
        return FALSE;
    }

    CFramework::GetInstance()->OnCreate(hInstance, hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
        CFramework::GetInstance()->SetActive(static_cast<bool>(wParam));
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
        CFramework::GetInstance()->ProcessMouseMessage(hWnd, message, wParam, lParam);
        break;
    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
        CFramework::GetInstance()->ProcessKeyboardMessage(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
