#pragma once

#include <Core/types.h>
#include <stdlib.h>

namespace Kraid
{
    template<typename T>
    class CircularBuffer
    {
    private:
        T* m_buffer = nullptr;
        T* m_backbuffer = nullptr;
    public:
        uint64 size = 0;
    public:
        CircularBuffer(uint64 size):size(size)
        {
            m_buffer = (T*)malloc(size * sizeof(T));
            m_backbuffer = (T*)malloc(size * sizeof(T));
            memset(m_buffer, 0, size * sizeof(T));
        }

        T& operator[](const uint64 index)
        {
            return this->m_buffer[index];
        }

        T at(const uint64 index) const
        {
            return this->m_buffer[index];
        }

        T* data() const
        {
            return this->m_buffer;
        }

        void push(T value)
        {
            memcpy(m_backbuffer, m_buffer + 1, (size - 1) * sizeof(T));
            m_backbuffer[size - 1] = value;
            memcpy(m_buffer, m_backbuffer, size * sizeof(T));
        }
    };

}
