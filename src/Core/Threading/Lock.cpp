#include "Lock.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

namespace Kraid
{

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

/*
    ConditionVariable::ConditionVariable()
    {
        InitializeConditionVariable(&this->cond_var);
    }

    void ConditionVariable::Sleep()
    {
        bool result = SleepConditionVariableSRW(&this->cond_var, this->associated_mutex.mutex_handle, INFINITE, NULL);
        if(result == 0)
        {
            LERROR("Failed to sleep on condition variable");
            PRINT_WINERROR();
        }
    }

    Mutex& ConditionVariable::operator->()
    {
        return this->associated_mutex;
    }
*/
}
