#include <windows.h>
#include <iostream>
#include "Window.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst);

    while(true)
    {
        OutputDebugString("running...\n");
    }
    
    return MessageBox(NULL, "hello, world 21234", "caption", 0);
}
