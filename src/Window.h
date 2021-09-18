#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

#include "types.h"
#include <functional>

double test2(void* args)
{
    return 0;
}

void testfn(const std::function<double(void*)>& func)
{
    return;
}

namespace hvrt
{
    struct Window
    {
        HWND window_handle;
        bool open = false;

        //I need the window title, I need the window size
        //i nee dto compute the actual window area
        Window(HINSTANCE instance, const std::wstring& title, uint16 width, uint16 heigth);
        ~Window();
    };
}
