#pragma once

#include <Core/Rendering/D3D12/D3D12SDK.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/Resource/DepthStencilView.h>
#include <Core/Rendering/D3D12/Resource/RenderTargetView.h>
#include <vector>

namespace Kraid
{
    struct Window;

    namespace D3D12
    {

        struct GPUDevice;
        struct GraphicsCommandList;

        using namespace Microsoft::WRL;
        using namespace DirectX;

        struct Swapchain
        {
            ComPtr<IDXGISwapChain4> swapchain = nullptr;
            std::vector<ComPtr<ID3D12Resource2>> render_target_buffers;
            std::vector<RenderTargetView> render_target_views;
            Texture2DResource depth_stencil_buffer;
            DepthStencilView depth_stencil_view;
            RTVDescriptorHeap rtv_heap;
            DSVDescriptorHeap dsv_heap;
            uint32 width = 0;
            uint32 height = 0;
            uint8 render_target_count = 2;
            uint8 current_backbuffer = 0;
            volatile bool swapchain_should_resize = false;


        public:
            Swapchain() = default;
            Swapchain(Window& window, GraphicsCommandList& command_list);
            Swapchain(GPUDevice& device, Window& window, GraphicsCommandList& command_list);
            IDXGISwapChain4* operator->();
            void Clear(GraphicsCommandList& command_list);
            void StartFrame(GraphicsCommandList& command_list);
            void EndFrame(GraphicsCommandList& command_list);
            void Present();
            XMMATRIX ProjectionMatrix(float FoV = 45.0f) const;

        private:
            void CreateSwapchain(GPUDevice& device, Window& window);
            void ResizeSwapchain(GPUDevice& device, GraphicsCommandList& command_list, uint32 width, uint32 height);
            void CreateRenderTargetViews(GPUDevice& device);
            void CreateDepthStencilView(GPUDevice& device, GraphicsCommandList&  command_list);
            void SetViewport(GraphicsCommandList& command_list, uint64 width, uint64 height);
            void SetScisorRect(GraphicsCommandList& command_list, uint64 width, uint64 height);
            void SetRenderTarget(GraphicsCommandList& command_list);
        };

    }
}
