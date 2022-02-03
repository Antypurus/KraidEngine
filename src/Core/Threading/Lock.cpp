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

}
