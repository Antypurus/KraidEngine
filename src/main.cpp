#include <windows.h>
#include <iostream>

#include <io.h>
#include <fcntl.h>
#include <Core/Core.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst, L"window1", 1280, 720);

    AllocConsole();

    FILE* instream;
    FILE* outstream;
    FILE* errstream;

    freopen_s(&instream, "CONIN$", "r", stdin);//replace stdin with the current console input buffer
    freopen_s(&outstream, "CONOUT$", "w", stdout);//replace stdout with the current console output buffer
    freopen_s(&errstream, "CONOUT$", "w", stderr);//replace stderr with the current console output buffer

    std::cout << "test" << std::endl;

    while(window.open)
    {
    }
    
    return 0;
}
