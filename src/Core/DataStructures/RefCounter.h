#pragma once

#include <Core/types.h>
#include <Core/Threading/Lock.h>

namespace Kraid
{

    struct ReferenceCounterInternal
    {
        mutable uint64 refcount = 0;
        mutable Mutex refcount_mutex;

        ReferenceCounterInternal() = default;
        void Increment() const;
        void Decrement() const;

        ReferenceCounterInternal(const ReferenceCounterInternal& other) = delete;
        ReferenceCounterInternal& operator=(const ReferenceCounterInternal& other) = delete;
    };

    struct ReferenceCounter
    {
        ReferenceCounterInternal* refcounter = nullptr;

        ReferenceCounter();
        ReferenceCounter(const ReferenceCounter& other);
        ReferenceCounter& operator=(const ReferenceCounter& other);
        ReferenceCounterInternal* operator->() const;
        ~ReferenceCounter();
    };

}
