#include "Thread.h"

#include <Core/Utils/Log.h>
#include <Core/Windows.h>

namespace Kraid {

    Thread::Thread()
    {
    }

    Thread::Thread(DWORD (*thread_function)(void*), void* parameters)
    {
        this->thread_handle = CreateThread(NULL, NULL, thread_function, parameters, 0, &this->thread_id);
        if (this->thread_handle == NULL)
        {
            HRESULT error = GetLastError();
            wchar_t* error_message = FormatErrorMessage(error);
            LERROR(L"Failed to create thread: %ls", error_message);
            free(error_message);
        }
        else
        {
            LSUCCESS("Thread created");
        }
    }

    DWORD Thread::lambda_thread_function_caller(void* args)
    {
        lambda_caller_params lambda_params = *(lambda_caller_params*)args;
        delete (lambda_caller_params*)args;

        return lambda_params.lambda(lambda_params.args);
    }

    Thread::Thread(const std::function<DWORD(void*)>& thread_function, void* parameters)
    {
        lambda_caller_params* args = new lambda_caller_params;
        if (args == nullptr)
        {
            LERROR("Failed to allocate memory required to hold lambda caller parameters");
            return;
        }
        args->lambda = thread_function;
        args->args = parameters;

        this->thread_handle = CreateThread(NULL, NULL, lambda_thread_function_caller, args, 0, &this->thread_id);
        if (this->thread_handle == NULL)
        {
            HRESULT error = GetLastError();
            wchar_t* error_message = FormatErrorMessage(error);
            LERROR(L"Failed to create thread: %ls", error_message);
            free(error_message);
        }
        else
        {
            LSUCCESS("Thread created");
        }
    }

}
