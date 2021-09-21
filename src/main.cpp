#include <windows.h>
#include <iostream>
#include "Window.h"
#include "Thread.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst, L"window1", 1280, 720);
    Window window2(hInst, L"window2", 1280, 720);
    Window window3(hInst, L"window3", 1280, 720);

    while(window.open)
    {
    }
    
    return 0;
}
