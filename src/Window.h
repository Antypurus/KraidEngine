#pragma once

#define UNICODE
#include <Windows.h>

namespace hvrt
{
    struct Window
    {
        HWND window_handle;

        Window(HINSTANCE instance);
        ~Window();
    };
}
