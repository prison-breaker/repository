#include "pch.h"
#include "Core.h"

HINSTANCE hInst;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPTSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    // �޸� ���� üũ
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    srand(unsigned int(time(nullptr)));

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // �⺻ �޽��� �����Դϴ�:
    // 1. GetMessage
    // - �޼��� ť���� �޼����� Ȯ�ε� ������ ����Ѵ�.
    // - msg.messge == WM_QUIT�� ���, false�� ��ȯ�Ѵ�.(���α׷��� �����Ѵ�.)
    //
    // 2. PeekMessage
    // - �޼��� ť�� �޼����� ������ ���� ���� ������� �ʴ´�.
    // - �޼����� ���� ��� true, ���� ��� false�� ��ȯ�Ѵ�.
    MSG msg = {};

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else // �޼����� ���� ��� ȣ��
        {
            CCore::GetInstance()->AdvanceFrame();
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WndClass";
    wcex.hIconSm = nullptr;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    RECT rect = { 0, 0, 1920, 1080 };

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    XMFLOAT2 resolution = { (float)(rect.right - rect.left), (float)(rect.bottom - rect.top) };
    HWND hWnd = CreateWindowW(L"WndClass", L"PRISON BREAKER", WS_OVERLAPPEDWINDOW, 0, 0, (int)resolution.x, (int)resolution.y, nullptr, nullptr, hInstance, nullptr);

    if (hWnd == nullptr)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    CCore::GetInstance()->Init(hWnd, resolution);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
