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
    }

}
