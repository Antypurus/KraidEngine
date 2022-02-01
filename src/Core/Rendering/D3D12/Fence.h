#pragma once

#include <Core/Rendering/D3D12/D3D12.h>

#include <Core/types.h>

namespace Kraid
{

    namespace D3D12
    {

        struct GPUDevice;

        using namespace Microsoft::WRL;

        struct Fence
        {
            ComPtr<ID3D12Fence1> fence = nullptr;

            Fence() = default;
            Fence(GPUDevice& device, uint64 initial_value = 0);

            ID3D12Fence1* operator->();
        };

    }

}
