#include "DepthStencilView.h"

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>

namespace Kraid
{

    namespace D3D12
    {

        DepthStencilView::DepthStencilView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = resource.width;
            this->height = 0;

            D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
            dsv_desc.Format = format;
            dsv_desc.Flags = D3D12_DSV_FLAG_NONE;//TODO(Tiago): should i add some way to specify these flags?
            dsv_desc.Texture1D.MipSlice = 0;

            device->CreateDepthStencilView(resource.resource.Get(), &dsv_desc, this->descriptor_handle);
        }

        DepthStencilView::DepthStencilView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = resource.width;
            this->height = resource.height;

            D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsv_desc.Format = format;
            dsv_desc.Texture2D.MipSlice = 0;
            dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

            device->CreateDepthStencilView(resource.resource.Get(), &dsv_desc, this->descriptor_handle);
        }

        void DepthStencilView::Clear(GraphicsCommandList& command_list)
        {
            D3D12_RECT clear_rect = {};
            clear_rect.bottom = this->height;
            clear_rect.top = 0;
            clear_rect.left = 0;
            clear_rect.right = this->width;

            command_list->ClearDepthStencilView(this->descriptor_handle, D3D12_CLEAR_FLAG_DEPTH  | D3D12_CLEAR_FLAG_STENCIL, 1, 0, 1, &clear_rect);
        }

    }

}

