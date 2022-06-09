#include "GPUDevice.h"

#include <Core/stdcpp.h>
#include <Core/Windows.h>
#include <Core/Utils/Log.h>

#include "CommandList.h"

namespace Kraid
{

    namespace D3D12
    {

        const D3D_FEATURE_LEVEL feature_level_query_order_list[5] =
        {
            D3D_FEATURE_LEVEL_12_2,
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };

        const std::unordered_map<D3D_FEATURE_LEVEL, std::string> feature_level_lookup =
        {
            {D3D_FEATURE_LEVEL_11_0, "11.0"},
            {D3D_FEATURE_LEVEL_11_1, "11.1"},
            {D3D_FEATURE_LEVEL_12_0, "12.0"},
            {D3D_FEATURE_LEVEL_12_1, "12.1"},
            {D3D_FEATURE_LEVEL_12_2, "12.2"},
        };

        GPUDevice::GPUDevice(uint8 gpu_index)
        {
#ifndef NDEBUG
            this->EnableDebugLayer();
#endif

            this->CreateD3D12Device(gpu_index);
            this->QueryDescriptorSizes();
            this->CreateCommandAllocator();
            this->CreateCommandQueues();

            this->shader_resource_heap = CBV_SRV_UAVDescriptorHeap(*this, 2048);
            this->sampler_descriptior_heap = SamplerDescriptorHeap(*this, 2048); //NOTE(Tiago):2048 is the max for sampler heaps
        }

        GPUDevice::GPUDevice(IDXGIAdapter4* adapater)
        {
#ifndef NDEBUG
            this->EnableDebugLayer();
#endif

            this->CreateD3D12DeviceFromAdapater(adapater);
            //TODO(Tiago): add missing method invocations that are in the constructor above
        }

        inline void GPUDevice::CreateD3D12Device(uint8 gpu_index)
        {
#ifndef NDEBUG
            this->factory.LogGPUList();
#endif
            std::vector<ComPtr<IDXGIAdapter4>> gpu_list = this->factory.GetGPUList();
            this->CreateD3D12DeviceFromAdapater(gpu_list[gpu_index].Get());
        }

        inline void GPUDevice::CreateD3D12DeviceFromAdapater(IDXGIAdapter4* adapter)
        {
            constexpr uint64 feature_level_count = sizeof(feature_level_query_order_list) / sizeof(D3D_FEATURE_LEVEL);
            for(uint8 i = 0; i < feature_level_count; ++i)
            {
                HRESULT res = D3D12CreateDevice(adapter, feature_level_query_order_list[i], IID_PPV_ARGS(&this->device));
                if(res != S_OK)
                {
                    continue;
                }
                else
                {
                    this->initialized = true;
                    this->feature_level = feature_level_query_order_list[i];
                    LSUCCESS("Created D3D12 device with feature level %s", feature_level_lookup.at(this->feature_level).c_str());
                    break;
                }
            }
            if (!this->initialized)
            {
                LERROR("Failed to created D3D12 device, no supported feature level was found");
            }
        }
     
        inline void GPUDevice::QueryDescriptorSizes()
        {
            //query render target descriptor size
            this->descriptor_sizes.rtv = this->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            //query SRV/UAV/CBV descriptor size
            this->descriptor_sizes.cbv_srv_uav = this->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            //query sample descriptor size
            this->descriptor_sizes.sampler = this->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            //query depth stencil buffer descriptor size
            this->descriptor_sizes.dsv = this->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            //TODO(Tiago): log the sizes of the various things
        }

        inline void GPUDevice::CreateCommandAllocator()
        {
            D3DCALL(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->direct_command_allocator)), "Created Direct Command Allocator");
            D3DCALL(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&this->bundle_command_allocator)), "Created Bundle Command Allocator");
            D3DCALL(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&this->copy_command_allocator)), "Created Copy Command Allocator");
            D3DCALL(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&this->computer_comamnd_allocator)), "Create Compute Command Allocator");
            D3DCALL(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE, IID_PPV_ARGS(&this->video_decode_command_allocator)),"Creted Video Decode Command Allocator");
            D3DCALL(this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS, IID_PPV_ARGS(&this->video_process_command_allocator)), "Create Video Process Command Allocator");
        }

        inline void GPUDevice::CreateCommandQueues()
        {
            this->direct_command_queue = CommandQueue(*this, CommandQueueType::Direct);
            this->compute_command_queue = CommandQueue(*this, CommandQueueType::Compute);
            this->copy_command_queue = CommandQueue(*this, CommandQueueType::Copy);
            this->video_decode_command_queue = CommandQueue(*this, CommandQueueType::VideoDecode);
            this->video_process_command_queue = CommandQueue(*this, CommandQueueType::VideoProcess);
        }

        ID3D12Device8* GPUDevice::operator->()
        {
            return this->device.Get();
        }

        GraphicsCommandList GPUDevice::CreateGraphicsCommandList()
        {
            return GraphicsCommandList(*this);
        }

        CopyCommandList GPUDevice::CreateCopyCommandList()
        {
            return CopyCommandList(*this);
        }

        ComputeCommandList GPUDevice::CreateComputeCommandList()
        {
            return ComputeCommandList(*this);
        }

        VideoProcessCommandList GPUDevice::CreateVideoProcessCommandList()
        {
            return VideoProcessCommandList(*this);
        }

        VideoDecodeCommandList GPUDevice::CreateVideoDecodeCommandList()
        {
            return VideoDecodeCommandList(*this);
        }

    //are where debug only functions go
#ifndef NDEBUG
        inline void GPUDevice::EnableDebugLayer()
        {
            D3DCALL(D3D12GetDebugInterface(IID_PPV_ARGS(&this->debug_layer)),"D3D12 Debug Interface Created");
            this->debug_layer->EnableDebugLayer();
            //the following options is very heavy according to the docs and as such i might need to use
            //some form of selective toggle for its usage instead of having it on all the time
            //this->debug_layer->SetEnableGPUBasedValidation(true);
            LSUCCESS("D3D12 Debugging Enabled");
        }
#endif

    }

}
