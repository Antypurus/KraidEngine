#include "IndexBuffer.h"

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <d3d12.h>

namespace Kraid
{

    namespace D3D12
    {

        IndexBuffer::IndexBuffer(GPUDevice& device, const std::vector<uint32>& indices, GraphicsCommandList& command_list):
            index_count(indices.size())
        {
            this->upload_buffer = UploadBufferResource(device, indices.size() * sizeof(uint32));
            this->upload_buffer->SetName(L"Index Upload Buffer");
            this->index_buffer = BufferResource(device, indices.size() * sizeof(uint32));
            this->index_buffer->SetName(L"Index Buffer");

            this->upload_buffer.SetBufferData(indices);
            this->upload_buffer.CopyTo(this->index_buffer, command_list);

            this->index_buffer.TransitionStateTo(ResourceState::GenericRead, command_list);

            //NOTE(Tiago):the upload buffer needs to be kept alive until the upload is complete
        }

        void IndexBuffer::Bind(GraphicsCommandList &command_list)
        {
            D3D12_INDEX_BUFFER_VIEW index_buffer_descriptioon = this->GetIndexBufferDescription();
            command_list->IASetIndexBuffer(&index_buffer_descriptioon);
        }

        D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetIndexBufferDescription() const
        {
            D3D12_INDEX_BUFFER_VIEW ret = {};
            ret.BufferLocation = this->index_buffer.resource->GetGPUVirtualAddress();
            ret.Format = DXGI_FORMAT_R32_UINT;
            ret.SizeInBytes = this->index_buffer.width;

            return ret;
        }

    }

}
