#pragma once

#include <Windows.h>
#include <functional>

namespace hvrt
{
    struct Thread
    {
        HANDLE thread_handle;
        DWORD thread_id;

        Thread();
        Thread(DWORD (*thread_function)(void*), void* parameters);
        explicit Thread(const std::function<DWORD(void*)>& thread_function, void* parameters);

    private:
        static DWORD lambda_thread_function_caller(void* args);
    };
}
