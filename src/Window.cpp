#include "Window.h"

namespace hvrt
{

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
                EndPaint(hwnd, &ps);
            }
            return 0;

        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    Window::Window(HINSTANCE instance)
    {
        const char window_class_name[] = "Window class";

        WNDCLASS window_class = {};
        window_class.lpszClassName = window_class_name;
        window_class.hInstance = instance;
        window_class.lpfnWndProc = WindowProc;
    }
}
