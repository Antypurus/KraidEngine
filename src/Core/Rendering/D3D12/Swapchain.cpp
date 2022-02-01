#include "Swapchain.h"

#include <Core/Window/Window.h>
#include <Core/Utils/Log.h>

#include "Core/Rendering/D3D12/Resource/Resource.h"
#include <Core/Rendering/D3D12/CommandList.h>
#include "GPUDevice.h"
#include "DXGIFactory.h"

namespace Kraid
{

    namespace D3D12
    {

        Swapchain::Swapchain(GPUDevice& device, Window& window, GraphicsCommandList& command_list)
        {
            this->rtv_heap = RTVDescriptorHeap(device, this->render_target_count);
            this->dsv_heap = DSVDescriptorHeap(device, 1);

            this->CreateSwapchain(device, window);
            this->CreateRenderTargetViews(device);
            this->CreateDepthStencilView(device, window, command_list);
            this->SetViewport(command_list, window);
        }

        void Swapchain::CreateSwapchain(GPUDevice &device, Kraid::Window& window)
        {
            DXGI_SWAP_CHAIN_DESC1 swapchain_description = {};
            swapchain_description.Height = window.height;
            swapchain_description.Width = window.width;
            swapchain_description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//TODO(Tiago): what if we want to use an HDR format
            swapchain_description.BufferCount = this->render_target_count;//TODO(Tiago): what if we want to have triple buffering
            swapchain_description.SampleDesc.Count = 1;
            swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

            ComPtr<IDXGISwapChain1> intermediate_swapchain = nullptr;
            DXGIFactory factory;
            D3DCALL(factory->CreateSwapChainForHwnd(
                        device.direct_command_queue.command_queue.Get(),
                        window.window_handle,
                        &swapchain_description,
                        nullptr,
                        nullptr,
                        intermediate_swapchain.GetAddressOf()),
                    "Created Swapchain");
            D3DCALL(intermediate_swapchain.As(&this->swapchain), "Converted From Swapchain1 to Swapchain4");
        }

        void Swapchain::CreateRenderTargetViews(GPUDevice& device)
        {
            this->render_target_buffers.resize(this->render_target_count);
            for(uint8 i = 0; i < this->render_target_count; ++i)
            {
               D3DCALL(this->swapchain->GetBuffer(i, IID_PPV_ARGS(&this->render_target_buffers[i])), "Obtained Render Target Buffer %d", i);
               device->CreateRenderTargetView(this->render_target_buffers[i].Get(), nullptr, this->rtv_heap[i]);
            }
        }

        void Swapchain::CreateDepthStencilView(GPUDevice& device, Window& window, GraphicsCommandList& command_list)
        {
            this->depth_stencil_buffer = Texture2DResource(device,
                    window.width, window.height,
                    DXGI_FORMAT_D16_UNORM,
                    0.0f,1.0f,
                    ResourceState::Common,
                    ResourceLayout::Unknown,
                    ResourceFlags::AllowDepthStencil);
            this->depth_stencil_view = DepthStencilView(device, this->depth_stencil_buffer, this->dsv_heap[0]);
            this->depth_stencil_buffer.TransitionStateTo(ResourceState::DepthWrite, command_list);
        }


        void Swapchain::SetViewport(GraphicsCommandList &command_list, Window& window)
        {
            D3D12_VIEWPORT viewport = {};
            viewport.Height = window.height;
            viewport.Width = window.width;
            viewport.MaxDepth = 1;
            viewport.MinDepth = 0;
            viewport.TopLeftX =  0;
            viewport.TopLeftY = 0;

            command_list->RSSetViewports(1, &viewport);
        }

        void Swapchain::SetScisorRect(GraphicsCommandList& command_list,  Window& window)
        {
            D3D12_RECT scisor_rect = {};
            scisor_rect.top = 0;
            scisor_rect.bottom = window.height;
            scisor_rect.left = 0;
            scisor_rect.right = window.width;

            command_list->RSSetScissorRects(1, &scisor_rect);
        }

        void Swapchain::SetRenderTarget(GraphicsCommandList& command_list)
        {
            uint8 backbuffer = this->swapchain->GetCurrentBackBufferIndex();
            D3D12_CPU_DESCRIPTOR_HANDLE render_taget_descriptor_handle = this->rtv_heap[backbuffer];
            command_list->OMSetRenderTargets(1, &render_taget_descriptor_handle, 1, &this->depth_stencil_view.descriptor_handle);
        }

        IDXGISwapChain4* Swapchain::operator->()
        {
            return this->swapchain.Get();
        }

    }

}
