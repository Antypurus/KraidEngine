#include "Fence.h"

#include <Core/Windows.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Utils/Log.h>

namespace Kraid
{
    namespace D3D12
    {

        Fence::Fence(GPUDevice& device, uint64 initial_value)
            :current_value(initial_value)
        {
            D3DCALL(device->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence)), "Fence with initial value of %llu created", initial_value);
        }

        ID3D12Fence1* Fence::operator->()
        {
            return this->fence.Get();
        }

        void Fence::Increment(CommandQueue& command_queue)
        {
            this->current_value++;
            command_queue->Signal(this->fence.Get(), this->current_value);
        }

        uint64 Fence::GetCurrentCompletedValue()
        {
            return this->fence->GetCompletedValue();
        }

        uint64 Fence::GetCurrentValue()
        {
            return this->current_value;
        }

        void Fence::WaitForCompletion()
        {
            while(this->GetCurrentCompletedValue() < this->current_value) {}
        }

    }

}
