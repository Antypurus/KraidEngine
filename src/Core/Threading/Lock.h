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

        //NOTE(Tiago):copying and/or moving around an SRW lock is generally not safe. While MS states it can be done when the lock is not taken
        //and there are no waiting threads, the details on how it should be done are non-existent coupled with the generally hard to understand
        //inner working on this structure I prefer to disallow copies/moves of it, this will however extend this restriction to the condition
        //variable class. The other thing to consider is that doing the move/copy while not protected by a lock will probably lead to issues and
        //require the SRW lock to keep a secondary lock that is used to protect during copies and that would likely add overhead to other ops.
        SlimMutex(const SlimMutex& other) = delete;
        SlimMutex(SlimMutex&& other) = delete;
        SlimMutex& operator=(const SlimMutex& other) = delete;
        SlimMutex& operator=(SlimMutex&& other) = delete;
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
        void Lock();
        void Unlock();
        SlimMutex* operator->();
        //NOTE(Tiago): The slim mutex class cannot be copied or moved around. Therefore unless we start keeping a pointer to one instead of
        //having the actuall class embedded as a member, moves and copies will also not be allowed for the condition variable.
        ConditionVariable(ConditionVariable&& other) = delete;
        ConditionVariable& operator=(ConditionVariable&& other) = delete;
        ConditionVariable(const ConditionVariable& other) = delete;
        ConditionVariable& operator=(const ConditionVariable& other) = delete;
    };
}
