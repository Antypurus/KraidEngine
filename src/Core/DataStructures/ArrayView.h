#pragma once

#include <Core/types.h>
#include <vector>

#include "Buffer.h"

namespace Kraid
{

    template<typename T>
    struct ArrayView
    {
        T* data;
        uint64 size;

        ArrayView(const std::vector<T>& vector)
        {
            this->data = vector.data();
            this->size = vector.size();
        }

        ArrayView(const Buffer& buffer)
        {
            this->data = buffer.data;
            this->size = buffer.size;
        }

        ArrayView(const T* buffer, uint64 size)
        {
            this->data = buffer;
            this->size = size;
        }

        inline uint64 GetSize()
        {
            return this->size;
        }

        inline T* GetArray()
        {
            return this->data;
        }

        inline T& operator[](const uint64 index)
        {
            return this->data[index];
        }

        inline T at(const uint64 index) const
        {
            return this->data[index];
        }
    };

}
