#include "Swapchain.h"

#include <Core/Window/Window.h>
#include <Core/Utils/Log.h>

#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Window/Window.h>

#define rad(x) (x*DirectX::XM_PI)/180.0f

namespace Kraid
{

    namespace D3D12
    {

        Swapchain::Swapchain(Window& window, GraphicsCommandList& command_list)
        {
            this->rtv_heap = RTVDescriptorHeap(GPUDevice::Instance(), this->render_target_count);
            this->dsv_heap = DSVDescriptorHeap(GPUDevice::Instance(), 1);
            //TODO(Tiago):needs to be cleaned up to update when we change the window size
            this->height = window.height;
            this->width = window.width;

            this->CreateSwapchain(GPUDevice::Instance(), window);
            this->CreateRenderTargetViews(GPUDevice::Instance());
            this->CreateDepthStencilView(GPUDevice::Instance(), command_list);
            this->SetViewport(command_list, width, height);

            window.RegisterWindowResizeEventCallback([this](uint32 new_width, uint32 new_height) -> void
                    {
                        if(new_width == 0 || new_height == 0) return;

                        this->width = new_width;
                        this->height = new_height;

                        this->swapchain_should_resize = true;

                        return;
                    });
        }

        Swapchain::Swapchain(GPUDevice& device, Window& window, GraphicsCommandList& command_list)
        {
            this->rtv_heap = RTVDescriptorHeap(device, this->render_target_count);
            this->dsv_heap = DSVDescriptorHeap(device, 1);
            //TODO(Tiago):needs to be cleaned up to update when we change the window size
            this->height = window.height;
            this->width = window.width;

            this->CreateSwapchain(device, window);
            this->CreateRenderTargetViews(device);
            this->CreateDepthStencilView(device, command_list);
            this->SetViewport(command_list, width, height);
        }

        void Swapchain::CreateSwapchain(GPUDevice &device, Kraid::Window& window)
        {
            DXGI_SWAP_CHAIN_DESC1 swapchain_description = {};
            swapchain_description.Height = this->height;
            swapchain_description.Width = this->width;
            swapchain_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//TODO(Tiago): what if we want to use an HDR format
            swapchain_description.BufferCount = this->render_target_count;//TODO(Tiago): what if we want to have triple buffering
            swapchain_description.SampleDesc.Count = 1;
            swapchain_description.SampleDesc.Quality = 0;
            swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapchain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
            swapchain_description.Stereo = false;
            swapchain_description.Scaling = DXGI_SCALING_STRETCH;

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
            factory->MakeWindowAssociation(window.window_handle, DXGI_MWA_NO_ALT_ENTER);
            D3DCALL(intermediate_swapchain.As(&this->swapchain), "Converted From Swapchain1 to Swapchain4");
        }

        void Swapchain::ResizeSwapchain(GPUDevice& device, GraphicsCommandList& command_list, uint32 width, uint32 height)
        {
            for(uint32 i = 0; i < this->render_target_count; ++i)
            {
                this->render_target_buffers[i].Reset();
            }

            D3DCALL(this->swapchain->ResizeBuffers(
                    this->render_target_count,
                    this->width, this->height,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING)
                , "Resized Swapchain Buffers");

            this->CreateRenderTargetViews(GPUDevice::Instance());
            this->CreateDepthStencilView(GPUDevice::Instance(), command_list);
            this->current_backbuffer = this->swapchain->GetCurrentBackBufferIndex();
        }

        void Swapchain::CreateRenderTargetViews(GPUDevice& device)
        {
            this->render_target_views.clear();
            this->render_target_buffers.resize(this->render_target_count);
            for(uint8 i = 0; i < this->render_target_count; ++i)
            {
               D3DCALL(this->swapchain->GetBuffer(i, IID_PPV_ARGS(&this->render_target_buffers[i])), "Obtained Render Target Buffer %d", i);
               device->CreateRenderTargetView(this->render_target_buffers[i].Get(), nullptr, this->rtv_heap[i]);
               this->render_target_views.emplace_back(this->rtv_heap[i], this->width, this->height);
            }
        }

        void Swapchain::CreateDepthStencilView(GPUDevice& device, GraphicsCommandList& command_list)
        {
            this->depth_stencil_buffer = Texture2DResource(device,
                    this->width, this->height,
                    DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
                    1.0f,0.0f,
                    ResourceState::Common,
                    ResourceLayout::Unknown,
                    ResourceFlags::AllowDepthStencil);
            this->depth_stencil_view = DepthStencilView(device, this->depth_stencil_buffer, this->dsv_heap[0]);
            this->depth_stencil_buffer.TransitionStateTo(ResourceState::DepthWrite, command_list);
        }

        void Swapchain::Clear(GraphicsCommandList& command_list)
        {
            this->render_target_views[this->current_backbuffer].Clear(command_list);
            this->depth_stencil_view.Clear(command_list);
        }

        void Swapchain::StartFrame(GraphicsCommandList& command_list)
        {
            if(this->swapchain_should_resize)
            {
                this->ResizeSwapchain(GPUDevice::Instance(), command_list, this->width, this->height);
                this->swapchain_should_resize = false;
            }


            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.pResource = this->render_target_buffers[this->current_backbuffer].Get();
            command_list->ResourceBarrier(1, &barrier);

            this->Clear(command_list);
            this->SetRenderTarget(command_list);
            this->SetViewport(command_list, this->width, this->height);
            this->SetScisorRect(command_list, this->width, this->height);
        }

        void Swapchain::Present()
        {
            this->swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
            this->current_backbuffer = this->current_backbuffer==1?0:1;
        }

        void Swapchain::EndFrame(GraphicsCommandList& command_list)
        {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.pResource = this->render_target_buffers[this->current_backbuffer].Get();
            command_list->ResourceBarrier(1, &barrier);
        }

        void Swapchain::SetViewport(GraphicsCommandList &command_list, uint64 width, uint64 height)
        {
            D3D12_VIEWPORT viewport = {};
            viewport.Height = height;
            viewport.Width = width;
            viewport.MaxDepth = 1;
            viewport.MinDepth = 0;
            viewport.TopLeftX =  0;
            viewport.TopLeftY = 0;

            command_list->RSSetViewports(1, &viewport);
        }

        void Swapchain::SetScisorRect(GraphicsCommandList& command_list, uint64 width, uint64 height)
        {
            D3D12_RECT scisor_rect = {};
            scisor_rect.top = 0;
            scisor_rect.bottom = height;
            scisor_rect.left = 0;
            scisor_rect.right = width;

            command_list->RSSetScissorRects(1, &scisor_rect);
        }

        void Swapchain::SetRenderTarget(GraphicsCommandList& command_list)
        {
            uint8 backbuffer = this->current_backbuffer;
            D3D12_CPU_DESCRIPTOR_HANDLE render_taget_descriptor_handle = this->rtv_heap[backbuffer];
            command_list->OMSetRenderTargets(1, &render_taget_descriptor_handle, 1, &this->depth_stencil_view.descriptor_handle);
        }

        IDXGISwapChain4* Swapchain::operator->()
        {
            return this->swapchain.Get();
        }

        XMMATRIX Swapchain::ProjectionMatrix(float FoV) const
        {
            return XMMatrixPerspectiveFovRH(rad(FoV), (double)this->width / (double)this->height, 0.001f, 1000.0f);
        }

    }

}
