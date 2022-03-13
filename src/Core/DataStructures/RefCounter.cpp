#include "RefCounter.h"

namespace Kraid
{

    ReferenceCounter::ReferenceCounter()
    {
        this->refcounter = new ReferenceCounterInternal();
        this->refcounter_mutex = new Mutex();
    }

    ReferenceCounter::ReferenceCounter(const ReferenceCounter& other)
    {
        this->refcounter = other.refcounter;
        this->refcounter_mutex = other.refcounter_mutex;
        this->Increment();
    }

    ReferenceCounter& ReferenceCounter::operator=(const ReferenceCounter& other)
    {
        if(this == &other) return *this;

        this->refcounter = other.refcounter;
        this->refcounter_mutex = other.refcounter_mutex;
        this->Increment();

        return *this;
    }

    ReferenceCounterInternal* ReferenceCounter::operator->() const
    {
        return this->refcounter;
    }

    void ReferenceCounter::Increment() const
    {
        this->refcounter_mutex->Lock();
        this->refcounter->refcount++;
        this->refcounter_mutex->Lock();
    }

    void ReferenceCounter::Decrement() const
    {
        bool should_free_mutex = false;

        this->refcounter_mutex->Lock();

        this->refcounter->refcount--;
        if(this->refcounter_mutex == 0)
        {
            free(this->refcounter);
            this->refcounter = nullptr;
            should_free_mutex = true;
        }

        this->refcounter_mutex->Unlock();

        if(should_free_mutex)
        {
            free(this->refcounter_mutex);
            this->refcounter_mutex = nullptr;
        }
    }

}
