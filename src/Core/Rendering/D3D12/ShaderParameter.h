#pragma once

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/Resource/ConstantBufferView.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>
#include <Core/Utils/Log.h>

namespace Kraid
{

namespace D3D12
{

    template<typename T>
    class ShaderParameter
    {
    public:
        UploadBufferResource parameter_buffer;
        ConstantBufferView buffer_cbv;
        
        ShaderParameter() = default;
        ShaderParameter(GraphicsCommandList& command_list, T value = T())
        {
            uint64 size_t = sizeof(T) + (256 - (sizeof(T) % 256));
            this->parameter_buffer = UploadBufferResource(GPUDevice::Instance(), size_t);

            this->UpdateData(value, command_list);

            uint64 heap_index = GPUDevice::Instance().shader_resource_heap.AllocateIndex();
            this->buffer_cbv = ConstantBufferView(GPUDevice::Instance(), this->parameter_buffer, GPUDevice::Instance().shader_resource_heap, heap_index);
        }

        inline void UpdateData(T value, GraphicsCommandList& command_list)
        {
            std::vector<T> data_buffer;
            data_buffer.push_back(value);
            this->parameter_buffer.SetBufferData(data_buffer);
        }

        inline void Bind(GraphicsCommandList& command_list, uint64 slot)
        {
            this->buffer_cbv.Bind(command_list, slot);
        }
    };

}

}
