#pragma once

#include <Core/types.h>
#include <windef.h>

namespace Kraid
{

    class HardMutex
    {
    public:
        HANDLE mutex_handle = nullptr;
    public:
        HardMutex();
        ~HardMutex();
        void Lock();
        void Unlock();

        HardMutex(const HardMutex& other) = delete;
        HardMutex(HardMutex&& other);
        HardMutex& operator=(const HardMutex& other) = delete;
        HardMutex& operator=(HardMutex&& other);
    };

    class Mutex
    {
    };

    class SlimMutex
    {
    };

/*
    class ConditionVariable
    {
    public:
        CONDITION_VARIABLE cond_var;
        Mutex associated_mutex;

    public:
        ConditionVariable();
        void Sleep();
        void Wake();
        Mutex& operator->();
    };
*/
}
