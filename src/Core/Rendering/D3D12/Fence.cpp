#include "Fence.h"

#include <Core/Windows.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Utils/Log.h>

namespace Kraid
{
    namespace D3D12
    {

        Fence::Fence(GPUDevice& device, uint64 initial_value)
        {
            D3DCALL(device->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence)), "Fence with initial value of %llu created", initial_value);
        }

        ID3D12Fence1* Fence::operator->()
        {
            return this->fence.Get();
        }

    }

}
