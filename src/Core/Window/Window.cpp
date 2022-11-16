#include "Window.h"
#include "Core/Window/Window.h"

//our includes
#include <Core/Threading/Thread.h>
#include <Core/Utils/Log.h>

namespace Kraid
{

	static std::unordered_map<HWND, Window*>* g_handle_map = new std::unordered_map<HWND, Window*>;

	LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        //NOTE(Tiago): when the window is initially created, a message is immeditely sent
        //at that point we still dont have the window handle and as such the window ptr
        //has yet to be registered into the map, which forces nullptr to be a valid condition
        //for the window ptr
        Window* window = (*g_handle_map)[hwnd];
        if(window != nullptr)
        {
            window->ExecuteEventCallbacks(uMsg,hwnd,wParam,lParam);
        }

        //NOTE(Tiago):I think that for now the way I want to handle this is that callbacks are
        //called at the begginig outside of the switch. And, then any special behavior we want
        //from inside the window class itself will be inside this switch. This way we kinda
        //have a pretty clear seperation between the "user" code that gets called and the internal
        //window class code.
		switch (uMsg)
		{
            case WM_CLOSE:
            {
                if(window != nullptr)
                {
                    window->open = false;
                }

                DestroyWindow(hwnd);
                return 0;
            }
            case WM_SIZE:
            {
                uint32 new_width = LOWORD(lParam);
                uint32 new_height = HIWORD(lParam);

                if(window != nullptr)
                {
                    window->height = new_height;
                    window->width = new_width;
                }
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
                EndPaint(hwnd, &ps);
                return 0;
            }
            default: return DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
	}

	Window::Window(HINSTANCE instance,const std::wstring& title, uint16 width, uint16 heigth) :width(width), height(heigth)
	{
		//NOTE(Tiago): i could use a thread barrier to wait for the window to be created in its thread, but really
		//there is no need since only one thread updates it and the other thread reads it, and it will be updated
		//only once, so there is no need to pay the cost of a thread barrier.
		volatile bool windows_was_created = false;

		std::function<DWORD(void*)> thread_function = [&windows_was_created, this, instance, title](void* args) -> DWORD
		{
            //compute the window size such that the client rect (the area where things are drawn) has the size
            //we desire for it.
			RECT client_rect;
			client_rect.bottom = this->height;
			client_rect.left = 0;
			client_rect.right = this->width;
			client_rect.top = 0;

			if (AdjustWindowRectEx(&client_rect, WS_OVERLAPPEDWINDOW, false, NULL) == 0)
			{
				//failed to adjust
                LERROR("Failed to adjust window client rectangle");
				return 1;
			}

			const wchar_t window_class_name[] = L"Window class";

			WNDCLASSW window_class = {};
			window_class.lpszClassName = window_class_name;
			window_class.hInstance = instance;
			window_class.lpfnWndProc = Window::WindowProc;
            window_class.hIcon = (HICON) LoadImageW(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			RegisterClassW(&window_class);

			this->window_handle = CreateWindowExW(
				NULL,
				window_class_name,
				title.c_str(),
				WS_OVERLAPPEDWINDOW,
				//position
				CW_USEDEFAULT, CW_USEDEFAULT,
                //size (width, heigth)
				client_rect.right - client_rect.left, client_rect.bottom - client_rect.top,
				NULL,
				NULL,
				instance,
				NULL
			);

			if (this->window_handle == NULL)
			{
				this->open = false;
                HRESULT error = GetLastError();
                wchar_t* error_message = FormatErrorMessage(error);
                LERROR(L"Failed to create window:%ls", error_message);
                free(error_message);
				return 1;
			}
            LSUCCESS(L"Window %ls create", title.c_str());

            (*g_handle_map)[this->window_handle] = this;
			ShowWindow(this->window_handle, SW_SHOW);

			this->open = true;
			windows_was_created = true;

			MSG msg;
			while (this->open)
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			return 0;
		};

		Thread create_window_thread(thread_function, nullptr);
		while (!windows_was_created) {}
        this->RegisterSpecializedEventCallbacks();
	}

	Window::~Window()
	{
		this->open = false;
        (*g_handle_map)[this->window_handle] = nullptr;
	}

    void Window::RegisterUniversalEventCallback(const std::function<LRESULT (HWND, uint32, WPARAM, LPARAM)> &callback)
    {
        if(this->m_event_callbacks.contains(0))
        {
            this->m_event_callbacks[0].push_back(callback);
        }
        else
        {
            this->m_event_callbacks[0] = {callback};
        }
    }

    void Window::RegisterEventCallback(uint32 event, const std::function<LRESULT(HWND,uint32,WPARAM,LPARAM)>& callback)
    {
        if(this->m_event_callbacks.contains(event))
        {
            this->m_event_callbacks[event].push_back(callback);
        }
        else
        {
            this->m_event_callbacks[event] = {callback};
        }
    }

    LRESULT Window::ExecuteEventCallbacks(uint32 event, HWND window_handle, WPARAM wParam, LPARAM lParam)
    {
        //execute universal callbacks
        if(this->m_event_callbacks.contains(0)){
            std::vector<std::function<LRESULT(HWND,uint32,WPARAM,LPARAM)>>& global_callbacks = this->m_event_callbacks[0];
            for(uint32 i = 0; i < global_callbacks.size(); ++i)
            {
                global_callbacks[i](window_handle,event,wParam,lParam);
            }
        }

        //execute callbacks registered to this event specifically
        if(this->m_event_callbacks.contains(event)){
            std::vector<std::function<LRESULT(HWND,uint32,WPARAM,LPARAM)>>& callbacks = this->m_event_callbacks[event];
            for(uint32 i = 0; i < callbacks.size(); ++i)
            {
                callbacks[i](window_handle,event,wParam,lParam);
            }
        }

        return 0;
    }

    void Window::RegisterSpecializedEventCallbacks()
    {
        this->RegisterEventCallback((uint32)WindowEventMessage::Size, )
    }
}


