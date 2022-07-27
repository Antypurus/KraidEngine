#pragma once

#include <Core/DataStructures/StringView.h>
#include <Core/Rendering/Model/Model.h>
#include <Core/types.h>
#include <Core/Rendering/D3D12/GPUDevice.h>

namespace Kraid
{

    namespace D3D12
    {
        struct GPUDevice;
        struct GraphicsCommandList;
    }

    class ModelLoader
    {
    public:
        static Model LoadOBJModel(
            const StringView& filepath,
            GraphicsCommandList& command_list,
            GPUDevice& device = GPUDevice::Instance()
        );
    };

}
