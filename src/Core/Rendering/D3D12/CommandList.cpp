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
        
        void CommandList::Close()
        {
            this->command_list->Close();
        }

        void CommandList::Reset()
        {
            this->command_allocator->Reset();
            this->command_list->Reset(this->command_allocator.Get(), nullptr);
        }

        void CommandList::Execute()
        {
            ID3D12CommandList* lists[1] = { this->command_list.Get() };
            this->command_queue->ExecuteCommandLists(1, lists);
        }

        void CommandList::SetShaderResourceHeap(CBV_SRV_UAVDescriptorHeap& heap)
        {
            this->shader_resource_heap = &heap;
            this->SetDescriptorHeaps();
        }

        void CommandList::SetSamplerHeap(SamplerDescriptorHeap& heap)
        {
            this->sampler_heap = &heap;
            this->SetDescriptorHeaps();
        }

        void CommandList::SetDescriptorHeaps()
        {
            if(this->sampler_heap == nullptr && this->shader_resource_heap == nullptr)
            {
                return;
            }
            else if(this->sampler_heap != nullptr && this->shader_resource_heap == nullptr)
            {
                ID3D12DescriptorHeap* heaps[] = {this->sampler_heap->descriptor_heap.Get()};
                this->command_list->SetDescriptorHeaps(1, heaps);
            }
            else if (this->sampler_heap == nullptr && this->shader_resource_heap != nullptr)
            {
                ID3D12DescriptorHeap* heaps[] = {this->shader_resource_heap->descriptor_heap.Get()};
                this->command_list->SetDescriptorHeaps(1, heaps);
            }
            else
            {
                ID3D12DescriptorHeap* heaps[] = {
                    this->shader_resource_heap->descriptor_heap.Get(),
                    this->sampler_heap->descriptor_heap.Get()
                };
                this->command_list->SetDescriptorHeaps(2, heaps);
            }
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
          device.direct_command_allocator.CopyTo(&this->command_allocator);
          this->command_queue = device.direct_command_queue;
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
            device.copy_command_allocator.CopyTo(&this->command_allocator);
            this->command_queue = device.copy_command_queue;
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
            device.computer_comamnd_allocator.CopyTo(&this->command_allocator);
            this->command_queue = device.compute_command_queue;
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
            device.video_decode_command_allocator.CopyTo(&this->command_allocator);
            this->command_queue = device.video_decode_command_queue;
        }

        ID3D12VideoDecodeCommandList2* VideoDecodeCommandList::operator->()
        {
            return this->command_list.Get();
        }

        void VideoDecodeCommandList::Close()
        {
            this->command_list->Close();
        }

        void VideoDecodeCommandList::Reset()
        {
            this->command_allocator->Reset();
            this->command_list->Reset(this->command_allocator.Get());
        }
        
        void VideoDecodeCommandList::Execute()
        {
            ID3D12CommandList* lists[1] = { this->command_list.Get() };
            this->command_queue->ExecuteCommandLists(1, lists);
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
            device.video_process_command_allocator.CopyTo(&this->command_allocator);
            this->command_queue = device.video_process_command_queue;
        }

        ID3D12VideoProcessCommandList2* VideoProcessCommandList::operator->()
        {
            return this->command_list.Get();
        }

        void VideoProcessCommandList::Close()
        {
            this->command_list->Close();
        }

        void VideoProcessCommandList::Reset()
        {
            this->command_allocator->Reset();
            this->command_list->Reset(this->command_allocator.Get());
        }
        
        void VideoProcessCommandList::Execute()
        {
            ID3D12CommandList* lists[1] = { this->command_list.Get() };
            this->command_queue->ExecuteCommandLists(1, lists);
        }

    }

}
