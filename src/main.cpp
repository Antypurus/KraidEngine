#include <windows.h>
#include <iostream>
#include "Window.h"
#include "Thread.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst, L"", 0, 0);

    Thread thread([&window](void* args) -> DWORD {
            while (true)
            {
                OutputDebugStringW(L"test");
            }
        }, nullptr);

    while(window.open)
    {
    }
    
    return 0;
}
