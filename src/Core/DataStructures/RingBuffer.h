#pragma once

#include <Core/types.h>
#include <stdlib.h>

namespace Kraid
{
        template<typename T>
    class CircularBuffer
    {
    public:
        T* buffer = nullptr;
        T* backbuffer = nullptr;
        uint64 size = 0;
    public:
        CircularBuffer(uint64 size):size(size)
        {
            buffer = (T*)malloc(size * sizeof(T));
            backbuffer = (T*)malloc(size * sizeof(T));
            memset(buffer, 0, size * sizeof(T));
        }

        void push(T value)
        {
            memcpy(backbuffer, buffer + 1, (size - 1) * sizeof(T));
            backbuffer[size - 1] = value;
            memcpy(buffer, backbuffer, size * sizeof(T));
        }
    };

}
