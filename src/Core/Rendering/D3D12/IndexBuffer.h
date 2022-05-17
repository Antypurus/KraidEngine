#pragma once

#include "Core/Rendering/D3D12/CommandList.h"
#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>

#include <d3d12.h>
#include <vector>

namespace Kraid
{

    namespace D3D12
    {

        struct GPUDevice;

        struct IndexBuffer
        {
            UploadBufferResource upload_buffer;
            BufferResource index_buffer;
            uint64 index_count = 0;

            IndexBuffer() = default;
            IndexBuffer(GPUDevice& device, const std::vector<uint32>& indices, GraphicsCommandList& command_list);

            void Bind(GraphicsCommandList& command_list);
            D3D12_INDEX_BUFFER_VIEW GetIndexBufferDescription() const;
        };

    }

}
