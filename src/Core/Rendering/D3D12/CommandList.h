#pragma once

#include <Core/Rendering/D3D12/D3D12SDK.h>
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
        public:
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
        public:
           GraphicsCommandList();
           GraphicsCommandList(GPUDevice& device);
        private:
           void CreateCommandList(GPUDevice& device);
        };

        struct CopyCommandList :public CommandList
        {
        public:
            CopyCommandList();
            CopyCommandList(GPUDevice& device);
        private:
            void CreateCommandList(GPUDevice& device);
        };

        struct ComputeCommandList :public CommandList
        {
        public:
            ComputeCommandList();
            ComputeCommandList(GPUDevice& device);
        private:
            void CreateCommandList(GPUDevice& device);
        };

        struct VideoDecodeCommandList
        {
        public:
            ComPtr<ID3D12VideoDecodeCommandList2> command_list = nullptr;
            ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
            CommandQueue command_queue;

            VideoDecodeCommandList();
            VideoDecodeCommandList(GPUDevice& device);

            ID3D12VideoDecodeCommandList2* operator->();
            void Close();
            void Reset();
            void Execute();
        private:
            void CreateCommandList(GPUDevice& device);
        };

        struct VideoProcessCommandList
        {
        public:
            ComPtr<ID3D12VideoProcessCommandList2> command_list = nullptr;
            ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
            CommandQueue command_queue;

            VideoProcessCommandList();
            VideoProcessCommandList(GPUDevice& device);

            ID3D12VideoProcessCommandList2* operator->();
            void Close();
            void Reset();
            void Execute();
        private:
            void CreateCommandList(GPUDevice& device);
        };

    }

}
