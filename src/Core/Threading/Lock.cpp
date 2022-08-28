#include "Lock.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

namespace Kraid
{
    
    /* ================ Start of Hard/True/Pure Mutex Implementation ================================*/

    HardMutex::HardMutex()
    {
        this->mutex_handle = CreateMutexW(nullptr, false, nullptr);
        if(this->mutex_handle == nullptr)
        {
            PRINT_WINERROR();
            return;
        }
    }

    HardMutex::HardMutex(HardMutex&& other)
    {
        if(this == &other) return;

        other.Lock();

        this->mutex_handle = other.mutex_handle;
        other.mutex_handle = nullptr;

        this->Unlock();
    }

    HardMutex& HardMutex::operator=(HardMutex&& other)
    {
        if(this == &other) return *this;

        other.Lock();

        this->mutex_handle = other.mutex_handle;
        other.mutex_handle = nullptr;

        this->Unlock();

        return *this;
    }

    HardMutex::~HardMutex()
    {
        if(this->mutex_handle != nullptr)
        {
            CloseHandle(this->mutex_handle);
        }
    }

    void HardMutex::Lock()
    {
        auto result = WaitForSingleObject(this->mutex_handle, INFINITE);
        while( result != WAIT_OBJECT_0)
        {
            result = WaitForSingleObject(this->mutex_handle, INFINITE);
        }
    }

    void HardMutex::Unlock()
    {
        bool result = ReleaseMutex(this->mutex_handle);
        if(result == false)
        {
            PRINT_WINERROR();
        }
    }

    /* ================ End of Hard/True/Pure Mutex Implementation ================================*/

    /* ================ Start of Standard Hybrid Mutex Implementation =============================*/
   
    Mutex::Mutex()
    {
        if(!InitializeCriticalSectionAndSpinCount(&this->mutex, this->spin_limit))
        {
            PRINT_WINERROR();
            return;
        }
    }

    Mutex::~Mutex()
    {
        DeleteCriticalSection(&this->mutex);
    }

    Mutex::Mutex(Mutex&& other)
    {
        if(this == &other) return;

        other.Lock();

        this->mutex = other.mutex;
        this->spin_limit = other.spin_limit;
        other.mutex = {};
        other.spin_limit = 0;

        this->Unlock();
    }

    Mutex& Mutex::operator=(Mutex &&other)
    {
        if(this == &other) return *this;

        other.Lock();

        this->mutex = other.mutex;
        this->spin_limit = other.spin_limit;
        other.mutex = {};
        other.spin_limit = 0;

        this->Unlock();

        return *this;
    }
    
    void Mutex::Unlock()
    {
        LeaveCriticalSection(&this->mutex);
    }
        
    void Mutex::Lock()
    {
        EnterCriticalSection(&this->mutex);
    }

    /* ================ End of Standard Hybrid Mutex Implementation ============================= */

    
    /* ================ Start of Lightweight Hybrid Mutex Implementation ============================= */
    
    SlimMutex::SlimMutex()
    {
        InitializeSRWLock(&this->mutex);
    }

    SlimMutex::SlimMutex(SlimMutex&& other)
    {
        if(this == &other) return;

        other.Lock();

        this->mutex = other.mutex;
        other.mutex = {};

        this->Unlock();
    }

    SlimMutex& SlimMutex::operator=(SlimMutex &&other)
    {
        if(this == &other) return *this;

        other.Lock();

        this->mutex = other.mutex;
        other.mutex = {};

        this->Unlock();

        return *this;
    }

    void SlimMutex::Lock()
    {
        AcquireSRWLockExclusive(&this->mutex);
    }

    void SlimMutex::Unlock()
    {
        ReleaseSRWLockExclusive(&this->mutex);
    }

    /* ================ End of Lightweight Hybrid Mutex Implementation ============================= */

    ConditionVariable::ConditionVariable()
    {
        this->associated_mutex = {};
        InitializeConditionVariable(&this->cond_var);
    }

    void ConditionVariable::Sleep()
    {
        bool result = SleepConditionVariableSRW(&this->cond_var, &this->associated_mutex.mutex, INFINITE, NULL);
        if(result == 0)
        {
            LERROR("Failed to sleep on condition variable");
            PRINT_WINERROR();
        }
    }

    void ConditionVariable::Wake()
    {
        WakeAllConditionVariable(&this->cond_var);
    }

    SlimMutex& ConditionVariable::operator->()
    {
        return this->associated_mutex;
    }
}
