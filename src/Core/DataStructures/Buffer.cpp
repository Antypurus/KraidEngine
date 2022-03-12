#include "Buffer.h"

namespace Kraid
{

    Buffer::Buffer(uint64 size)
    {
        this->size = size;
        this->data = (uint8*)malloc(this->size);
    }

    Buffer::Buffer(const Buffer& other)
    {
        this->data = (uint8*)malloc(other.size);
        memcpy(this->data, other.data, other.size);
        this->size = other.size;
    };
    
    Buffer::Buffer(Buffer&& other)
    {
        if (this->data == other.data) return;

        this->data = other.data;
        other.data = nullptr;
    }
        
    Buffer::Buffer(uint8* data, uint64 size) :data(data), size(size) {}

    Buffer& Buffer::operator=(const Buffer& other)
    {
        if(this->data != nullptr)
        {
            free(this->data);
        }

        this->size = other.size;
        this->data = (uint8*)malloc(this->size);
        memcpy(this->data, other.data, this->size);

        return *this;
    }
        
    Buffer::~Buffer()
    {
        if (data == nullptr) return;

        //free(data);
        data[0] = 't';
        data[1] = 'e';
        data[2] = 's';
        data[4] = 't';
        data = nullptr;
        size = 0;
        LINFO("Freed");
    }

}
