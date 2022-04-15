#pragma once

#include <Core/types.h>
#include <windef.h>

namespace Kraid
{

    struct Mutex
    {
    public:
        HANDLE mutex_handle = nullptr;
        void Lock();
        void Unlock();
    public:
        Mutex();
        ~Mutex();
    };

}
