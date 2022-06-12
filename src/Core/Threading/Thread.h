#pragma once

#include <Core/stdcpp.h>

namespace Kraid
{

    typedef void* HANDLE;
    typedef unsigned long DWORD;

    struct Thread
    {

    public:
        HANDLE thread_handle = NULL;
        DWORD thread_id = NULL;

    public:
        Thread();
        Thread(DWORD (*thread_function)(void*), void* parameters);
        explicit Thread(const std::function<DWORD(void*)>& thread_function, void* parameters = nullptr);

    private:
        static DWORD lambda_thread_function_caller(void* args);

    private:
        struct lambda_caller_params
        {
            std::function<DWORD(void*)> lambda;
            void* args = nullptr;
        };
    };
}
