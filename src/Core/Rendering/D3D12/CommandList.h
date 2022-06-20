#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/Rendering/D3D12/CommandQueue.h>

namespace Kraid
{

    namespace D3D12
    {

        struct GPUDevice;
        struct CBV_SRV_UAVDescriptorHeap;
        struct SamplerDescriptorHeap;

        using namespace Microsoft::WRL;

        struct CommandList
        {
            ComPtr<ID3D12GraphicsCommandList6> command_list = nullptr;
            ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
            CommandQueue command_queue;
            CBV_SRV_UAVDescriptorHeap* shader_resource_heap = nullptr;//TODO(Tiago):maybe i need some kind of way to clearly demark references
            SamplerDescriptorHeap* sampler_heap = nullptr;

            CommandList() = default;
            ID3D12GraphicsCommandList6* operator->();
            void Close();
            void Reset();
            void Execute();
            void SetShaderResourceHeap(CBV_SRV_UAVDescriptorHeap& heap);
            void SetSamplerHeap(SamplerDescriptorHeap& heap);

        private:
            void SetDescriptorHeaps();
        };

        struct GraphicsCommandList :public CommandList
        {
           GraphicsCommandList() = default;
           GraphicsCommandList(GPUDevice& device);
        };

        struct CopyCommandList :public CommandList
        {
            CopyCommandList() = default;
            CopyCommandList(GPUDevice& device);
        };

        struct ComputeCommandList :public CommandList
        {
            ComputeCommandList() = default;
            ComputeCommandList(GPUDevice& device);
        };

        struct VideoDecodeCommandList
        {
            ComPtr<ID3D12VideoDecodeCommandList2> command_list = nullptr;
            ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
            CommandQueue command_queue;

            VideoDecodeCommandList() = default;
            VideoDecodeCommandList(GPUDevice& device);

            ID3D12VideoDecodeCommandList2* operator->();
            void Close();
            void Reset();
            void Execute();
        };

        struct VideoProcessCommandList
        {
            ComPtr<ID3D12VideoProcessCommandList2> command_list = nullptr;
            ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
            CommandQueue command_queue;

            VideoProcessCommandList() = default;
            VideoProcessCommandList(GPUDevice& device);

            ID3D12VideoProcessCommandList2* operator->();
            void Close();
            void Reset();
            void Execute();
        };

    }

}
