#include <windows.h>
#include <iostream>
#include "Window.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst);

    while(!window.closed)
    {
    }
    
    return 0;
}
