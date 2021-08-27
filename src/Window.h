#pragma once

#include <Windows.h>

namespace hvrt
{
    struct Window
    {
        HWND window_handle;

        Window(HINSTANCE instance);
    };
}
