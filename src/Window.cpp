#include "Window.h"
#include "Thread.h"
#include "stdlib.h"

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

    struct CreateWindowFunctionParameters
    {
        Window* window_obj;
        HINSTANCE instance;
    };

    static DWORD CreateWindowFunction(CreateWindowFunctionParameters* params)
    {
        CreateWindowFunctionParameters args = *params;
        free(params);

        const wchar_t window_class_name[] = L"Window class";

        WNDCLASS window_class = {};
        window_class.lpszClassName = window_class_name;
        window_class.hInstance = args.instance;
        window_class.lpfnWndProc = WindowProc;
        RegisterClass(&window_class);

        args.window_obj->window_handle = CreateWindowExW(
                    0,
                    window_class_name,
                    L"Learn to program windows",
                    WS_OVERLAPPEDWINDOW,
                    //size and position
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                    NULL,
                    NULL,
                    args.instance,
                    NULL
                );

        if(args.window_obj->window_handle == NULL)
        {
            __debugbreak();
            return 0;
        }

        ShowWindow(args.window_obj->window_handle, SW_SHOW);
        
        MSG msg;
        while (1)
        {
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }

    Window::Window(HINSTANCE instance)
    {
        CreateWindowFunctionParameters* params = (CreateWindowFunctionParameters*)malloc(sizeof(CreateWindowFunctionParameters));
        params->window_obj = this;
        params->instance = instance;

        Thread create_window_thread((DWORD(*)(void*))CreateWindowFunction, (void*)params);
    }

    Window::~Window()
    {
        TerminateThread(this->window_handle, 1);
    }
}
