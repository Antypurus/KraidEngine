#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <functional>
#include <unordered_map>

#include "types.h"

namespace hvrt
{
    struct Window
    {
    public:
        HWND window_handle = NULL;
        uint16 width = 0;
        uint16 height = 0;
        bool open = false;
        std::unordered_multimap<uint32, std::function<LRESULT(HWND, uint32, WPARAM, LPARAM)>> event_callbacks;

    public:
        Window(HINSTANCE instance, const std::wstring& title, uint16 width, uint16 heigth);
        ~Window();
        void RegisterEventCallback(uint32 event,const std::function<LRESULT(HWND,uint32,WPARAM,LPARAM)>& callback);
    private:
        LRESULT ExecuteEventCallbacks(uint32 event);
    };
}
