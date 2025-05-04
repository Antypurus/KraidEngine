#pragma once

#include <Core/types.h>
#include <Core/stdcpp.h>

#include "Buffer.h"

namespace Kraid
{

    template<typename T>
    class ArrayView
    {
    public:
        const T* const data;
        const uint64 size;
    public:
        ArrayView(const std::vector<T>& vector):
            data(vector.data()),
            size(vector.size()) {}

        ArrayView(const Buffer& buffer):
            data(buffer.data),
            size(buffer.size) {}

        ArrayView(const T* buffer, uint64 size):
            data(buffer),
            size(size) {}

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
