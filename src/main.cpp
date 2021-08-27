#include <windows.h>
#include <iostream>
#include "Window.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    std::cout << "hello there" << std::endl;
    return MessageBox(NULL, "hello, world 21234", "caption", 0);
}
