#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/stdcpp.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/Resource/DepthStencilView.h>
#include <Core/Rendering/D3D12/Resource/RenderTargetView.h>

namespace Kraid
{
    struct Window;

    namespace D3D12
    {
        
        struct GPUDevice;
        struct GraphicsCommandList;

        using namespace Microsoft::WRL;

        struct Swapchain
        {
            ComPtr<IDXGISwapChain4> swapchain = nullptr;
            std::vector<ComPtr<ID3D12Resource2>> render_target_buffers;
            std::vector<RenderTargetView> render_target_views;
            Texture2DResource depth_stencil_buffer;
            DepthStencilView depth_stencil_view;
            RTVDescriptorHeap rtv_heap;
            DSVDescriptorHeap dsv_heap;
            uint8 render_target_count = 2;
            uint8 current_backbuffer = 0;
            uint64 width = 0;
            uint64 height = 0;

        public:
            Swapchain() = default;
            Swapchain(GPUDevice& device, Window& window, GraphicsCommandList& command_list);
            IDXGISwapChain4* operator->();
            void Clear(GraphicsCommandList& command_list);
            void StartFrame(GraphicsCommandList& command_list);
            void EndFrame(GraphicsCommandList& command_list);
            void Present();

        private:
            void CreateSwapchain(GPUDevice& device, Window& window);
            void CreateRenderTargetViews(GPUDevice& device);
            void CreateDepthStencilView(GPUDevice& device, Window& window, GraphicsCommandList&  command_list);
            void SetViewport(GraphicsCommandList& command_list, uint64 width, uint64 height);
            void SetScisorRect(GraphicsCommandList& command_list, uint64 width, uint64 height);
            void SetRenderTarget(GraphicsCommandList& command_list);
        };

    }
}
