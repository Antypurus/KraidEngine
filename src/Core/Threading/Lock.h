#pragma once

namespace Kraid
{

    struct Mutex
    {
    public:
        HANDLE mutex_handle = nullptr;

    public:
        Mutex();
        ~Mutex();
    };

}
