#include <windows.h>
#include <iostream>

#include <Core/Core.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst, L"window1", 1280, 720);

    INFO("test %d", 123);
    WARNING("test %d", 123);
    ERROR("test %d", 123);
    SUCCESS("test %d", 123);

    while(window.open)
    {
    }

    return 0;
}
