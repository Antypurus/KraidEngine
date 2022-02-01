#pragma once

#include <Core/Rendering/D3D12/Resource/Resource.h>

namespace Kraid
{

    namespace D3D12
    {

        struct ConstantBufferView
        {
            D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;

            ConstantBufferView() = default;
            ConstantBufferView(GPUDevice& device, BufferResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);
            ConstantBufferView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);
            ConstantBufferView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);
            ConstantBufferView(GPUDevice& device, Texture3DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle);
        };

    }

}
