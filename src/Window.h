#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <functional>

#include "types.h"

namespace hvrt
{
    struct Window
    {
        HWND window_handle = NULL;
        uint16 width = 0;
        uint16 height = 0;
        bool open = false;

        Window(HINSTANCE instance, const std::wstring& title, uint16 width, uint16 heigth);
        ~Window();
    };
}
