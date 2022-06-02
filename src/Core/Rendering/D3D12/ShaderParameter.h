#pragma once

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/Resource/ConstantBufferView.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>

namespace Kraid
{

namespace D3D12
{

    template<typename T>
    class ShaderParameter
    {
    public:
        //BufferResource parameter_buffer;
        UploadBufferResource upload_buffer;
        ConstantBufferView buffer_cbv;
        T data_copy;
        
        ShaderParameter() = default;
        ShaderParameter(GPUDevice& device, GraphicsCommandList& command_list, T value = T())
        {
            this->data_copy = value;
            uint64 size_t = sizeof(T) + (256 - (sizeof(T) % 256));
            this->upload_buffer = UploadBufferResource(device, size_t);
            //this->parameter_buffer = BufferResource(device, size_t);

            this->UpdateData(data_copy, command_list);

            uint64 heap_index = device.shader_resource_heap.AllocateIndex();
            this->buffer_cbv = ConstantBufferView(device, this->upload_buffer, device.shader_resource_heap, heap_index);
        }

        inline void UpdateData(T value, GraphicsCommandList& command_list)
        {
            this->data_copy = value;
            this->upload_buffer.SetBufferData(&data_copy);
            //this->upload_buffer.CopyTo(this->parameter_buffer, command_list);
        }

        inline void Bind(GraphicsCommandList& command_list, uint64 slot)
        {
            this->buffer_cbv.Bind(command_list, slot);
        }
    };

}

}
