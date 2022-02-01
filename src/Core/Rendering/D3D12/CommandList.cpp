#include "CommandList.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

#include "GPUDevice.h"

namespace Kraid
{

    namespace D3D12
    {

        ID3D12GraphicsCommandList6* CommandList::operator->()
        {
            return this->command_list.Get();
        }

        GraphicsCommandList::GraphicsCommandList(GPUDevice& device)
        {
          D3DCALL(device->CreateCommandList(0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                device.direct_command_allocator.Get(),
                nullptr, 
                IID_PPV_ARGS(&this->command_list)), 
            "Graphics Command List Created");
          this->command_list->Close();
          this->command_list->Reset(device.direct_command_allocator.Get(), nullptr);
        }

        CopyCommandList::CopyCommandList(GPUDevice& device)
        {
            D3DCALL(device->CreateCommandList(0,
                D3D12_COMMAND_LIST_TYPE_COPY,
                device.copy_command_allocator.Get(),
                nullptr,
                IID_PPV_ARGS(&this->command_list)),
            "Copy Command List Created");
            this->command_list->Close();
            this->command_list->Reset(device.copy_command_allocator.Get(), nullptr);
        }

        ComputeCommandList::ComputeCommandList(GPUDevice& device)
        {
            D3DCALL(device->CreateCommandList(0, 
                D3D12_COMMAND_LIST_TYPE_COMPUTE, 
                device.computer_comamnd_allocator.Get(),
                nullptr,
                IID_PPV_ARGS(&this->command_list)),
            "Compute Command List Created");
            this->command_list->Close();
        }

        VideoDecodeCommandList::VideoDecodeCommandList(GPUDevice& device)
        {
            D3DCALL(device->CreateCommandList(0,
                        D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE,
                        device.video_decode_command_allocator.Get(), 
                        nullptr,
                        IID_PPV_ARGS(&this->command_list)),
                    "Video Decode Command List Create");
            this->command_list->Close();
        }

        ID3D12VideoDecodeCommandList2* VideoDecodeCommandList::operator->()
        {
            return this->command_list.Get();
        }

        VideoProcessCommandList::VideoProcessCommandList(GPUDevice& device)
        {
            D3DCALL(device->CreateCommandList(0,
                        D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS,
                        device.video_process_command_allocator.Get(),
                        nullptr,
                        IID_PPV_ARGS(&this->command_list)),
                    "Video Process Command List Created");
            this->command_list->Close();
        }

        ID3D12VideoProcessCommandList2* VideoProcessCommandList::operator->()
        {
            return this->command_list.Get();
        }

    }

}
