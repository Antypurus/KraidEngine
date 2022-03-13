#include "RefCounter.h"

namespace Kraid
{

    void ReferenceCounterInternal::Increment() const
    {
        refcount_mutex.Lock();
        this->refcount++;
        refcount_mutex.Unlock();
    }

    void ReferenceCounterInternal::Decrement() const
    {
        refcount_mutex.Lock();
        this->refcount--;
        refcount_mutex.Unlock();
    }

}
