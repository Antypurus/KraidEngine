#include "RefCounter.h"

namespace Kraid
{

    ReferenceCounter::ReferenceCounter()
    {
        this->m_refcounter = new uint64(0);
        this->m_refcounter_mutex = new Mutex();
    }

    ReferenceCounter::ReferenceCounter(const ReferenceCounter& other)
    {
        this->m_refcounter = other.m_refcounter;
        this->m_refcounter_mutex = other.m_refcounter_mutex;
        this->Increment();
    }

    ReferenceCounter& ReferenceCounter::operator=(const ReferenceCounter& other)
    {
        if(this == &other) return *this;

        this->m_refcounter = other.m_refcounter;
        this->m_refcounter_mutex = other.m_refcounter_mutex;
        this->Increment();

        return *this;
    }

    uint64* ReferenceCounter::operator->() const
    {
        return this->m_refcounter;
    }

    void ReferenceCounter::Increment() const
    {
        this->m_refcounter_mutex->Lock();
        (*this->m_refcounter)++;
        this->m_refcounter_mutex->Lock();
    }

    void ReferenceCounter::Decrement() const
    {
        bool should_free_mutex = false;

        this->m_refcounter_mutex->Lock();

        (*this->m_refcounter)--;
        if(*this->m_refcounter == 0)
        {
            free(this->m_refcounter);
            this->m_refcounter = nullptr;
            should_free_mutex = true;
        }

        this->m_refcounter_mutex->Unlock();

        if(should_free_mutex)
        {
            free(this->m_refcounter_mutex);
            this->m_refcounter_mutex = nullptr;
        }
    }

}
