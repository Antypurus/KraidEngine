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
    };
}
