#pragma once

#include <Core/types.h>
#include <windef.h>

namespace Kraid
{

    struct Mutex
    {
    public:
        HANDLE mutex_handle = nullptr;
    public:
        Mutex();
        ~Mutex();
        void Lock();
        void Unlock();

        Mutex(const Mutex& other) = delete;
        Mutex(Mutex&& other);
        Mutex& operator=(const Mutex& other) = delete;
        Mutex& operator=(Mutex&& other);
    };

}
