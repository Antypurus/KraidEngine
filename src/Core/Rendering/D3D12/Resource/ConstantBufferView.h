#pragma once

#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>

namespace Kraid
{

    namespace D3D12
    {

        struct ConstantBufferView
        {
            D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle;
            D3D12_GPU_DESCRIPTOR_HANDLE gpu_descriptor_handle;

            ConstantBufferView() = default;
            ConstantBufferView(GPUDevice& device, BufferResource& resource, CBV_SRV_UAVDescriptorHeap& descriptor_heap, uint64 heap_index);
            ConstantBufferView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);
            ConstantBufferView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);
            ConstantBufferView(GPUDevice& device, Texture3DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);

            void Bind(GraphicsCommandList& command_list, uint64 slot);
        };

    }

}
