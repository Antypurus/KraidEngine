#include "Window.h"
#include "Thread.h"
#include "stdlib.h"
#include <unordered_map>

namespace hvrt
{

	static std::unordered_map<HWND, Window*> g_handle_map;

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
		{
			Window* window = g_handle_map[hwnd];
			window->open = false;
			DestroyWindow(hwnd);
			return 0;
		}
        case WM_SIZE:
        {
            uint32 new_width = LOWORD(lParam);
            uint32 new_height = HIWORD(lParam);

			Window* window = g_handle_map[hwnd];
			window->height = new_height;
			window->width = new_width;
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
		default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	Window::Window(HINSTANCE instance, const std::wstring& title, uint16 width, uint16 heigth) :width(width), height(heigth)
	{
		//NOTE(Tiago): i could use a thread barrier to wait for the window to be created in its thread, but really
		//there is no need since only one thread updates it and the other thread reads it, and it will be updated
		//only once, so there is no need to pay the cost of a thread barrier.
		bool windows_was_created = false;

		std::function<DWORD(void*)> thread_function = [&windows_was_created, this, instance, &title](void* args) -> DWORD
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
				__debugbreak();
				return 1;
			}

			const wchar_t window_class_name[] = L"Window class";

			WNDCLASS window_class = {};
			window_class.lpszClassName = window_class_name;
			window_class.hInstance = instance;
			window_class.lpfnWndProc = WindowProc;
			RegisterClass(&window_class);

			this->window_handle = CreateWindowExW(
				0,
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
				__debugbreak();
				return 1;
			}

			g_handle_map[this->window_handle] = this;
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
		while (!windows_was_created);
	}

	Window::~Window()
	{
		this->open = false;
		g_handle_map[this->window_handle] = nullptr;
	}
}
