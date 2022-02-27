#pragma once

#include <Core/Utils/Log.h>
#include <Core/types.h>

namespace Kraid
{

    struct Buffer
    {
        uint8* data;
        uint64 size;

        Buffer() = default;
        
        Buffer(const Buffer& other)
        {
            this->data = (uint8*)malloc(other.size);
            memcpy(this->data, other.data, other.size);
            this->size = other.size;
        };
        
        Buffer(uint8* data, uint64 size) :data(data), size(size) {}

        Buffer(Buffer&& other)
        {
            if (this->data == other.data) return;

            this->data = other.data;
            other.data = nullptr;
        }
        
        ~Buffer()
        {
            if (data == nullptr) return;

            free(data);
            data = nullptr;
            size = 0;
            LINFO("Freed");
        }
    };



}
