#pragma once

#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>

#include <DearImGui/imgui.h>

namespace Kraid
{

    struct Window;
    
    namespace D3D12
    {
        struct GPUDevice;
        struct GraphicsCommandList;
    }

    using namespace D3D12;

    class ImGUI
    {
    public:
        CBV_SRV_UAVDescriptorHeap gui_descriptor_heap;

        ImGUI() = default;
        ImGUI(GPUDevice& device, Window& window);
        void StartFrame();
        void EndFrame(GraphicsCommandList& command_list);
    };

}
