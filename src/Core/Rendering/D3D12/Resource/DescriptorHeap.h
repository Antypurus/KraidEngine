#pragma once

#include <Core/Rendering/D3D12/D3D12SDK.h>
#include <Core/Threading/Lock.h>
#include <Core/types.h>

namespace Kraid
{

    namespace D3D12
    {

        struct GPUDevice;

        using namespace Microsoft::WRL;

        struct DescriptorHeap
        {
            ComPtr<ID3D12DescriptorHeap> descriptor_heap = nullptr;
            uint32 descriptor_increment_size = 0;
            uint32 heap_size = 0;

            Mutex allocation_mutex;
            uint32 allocation_index = 0;

            DescriptorHeap() = default;
            ID3D12DescriptorHeap* operator->();

            D3D12_CPU_DESCRIPTOR_HANDLE operator[](const uint64 index);
            D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorByIndex(const uint64 index);
            uint32 AllocateIndex();
        };

        struct RTVDescriptorHeap :public DescriptorHeap
        {
            RTVDescriptorHeap() = default;
            RTVDescriptorHeap(GPUDevice& device, uint32 heap_size);
        };

        struct DSVDescriptorHeap :public DescriptorHeap
        {
            DSVDescriptorHeap() = default;
            DSVDescriptorHeap(GPUDevice& device, uint32 heap_size);
        };


        //NOTE(Tiago): there has to be a version of the sampler heap that is used for submission/rendering
        //all sampler heaps that are needed are merged into that one for rendeing
        struct SamplerDescriptorHeap :public DescriptorHeap
        {
            SamplerDescriptorHeap() = default;
            SamplerDescriptorHeap(GPUDevice& device, uint32 heap_size);
        };

        struct CBV_SRV_UAVDescriptorHeap :public DescriptorHeap
        {
            CBV_SRV_UAVDescriptorHeap() = default;
            CBV_SRV_UAVDescriptorHeap(GPUDevice& device, uint32 heap_size);
        };

    }

}
