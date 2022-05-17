#include "Texture.h"

#include <dxgiformat.h>
#include <cmath>

#include <Core/Utils/Log.h>

#define STB_IMAGE_IMPLEMENTATION
//NOTE(Tiago):have to be here because of clang. stb_image does not properly detect clang on windows
#define STBI_NO_SIMD
#define STBI__X64_TARGET
//NOTE(Tiago):end of note
#include <stb/stb_image.h>

namespace Kraid
{

namespace D3D12
{

    Texture::Texture(const StringView& filepath, GPUDevice& device, GraphicsCommandList& command_list)
    {
        uint8* texture_data = stbi_load(filepath.Get(), (int*)&this->width, (int*)&this->height, (int*)&this->channel_count, 0);
        if(texture_data == nullptr)
        {
            LERROR("Failed to load texture data");
            return; 
        }

        DXGI_FORMAT format;
        switch(this->channel_count)
        {
            case(1):
            {
                format = DXGI_FORMAT_R8_UNORM;
                break;
            }
            case(2):
            {
                format = DXGI_FORMAT_R8G8_UNORM;
                break;
            }
            case(3):
            {
                //TODO(Tiago):bugs out, need to expand to a 32bpp buffer
                //NOTE(Tiago):24bpp formats have been removed from DX, so we assume a 32bit format
                format = DXGI_FORMAT_B8G8R8X8_UNORM;
                break;
            }
            case(4):
            {
                format = DXGI_FORMAT_B8G8R8A8_UNORM;
                break;
            }
        }
        //TODO(Tiago):needs to clean up this calculation and figure out why its like this again
        uint64 buffer_size = ((((this->width * this->channel_count) + 255) & ~255) * (this->height - 1)) + (this->width * this->channel_count);
        this->upload_buffer = UploadBufferResource(device, buffer_size);
        this->upload_buffer.SetTextureData(texture_data, this->width, this->height, this->channel_count);

        this->texture = Texture2DResource(device, this->width, this->height, format);
        
        D3D12_TEXTURE_COPY_LOCATION source = {};
        source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        source.pResource = this->upload_buffer.resource.Get();
        source.PlacedFootprint = {};
        source.PlacedFootprint.Footprint.Width = this->width;
        source.PlacedFootprint.Footprint.Height = this->height;
        source.PlacedFootprint.Footprint.Depth = 1;
        source.PlacedFootprint.Footprint.Format = format;
        source.PlacedFootprint.Footprint.RowPitch = std::lround(std::ceil(this->width * this->channel_count / (double)256)) * 256;//TODO(Tiago):needs to clean up this calculation and redo the math in my head

        D3D12_TEXTURE_COPY_LOCATION destination = {};
        destination.pResource = this->texture.resource.Get();
        destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        destination.SubresourceIndex = 0;

        command_list->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
    }

}

}
