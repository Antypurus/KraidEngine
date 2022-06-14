#pragma once

#include <Core/DataStructures/StringView.h>
#include <Core/Rendering/Model/Model.h>
#include <Core/types.h>

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
            GPUDevice& device,
            GraphicsCommandList& command_list,
            const StringView& filepath
        );
    };

}
