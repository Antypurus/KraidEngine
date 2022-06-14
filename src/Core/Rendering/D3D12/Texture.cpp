#include "Texture.h"

#include <dxgiformat.h>
#include <cmath>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
//NOTE(Tiago):have to be here because of clang. stb_image does not properly detect clang on windows
#define STBI_NO_SIMD
#define STBI__X64_TARGET
//NOTE(Tiago):end of note
#include <stb/stb_image.h>

#include <Core/Utils/Log.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>

namespace Kraid
{

namespace D3D12
{

    TextureSampler::TextureSampler(
            GPUDevice& device,
            SamplerDescriptorHeap& heap,
            TextureSamplingMode sample_mode,
            TextureAddressingMode addressing_mode):
        addressing_mode(addressing_mode),
        sampling_mode(sample_mode)
    {
        uint64 heap_index = heap.AllocateIndex();
        this->cpu_descriptor_handle = heap[heap_index];
        this->gpu_descriptor_handle = heap.GetGPUDescriptorByIndex(heap_index);

        D3D12_SAMPLER_DESC sampler_description = this->CreateSamplerDescription();
        device->CreateSampler(&sampler_description, this->cpu_descriptor_handle);
    }

    TextureSampler::TextureSampler(
            GPUDevice& device,
            SamplerDescriptorHeap& heap,
            float border_color[4],
            TextureSamplingMode sample_mode):
        addressing_mode(TextureAddressingMode::Border),
        sampling_mode(sample_mode)
    {
        uint64 heap_index = heap.AllocateIndex();
        this->cpu_descriptor_handle = heap[heap_index];
        this->gpu_descriptor_handle = heap.GetGPUDescriptorByIndex(heap_index);

        memcpy(this->border_color, border_color, sizeof(float) * 4);

        D3D12_SAMPLER_DESC sampler_description = this->CreateSamplerDescription();
        device->CreateSampler(&sampler_description, this->cpu_descriptor_handle);
    }

    void TextureSampler::Bind(GraphicsCommandList& command_list, uint64 slot)
    {
        command_list->SetGraphicsRootDescriptorTable(slot, this->gpu_descriptor_handle);
    }

    D3D12_SAMPLER_DESC TextureSampler::CreateSamplerDescription() const
    {
        D3D12_SAMPLER_DESC ret = {};
        ret.AddressU = (D3D12_TEXTURE_ADDRESS_MODE)this->addressing_mode;
        ret.AddressV = (D3D12_TEXTURE_ADDRESS_MODE)this->addressing_mode;
        ret.AddressW = (D3D12_TEXTURE_ADDRESS_MODE)this->addressing_mode;
        if(this->addressing_mode == TextureAddressingMode::Border)
        {
            memcpy(ret.BorderColor, this->border_color, sizeof(float) * 4);
        }
        ret.Filter = (D3D12_FILTER)this->sampling_mode;
        ret.MaxAnisotropy = 16;
        ret.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        ret.MipLODBias = 0;
        ret.MinLOD = 0;
        ret.MaxLOD = 0;//TODO(Tiago): have way to set this

        return ret;
    }

    Texture::Texture(const StringView& filepath, GPUDevice& device, GraphicsCommandList& command_list)
    {
        uint8* texture_data = stbi_load(filepath.Get(), (int*)&this->width, (int*)&this->height, (int*)&this->channel_count, 0);
        if(texture_data == nullptr)
        {
            LERROR("Failed to load texture data");
            return; 
        }

        //TODO(Tiago):all this needs to be cleaned up
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
                //NOTE(Tiago):24bpp formats have been removed from DX, so we assume a 32bit format
                uint8* expanded_texture_data = (uint8*)malloc(this->width * this->height * (this->channel_count + 1));
                for(uint64 y = 0; y < this->height; y+=1)
                {
                    for(uint64 x = 0; x < this->width; x+=1)
                    {
                        uint64 offset = y * (this->width * this->channel_count) + x * this->channel_count;
                        uint64 expanded_offset = y * (this->width * (this->channel_count + 1)) + x * (this->channel_count + 1);
                        expanded_texture_data[expanded_offset] = texture_data[offset];
                        expanded_texture_data[expanded_offset + 1] = texture_data[offset + 1];
                        expanded_texture_data[expanded_offset + 2] = texture_data[offset + 2];
                        expanded_texture_data[expanded_offset + 3] = 255;
                    }
                }
                stbi_image_free(texture_data);
                texture_data = expanded_texture_data;
                this->channel_count += 1;
                format = DXGI_FORMAT_R8G8B8A8_UNORM;
                break;
            }
            case(4):
            {
                format = DXGI_FORMAT_R8G8B8A8_UNORM;
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

        stbi_image_free(texture_data);

        this->CreateDefaultSRV(device);
    }

    void Texture::BindDefaultSRV(GraphicsCommandList& command_list, uint32 texture_bind_slot)
    {
        this->default_srv.Bind(command_list, texture_bind_slot);
    }

    void Texture::CreateDefaultSRV(GPUDevice& device)
    {
        uint64 heap_index = device.shader_resource_heap.AllocateIndex();
        this->default_srv = ShaderResourceView(device, device.shader_resource_heap, heap_index, this->texture, this->texture.format);
    }

    static std::unordered_map<std::string, Texture> loaded_textures;
    Texture& Texture::LoadTexture(const StringView& filepath, GPUDevice& device, GraphicsCommandList& command_list)
    {
        if(loaded_textures.contains(filepath.Get()))
        {
            return loaded_textures[filepath.Get()];
        }

        loaded_textures.emplace(filepath.Get(), Texture(filepath, device, command_list));
        return loaded_textures[filepath.Get()];
    }

}

}
