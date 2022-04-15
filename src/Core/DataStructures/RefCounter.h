#pragma once

#include <Core/types.h>
#include <Core/Threading/Lock.h>

namespace Kraid
{

    struct ReferenceCounterInternal
    {
        mutable uint64 refcount = 0;

        ReferenceCounterInternal() = default;
        ReferenceCounterInternal(const ReferenceCounterInternal& other) = delete;
        ReferenceCounterInternal& operator=(const ReferenceCounterInternal& other) = delete;
    };

    struct ReferenceCounter
    {
        mutable ReferenceCounterInternal* refcounter = nullptr;
        mutable Mutex* refcounter_mutex = nullptr;

        ReferenceCounter();
        ReferenceCounter(const ReferenceCounter& other);
        ReferenceCounter& operator=(const ReferenceCounter& other);
        ReferenceCounterInternal* operator->() const;

        void Increment() const;
        void Decrement() const;

        //NOTE(Tiago):when the reference counted object decrements the object, only the last one to decrement will have a null pointer,
        //which means that only that threads copy of the refcounter will return on this, ensuring that only one thread tries to release
        //their data.
        bool inline ShouldFree() const 
        {
            return this->refcounter==nullptr && this->refcounter_mutex == nullptr;
        }
    };

}
