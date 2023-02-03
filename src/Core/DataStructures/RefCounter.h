#pragma once

#include <Core/types.h>
#include <Core/Threading/Lock.h>

namespace Kraid
{

    class ReferenceCounter
    {
    private:
        mutable uint64* m_refcounter = nullptr;
        mutable Mutex* m_refcounter_mutex = nullptr;
    public:
        ReferenceCounter();
        ReferenceCounter(const ReferenceCounter& other);
        ReferenceCounter& operator=(const ReferenceCounter& other);
        uint64* operator->() const;

        void Increment() const;
        void Decrement() const;

        //NOTE(Tiago):when the reference counted object decrements the object, only the last one to decrement will have a null pointer,
        //which means that only that threads copy of the refcounter will return on this, ensuring that only one thread tries to release
        //their data.
        bool inline ShouldFree() const
        {
            return this->m_refcounter==nullptr;
        }
    };

}
