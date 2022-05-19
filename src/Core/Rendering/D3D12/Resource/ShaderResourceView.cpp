#include "ShaderResourceView.h"

#include <Core/Rendering/D3D12/D3D12.h>
#include <d3d12.h>

namespace Kraid
{

namespace D3D12
{

    ShaderResourceView::ShaderResourceView(
            GPUDevice& device,
            CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
            BufferResource& resource,
            DXGI_FORMAT format):
        cpu_descriptor_handle(heap[heap_index]),
        gpu_descriptor_handle(heap.GetGPUDescriptorByIndex(heap_index))
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv_desc.Format = format;
        srv_desc.Buffer.FirstElement = 0;
        //TODO(Tiago):we might need a way to specify this in the future
        srv_desc.Buffer.NumElements = resource.width;
        srv_desc.Buffer.StructureByteStride = 1;
        srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(resource.resource.Get(), &srv_desc, this->cpu_descriptor_handle);
    }

    ShaderResourceView::ShaderResourceView(
            GPUDevice& device,
            CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
            Texture1DResource& resource,
            DXGI_FORMAT format)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
        srv_desc.Format = format;
        srv_desc.Texture1D.MipLevels = -1;
        srv_desc.Texture1D.MostDetailedMip = 0;
        srv_desc.Texture1D.ResourceMinLODClamp = 0;

        device->CreateShaderResourceView(resource.resource.Get(), &srv_desc, this->cpu_descriptor_handle);
    }

    ShaderResourceView::ShaderResourceView(
            GPUDevice& device,
            CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
            Texture2DResource& resource,
            DXGI_FORMAT format)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Format = format;
        srv_desc.Texture2D.MostDetailedMip = 0;
        srv_desc.Texture2D.ResourceMinLODClamp = 0;
        srv_desc.Texture2D.MipLevels = -1;
        srv_desc.Texture2D.PlaneSlice = 0;

        device->CreateShaderResourceView(resource.resource.Get(), &srv_desc, this->cpu_descriptor_handle);
    }

    ShaderResourceView::ShaderResourceView(
            GPUDevice& device,
            CBV_SRV_UAVDescriptorHeap& heap, uint64 heap_index,
            Texture3DResource& resource,
            DXGI_FORMAT format)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        srv_desc.Format = format;
        srv_desc.Texture3D.MipLevels = -1;
        srv_desc.Texture3D.ResourceMinLODClamp = 0;
        srv_desc.Texture3D.MostDetailedMip = 0;

        device->CreateShaderResourceView(resource.resource.Get(), &srv_desc, this->cpu_descriptor_handle);
    }

    void ShaderResourceView::Bind(GraphicsCommandList& command_list, uint64 slot)
    {
        command_list->SetGraphicsRootDescriptorTable(slot, this->gpu_descriptor_handle);
    }

}

}
