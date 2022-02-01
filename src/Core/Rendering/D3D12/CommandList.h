#pragma once

#include <Core/Rendering/D3D12/D3D12.h>

namespace Kraid
{

    namespace D3D12
    {

        struct GPUDevice;

        using namespace Microsoft::WRL;

        struct CommandList
        {
            ComPtr<ID3D12GraphicsCommandList6> command_list = nullptr;

            CommandList() = default;
            ID3D12GraphicsCommandList6* operator->();
        };

        struct GraphicsCommandList :public CommandList
        {
           GraphicsCommandList() = default;
           GraphicsCommandList(GPUDevice& device);
        };

        struct CopyCommandList :public CommandList
        {
            CopyCommandList() = default;
            CopyCommandList(GPUDevice& device);
        };

        struct ComputeCommandList :public CommandList
        {
            ComputeCommandList() = default;
            ComputeCommandList(GPUDevice& device);
        };

        struct VideoDecodeCommandList
        {
            ComPtr<ID3D12VideoDecodeCommandList2> command_list = nullptr;

            VideoDecodeCommandList() = default;
            VideoDecodeCommandList(GPUDevice& device);

            ID3D12VideoDecodeCommandList2* operator->();
        };

        struct VideoProcessCommandList
        {
            ComPtr<ID3D12VideoProcessCommandList2> command_list = nullptr;

            VideoProcessCommandList() = default;
            VideoProcessCommandList(GPUDevice& device);

            ID3D12VideoProcessCommandList2* operator->();
        };

    }

}
