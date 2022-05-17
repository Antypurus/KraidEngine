#pragma once

#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/DataStructures/StringView.h>

namespace Kraid
{

namespace D3D12
{

    class Texture
    {
    public:
        Texture2DResource texture;
        UploadBufferResource upload_buffer;
        uint64 width = 0;
        uint64 height = 0;
        uint8 channel_count = 0;

    public:
        Texture() = default;
        Texture(const StringView& filepath, GPUDevice& device, GraphicsCommandList& command_list); 
    };

}

}
