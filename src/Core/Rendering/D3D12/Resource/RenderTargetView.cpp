#include "RenderTargetView.h"

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Utils/Log.h>

#include "Resource.h"

namespace Kraid
{

    namespace D3D12
    {

        RenderTargetView::RenderTargetView(GPUDevice& device, BufferResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = resource.width;
            this->height = 0;

            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_BUFFER;
            rtv_desc.Format = format;
            rtv_desc.Buffer.FirstElement = 0;
            rtv_desc.Buffer.NumElements = this->width;

            device->CreateRenderTargetView(resource.resource.Get(), &rtv_desc, this->descriptor_handle);
        }

        RenderTargetView::RenderTargetView(GPUDevice& device, Texture1DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = resource.width;
            this->height = 0;
            
            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
            rtv_desc.Format = format;
            rtv_desc.Texture1D.MipSlice = 0;

            device->CreateRenderTargetView(resource.resource.Get(), &rtv_desc, this->descriptor_handle);
        }

        RenderTargetView::RenderTargetView(GPUDevice& device, Texture2DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = resource.width;
            this->height = resource.height;

            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtv_desc.Format = format;
            rtv_desc.Texture2D.MipSlice = 0;
            rtv_desc.Texture2D.PlaneSlice = 0;

            device->CreateRenderTargetView(resource.resource.Get(), &rtv_desc, this->descriptor_handle);
        }

        RenderTargetView::RenderTargetView(GPUDevice& device, Texture3DResource& resource, D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, DXGI_FORMAT format)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = resource.width;
            this->height = resource.height;

            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
            rtv_desc.Format = format;
            rtv_desc.Texture3D.FirstWSlice = 0;
            rtv_desc.Texture3D.MipSlice = 0;
            rtv_desc.Texture3D.WSize = resource.depth;

            device->CreateRenderTargetView(resource.resource.Get(), &rtv_desc, this->descriptor_handle);
        }

        RenderTargetView::RenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle, uint32 width, uint32 height)
        {
            this->descriptor_handle = descriptor_handle;
            this->width = width;
            this->height = height;
        }

        void RenderTargetView::Clear(CommandList& command_list)
        {
            D3D12_RECT resource_clear_rect = {};
            resource_clear_rect.bottom = this->height;
            resource_clear_rect.left = 0;
            resource_clear_rect.top = 0;
            resource_clear_rect.right = this->width;

            float clear_value[4] = {0,0,0,0};

            command_list->ClearRenderTargetView(this->descriptor_handle, clear_value, 1, &resource_clear_rect); 
        }

    }

}
