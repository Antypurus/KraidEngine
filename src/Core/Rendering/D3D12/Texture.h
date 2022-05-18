#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/DataStructures/StringView.h>

namespace Kraid
{

namespace D3D12
{

    enum class TextureAddressingMode
    {
        Wrap = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        Mirror = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
        Border = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        Clamp = D3D12_TEXTURE_ADDRESS_MODE_CLAMP
    };

    enum class TextureSamplingMode
    {
        Point = D3D12_FILTER_MIN_MAG_MIP_POINT,
        Linear = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        Anisotropic = D3D12_FILTER_ANISOTROPIC
    };

    class TextureSampler
    {
    public:
        TextureSampler() = default;
    };

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
