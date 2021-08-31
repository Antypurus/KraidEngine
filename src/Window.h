#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace hvrt
{
    struct Window
    {
        HWND window_handle;
        bool closed = false;
        bool force_close = false;

        Window(HINSTANCE instance);
        ~Window();
    };
}
