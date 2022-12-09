#pragma once

#include <Core/stdcpp.h>

#include <Core/Windows.h>
#include <Core/types.h>
#include <winuser.h>

#include <parallel_hashmap/phmap.h>


namespace Kraid
{

    enum class WindowEventMessage
    {
        Null                            = WM_NULL,
        Create                          = WM_CREATE,
        Destroy                         = WM_DESTROY,
        Move                            = WM_MOVE,
        Activate                        = WM_ACTIVATE,
        SetFocus                        = WM_SETFOCUS,
        KillFocus                       = WM_KILLFOCUS,
        Enable                          = WM_ENABLE,
        Paint                           = WM_PAINT,
        Close                           = WM_CLOSE,
        Quit                            = WM_QUIT,
        Size                            = WM_SIZE,
        ShowWindow                      = WM_SHOWWINDOW,
        SetCursor                       = WM_SETCURSOR,
        Power                           = WM_POWER,
        DisplayChanged                  = WM_DISPLAYCHANGE,
        InitialCreate                   = WM_NCCREATE,
        FinalDestroy                    = WM_NCDESTROY,
        FirstPressedKey                 = WM_KEYFIRST,
        KeyPressed                      = WM_KEYDOWN,
        KeyReleased                     = WM_KEYUP,
        LastKeyPressed                  = WM_KEYLAST,
        HorizontalScroll                = WM_HSCROLL,
        VerticalScroll                  = WM_VSCROLL,
        MouseMoved                      = WM_MOUSEMOVE,
        LeftMoutseButtonPressed         = WM_LBUTTONDOWN,
        LeftMouseButtonReleased         = WM_LBUTTONUP,
        LeftMouseButtonDoubleClick      = WM_LBUTTONDBLCLK,
        RightMouseButtonPressed         = WM_RBUTTONDOWN,
        RightMouseButtonReleased        = WM_RBUTTONUP,
        RightMouseButtonDoubleClick     = WM_RBUTTONDBLCLK,
        MiddleMouseButtonPressed        = WM_MBUTTONDOWN,
        MiddleMouseButtonReleased       = WM_MBUTTONUP,
        MiddleMouseButtonDoubleClick    = WM_MBUTTONDBLCLK,
        MouseScroll                     = WM_MOUSEWHEEL
    };

    struct Window
    {
    public:
        HWND window_handle = NULL;
        uint16 width = 0;
        uint16 height = 0;
        volatile bool open = false;
    private:
        phmap::flat_hash_map<
            uint32,
            std::vector<
                std::function<LRESULT(HWND, uint32, WPARAM, LPARAM)>
            >
        > m_event_callbacks;
        std::vector<std::function<void(uint32, uint32)>> m_window_resize_callbacks;

    public:
        Window(HINSTANCE instance,const std::wstring& title, uint16 width, uint16 heigth);
        ~Window();
        void RegisterUniversalEventCallback(const std::function<LRESULT(HWND,uint32,WPARAM,LPARAM)>& callback);
        void RegisterEventCallback(uint32 event,const std::function<LRESULT(HWND,uint32,WPARAM,LPARAM)>& callback);
        void RegisterWindowResizeEventCallback(const std::function<void(uint32,uint32)>& callback);
    private:
        LRESULT ExecuteEventCallbacks(uint32 event,HWND window_handle, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void RegisterSpecializedEventCallbacks();
    };
}
