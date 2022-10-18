#pragma once

#include <Core/Rendering/D3D12/D3D12SDK.h>

namespace Kraid
{

    namespace D3D12
    {

        struct GPUDevice;

        using namespace Microsoft::WRL;

        enum class CommandQueueType
        {
            Direct = D3D12_COMMAND_LIST_TYPE_DIRECT,
            Compute = D3D12_COMMAND_LIST_TYPE_COMPUTE,
            Copy = D3D12_COMMAND_LIST_TYPE_COPY,
            VideoDecode = D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE,
            VideoProcess = D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS
        };

        struct CommandQueue
        {
            ComPtr<ID3D12CommandQueue> command_queue = nullptr;

            CommandQueue() = default;
            CommandQueue(GPUDevice& device, CommandQueueType type);

            ID3D12CommandQueue* operator->();
        };

    }

}
