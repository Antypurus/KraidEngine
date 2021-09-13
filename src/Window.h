#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace hvrt
{
    struct Window
    {
        HWND window_handle;
        bool open = false;

        Window(HINSTANCE instance);
        ~Window();
    };
}
