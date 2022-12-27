#pragma once

#include <Core/types.h>
#include <stdlib.h>

namespace Kraid
{
    template<typename T>
    class CircularBuffer
    {
    public:
        T* ring_buffer = nullptr;
        uint64 head = 0;
        uint64 tail = 0;
        uint64 capacity = 0;
        uint64 size = 0;

    public:
        CircularBuffer(uint64 capacity)
        {
            this->capacity = capacity;
            this->size = capacity * sizeof(T);
            this->ring_buffer = (T*) malloc(capacity * sizeof(T));
            memset(this->ring_buffer, 0, this->capacity * sizeof(T));
        }

        void Push(T value)
        {
            return;
        }

        T* Data()
        {
            return ring_buffer;
        }

    };

}
