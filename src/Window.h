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
        HWND window_handle;
        uint16 width;
        uint16 height;
        bool open = false;

        Window(HINSTANCE instance, const std::wstring& title, uint16 width, uint16 heigth);
        ~Window();
    };
}
