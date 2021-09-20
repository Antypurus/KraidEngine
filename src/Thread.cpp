#include "Thread.h"

namespace hvrt {

    Thread::Thread():thread_handle(NULL), thread_id(0)
    {
    }

    Thread::Thread(DWORD (*thread_function)(void*), void* parameters)
    {
        this->thread_handle = CreateThread(NULL, NULL, thread_function, parameters, 0, &this->thread_id);
        if(this->thread_handle == NULL)
        {
            MessageBox(NULL, "Failed to create request thread", "Thread", 0);
        }
        SetThreadDescription(this->thread_handle, L"My grea thread");
    }

    struct lamba_caller_params
    {
        std::function<DWORD(void*)> lambda;
        void* args;
    };

    DWORD Thread::lambda_thread_function_caller(void* args)
    {
        lamba_caller_params lambda_params = *(lamba_caller_params*)args;
        delete args;

        return lambda_params.lambda(lambda_params.args);
    }

    Thread::Thread(const std::function<DWORD(void*)>& thread_function, void* parameters)
    {
        lamba_caller_params* args = new lamba_caller_params;
        if (args == nullptr)
        {
            MessageBox(NULL, "Failed to create request thread", "Thread", 0);
            return;
        }
        args->lambda = thread_function;
        args->args = parameters;

        this->thread_handle = CreateThread(NULL, NULL, lambda_thread_function_caller, args, 0, &this->thread_id);
        if (this->thread_handle == NULL)
        {
            MessageBox(NULL, "Failed to create request thread", "Thread", 0);
        }
        SetThreadDescription(this->thread_handle, L"My grea thread");
    }

}