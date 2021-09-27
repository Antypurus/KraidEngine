#include <windows.h>
#include <iostream>

#include <Core/Core.h>
#include <Core/Rendering/D3D12/DXGIFactory.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst, L"window1", 1280, 720);

    DXGIFactory factory;
    factory.LogGPUList();

    while(window.open)
    {
    }

    return 0;
}
