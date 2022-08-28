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
    public:
        CRITICAL_SECTION mutex = {};
        uint64 spin_limit = 1000;
    public:
        //TODO(Tiago): allow the spin limit to be configured in the constructor?
        Mutex();
        ~Mutex();
        void Lock();
        void Unlock();

        Mutex(const Mutex& other) = delete;
        Mutex(Mutex&& other);
        Mutex& operator=(const Mutex& other) = delete;
        Mutex& operator=(Mutex&& other);
    };

    class SlimMutex
    {
    public:
        SRWLOCK mutex = {};
    public:
        SlimMutex();
        ~SlimMutex() = default;
        void Lock();
        void Unlock();

        SlimMutex(const SlimMutex& other) = delete;
        SlimMutex(SlimMutex&& other);
        SlimMutex& operator=(const SlimMutex& other) = delete;
        SlimMutex& operator=(SlimMutex&& other);
    };

    class ConditionVariable
    {
    public:
        CONDITION_VARIABLE cond_var;
        SlimMutex associated_mutex;

    public:
        ConditionVariable();
        void Sleep();
        void Wake();
        SlimMutex& operator->();
    };
}
