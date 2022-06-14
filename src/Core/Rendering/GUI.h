#pragma once

namespace Kraid
{

    struct Window;
    
    namespace D3D12
    {
        struct GPUDevice;
    }

    using namespace D3D12;

    class ImGUI
    {
    public:
        ImGUI() = default;
        ImGUI(GPUDevice& device, Window& window);
    };

}
