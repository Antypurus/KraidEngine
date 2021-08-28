#include <windows.h>
#include <iostream>
#include "Window.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst);

    MSG msg;
    while (1)
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return MessageBox(NULL, "hello, world 21234", "caption", 0);
}
