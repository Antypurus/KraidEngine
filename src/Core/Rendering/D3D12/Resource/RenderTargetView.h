#pragma  once

#include <Core/types.h>

namespace Kraid
{

    namespace D3D12
    {

        struct CommandList;
        struct GPUDevice;
        struct BufferResource;
        struct Texture1DResource;
        struct Texture2DResource;
        struct Texture3DResource;

        struct RenderTargetView
        {
            D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
            uint32 width = 0;
            uint32 height = 0;

            RenderTargetView() = default;
            RenderTargetView(GPUDevice& device, BufferResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
            RenderTargetView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
            RenderTargetView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
            RenderTargetView(GPUDevice& device, Texture3DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
            RenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle,uint32 width, uint32 height);

            void Clear(CommandList& command_list);
        };

    }

}
