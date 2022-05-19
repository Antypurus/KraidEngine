#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/CommandList.h>

namespace Kraid
{

namespace D3D12
{

    class ShaderResourceView
    {
    public:
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpu_descriptor_handle;

    public:
        ShaderResourceView() = default;
        ShaderResourceView(
                GPUDevice& device,
                CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
                BufferResource& resource,
                DXGI_FORMAT format);
        ShaderResourceView(
                GPUDevice& device,
                CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
                Texture1DResource& resource,
                DXGI_FORMAT format);
        ShaderResourceView(
                GPUDevice& device,
                CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
                Texture2DResource& resource,
                DXGI_FORMAT format);
        ShaderResourceView(
                GPUDevice& device,
                CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
                Texture3DResource& resource,
                DXGI_FORMAT format);

        void Bind(GraphicsCommandList& command_list, uint64 slot = 0);
    };

}

}
