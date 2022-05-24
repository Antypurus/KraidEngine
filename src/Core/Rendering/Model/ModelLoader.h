#pragma once

#include <Core/DataStructures/StringView.h>
#include <Core/types.h>
#include <Core/Rendering/Model/Model.h>

namespace Kraid
{

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
