#include "Window.h"

namespace hvrt
{

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_CLOSE:
            {
                DestroyWindow(hwnd);
                return 0;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
                EndPaint(hwnd, &ps);
                return 0;
            }
            default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    Window::Window(HINSTANCE instance)
    {
        const wchar_t window_class_name[] = L"Window class";

        WNDCLASS window_class = {};
        window_class.lpszClassName = window_class_name;
        window_class.hInstance = instance;
        window_class.lpfnWndProc = WindowProc;
        RegisterClass(&window_class);

        this->window_handle = CreateWindowExW(
                    0,
                    window_class_name,
                    L"Learn to program windows",
                    WS_OVERLAPPEDWINDOW,
                    //size and position
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                    NULL,
                    NULL,
                    instance,
                    NULL
                );

        if(this->window_handle == NULL)
        {
            __debugbreak();
            return;
        }

        ShowWindow(this->window_handle, SW_SHOW);
    }
}
