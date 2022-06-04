#include "Lock.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

namespace Kraid
{

    Mutex::Mutex()
    {
        this->mutex_handle = CreateMutexW(nullptr, false, nullptr);
        if(this->mutex_handle == nullptr)
        {
            PRINT_WINERROR();
            return;
        }
    }

    Mutex::~Mutex()
    {
        //this->Unlock();
        //CloseHandle(this->mutex_handle);
    }

    void Mutex::Lock()
    {
        auto result = WaitForSingleObject(this->mutex_handle, INFINITE);
        while( result != WAIT_OBJECT_0)
        {
            result = WaitForSingleObject(this->mutex_handle, INFINITE);
        }
    }

    void Mutex::Unlock()
    {
        bool result = ReleaseMutex(this->mutex_handle);
        if(result == false)
        {
            PRINT_WINERROR();
        }
    }

}
