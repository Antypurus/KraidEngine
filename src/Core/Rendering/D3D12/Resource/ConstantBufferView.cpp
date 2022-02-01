#include "ConstantBufferView.h"
#include "Core/Rendering/D3D12/Resource/Resource.h"

#include <Core/Rendering/D3D12/GPUDevice.h>

namespace Kraid
{

    namespace D3D12
    {

        ConstantBufferView::ConstantBufferView(GPUDevice& device, BufferResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width;

           this->descriptor_handle = descriptor_handle;
           device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }

        ConstantBufferView::ConstantBufferView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width;//TODO(Tiago):we need to multiply by the size of each element which needs to be computed from the format

            this->descriptor_handle = descriptor_handle;
            device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }
        
        ConstantBufferView::ConstantBufferView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width * resource.height;//TODO(Tiago):we need to multiply by the size of each element which needs to be computed from the format
            
            this->descriptor_handle = descriptor_handle;
            device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }

        ConstantBufferView::ConstantBufferView(GPUDevice& device, Texture3DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle)
        {
           D3D12_CONSTANT_BUFFER_VIEW_DESC cbuffer_desc = {};
           cbuffer_desc.BufferLocation = resource.resource->GetGPUVirtualAddress();
           cbuffer_desc.SizeInBytes = resource.width * resource.height * resource.depth;//TODO(Tiago): we need to multiply by the size of each element which needs to be calculated from the format

           this->descriptor_handle = descriptor_handle;
           device->CreateConstantBufferView(&cbuffer_desc, descriptor_handle);
        }

    }

}
