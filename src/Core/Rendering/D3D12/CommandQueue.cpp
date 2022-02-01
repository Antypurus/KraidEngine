#include "CommandQueue.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

#include "GPUDevice.h"

#ifdef LOGGING
    #include <unordered_map>
    #include <string>
    const std::unordered_map<Kraid::D3D12::CommandQueueType, std::string> queue_type_name_table =
    {
        {Kraid::D3D12::CommandQueueType::Direct, "Direct"},
        {Kraid::D3D12::CommandQueueType::Compute, "Compute"},
        {Kraid::D3D12::CommandQueueType::Copy, "Copy"},
        {Kraid::D3D12::CommandQueueType::VideoDecode, "Video Decode"},
        {Kraid::D3D12::CommandQueueType::VideoProcess, "Video Process"}
    };
#endif

namespace Kraid
{

    namespace D3D12
    {

        CommandQueue::CommandQueue(GPUDevice& device, CommandQueueType type)
        {
            D3D12_COMMAND_QUEUE_DESC queue_desc;
            queue_desc.Type = (D3D12_COMMAND_LIST_TYPE)type;
            queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//NOTE(Tiago): honestly not sure what to use here
            queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queue_desc.NodeMask = 0;//NOTE(Tiago): if at some point i want multi gpu then this cant be 0 hardcoded.

            D3DCALL(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&this->command_queue)),"%s Command Queue Created", queue_type_name_table.at(type).c_str());
        }

        ID3D12CommandQueue* CommandQueue::operator->()
        {
            return this->command_queue.Get();
        }

    }

}
