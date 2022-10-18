#include "ConstantBufferView.h"

#include "Core/Rendering/D3D12/Resource/Resource.h"
#include <Core/Rendering/D3D12/GPUDevice.h>

namespace Kraid
{

    namespace D3D12
    {
        ConstantBufferView::ConstantBufferView(GPUDevice& device, BufferResource& resource, CBV_SRV_UAVDescriptorHeap& descriptor_heap, uint64 heap_index)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width;

           this->cpu_descriptor_handle = descriptor_heap[heap_index];
           this->gpu_descriptor_handle = descriptor_heap.GetGPUDescriptorByIndex(heap_index);
           device->CreateConstantBufferView(&cbuffer_desc, cpu_descriptor_handle);
        }

        ConstantBufferView::ConstantBufferView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width;//TODO(Tiago):we need to multiply by the size of each element which needs to be computed from the format

            this->cpu_descriptor_handle = descriptor_handle;
            device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }

        ConstantBufferView::ConstantBufferView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width * resource.height;//TODO(Tiago):we need to multiply by the size of each element which needs to be computed from the format

            this->cpu_descriptor_handle = descriptor_handle;
            device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }

        ConstantBufferView::ConstantBufferView(GPUDevice& device, Texture3DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width * resource.height * resource.depth;//TODO(Tiago): we need to multiply by the size of each element which needs to be calculated from the format

           this->cpu_descriptor_handle = descriptor_handle;
           device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }

        void ConstantBufferView::Bind(GraphicsCommandList& command_list, uint64 slot)
        {
            command_list->SetGraphicsRootDescriptorTable(slot, this->gpu_descriptor_handle);
        }

    }

}
