/** Resource System TODO
 *
 * [ ] For Texture1D, Width must be less than or equal to D3D10_REQ_TEXTURE1D_U_DIMENSION on feature levels less than 11_0 and D3D11_REQ_TEXTURE1D_U_DIMENSION on feature level 11_0 or greater.
 * [ ] For Texture1D, DepthOrArraySize is interpreted as array size and must be less than or equal to D3D10_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION on feature levels less than 11_0 and D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION on feature levels 11_0 or greater.
 *
 **/


#include "Resource.h"

#include <Core/Windows.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Utils/Log.h>

namespace Kraid
{

    namespace D3D12
    {

        ResourceHeap::ResourceHeap(const ResourceHeap& other_heap):
            heap_type(other_heap.heap_type),
            memory_pool(other_heap.memory_pool),
            cpu_page_property(other_heap.cpu_page_property),
            flags(other_heap.flags)
        {
        }

        ResourceHeap& ResourceHeap::operator=(const ResourceHeap& other_heap)
        {
            this->heap_type = other_heap.heap_type;
            this->memory_pool = other_heap.memory_pool;
            this->cpu_page_property = other_heap.cpu_page_property;
            this->flags = other_heap.flags;
            return *this;
        }

        ID3D12Resource2* Resource::operator->()
        {
            return this->resource.Get();
        }

        void Resource::TransitionStateTo(ResourceState state, CommandList &command_list)
        {
            if(state == this->state)
            {
                LWARNING("Attempt to transition resource to state it was already in, aborting...");
                return;
            }

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)this->state;
            barrier.Transition.StateAfter = (D3D12_RESOURCE_STATES)state;
            barrier.Transition.pResource = this->resource.Get();

            command_list->ResourceBarrier(1, &barrier);

            this->state = state;
        }

        BufferResource::BufferResource(
                GPUDevice& device,
                uint32 size,
                ResourceState initial_state,
                ResourceFlags flags,
                ResourceHeap heap)
        {
            this->width = size;
            this->flags = flags;
            this->format = DXGI_FORMAT_UNKNOWN;//NOTE(Tiago):buffers can only have this format
            this->state = initial_state;
            this->heap = heap;
            this->layout = ResourceLayout::RowMajor;

            D3D12_RESOURCE_DESC resource_desc = {};
            resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resource_desc.Width = this->width;
            resource_desc.Height = 1;
            resource_desc.DepthOrArraySize = 1;
            resource_desc.MipLevels = 1;
            //NOTE(Tiago): buffers cant be MSAA'd
            resource_desc.SampleDesc.Count = 1;
            resource_desc.SampleDesc.Quality = 0;
            resource_desc.Format = this->format;
            resource_desc.Layout = (D3D12_TEXTURE_LAYOUT)this->layout;
            resource_desc.Flags = (D3D12_RESOURCE_FLAGS)this->flags;

            D3D12_HEAP_PROPERTIES heap_properties = {};
            heap_properties.Type = (D3D12_HEAP_TYPE)this->heap.heap_type;
            heap_properties.CPUPageProperty = (D3D12_CPU_PAGE_PROPERTY)this->heap.cpu_page_property;
            heap_properties.MemoryPoolPreference = (D3D12_MEMORY_POOL)this->heap.memory_pool;
            heap_properties.CreationNodeMask = 0;
            heap_properties.VisibleNodeMask = 0;

            D3DCALL(device->CreateCommittedResource(
                    &heap_properties,
                    (D3D12_HEAP_FLAGS)this->heap.flags,
                    &resource_desc,
                    (D3D12_RESOURCE_STATES)this->state,
                    nullptr,
                    IID_PPV_ARGS(&this->resource)),
                "Buffer Resource Created");
        }

        void BufferResource::CopyTo(Resource& destination_resource, GraphicsCommandList& command_list)
        {
            //TODO(Tiago):cant we copy to an upload buffer at all? or can that only be done to a readback buffer? I honestly dont remember from last time
            //TODO(Tiago):add some generic way to do these types of asserts
            if(this->width > destination_resource.width)
            {
                LERROR("destination resource cannot hold data inside of source buffer, aborting copy");
                return;
            }

            ResourceState initial_destination_resource_state = destination_resource.state;
            if(initial_destination_resource_state != ResourceState::CopyDestination)
            {
                destination_resource.TransitionStateTo(ResourceState::CopyDestination, command_list);
            }

            //TODO(Tiago):if the resource we are copying from is an upload buffer we cant change its state
            ResourceState initial_source_destination_state = this->state;
            if(initial_source_destination_state != ResourceState::CopySource && this->heap.heap_type != ResourceHeapType::Upload)
            {
                this->TransitionStateTo(ResourceState::CopySource, command_list);
            }

            //TODO(Tiago):we migth want to add a way to specify an offset
            command_list->CopyBufferRegion(destination_resource.resource.Get(), 0, this->resource.Get(), 0, this->width);

            if(initial_destination_resource_state != ResourceState::CopyDestination)
            {
                destination_resource.TransitionStateTo(initial_destination_resource_state, command_list);
            }
            if(initial_source_destination_state != ResourceState::CopySource && this->heap.heap_type != ResourceHeapType::Upload)
            {
                this->TransitionStateTo(initial_source_destination_state, command_list);
            }
            LINFO("Schedule data copy between two buffer resources");
        }

        UploadBufferResource::UploadBufferResource(GPUDevice& device, uint32 size, ResourceState initial_state, ResourceFlags flags)
            :BufferResource(device, size, initial_state, flags, {ResourceHeapType::Upload, ResourceHeapMemoryPool::Unknown, ResourceHeapCPUPagePropterty::Unknown, ResourceHeapFlags::None})
        {
        }

        void UploadBufferResource::SetBufferData(const uint8* data, uint64 size)
        {
            D3D12_RANGE buffer_addressing_range = {};
            buffer_addressing_range.Begin = 0;
            buffer_addressing_range.End = this->width;//NOTE(Tiago): is there any reason why we should not address the entire buffer upfront?

            uint8* buffer_data_addressing = nullptr;
            D3DCALL(this->resource->Map(0, &buffer_addressing_range, (void**)&buffer_data_addressing), "Mapped upload buffer into CPU addressing range");
            if(buffer_data_addressing != nullptr)
            {
                memcpy(buffer_data_addressing, data, size);
                this->resource->Unmap(0, &buffer_addressing_range);
                LSUCCESS("Upload Buffer Data Set");
            }
            else
            {
                LERROR("Failed to map upload buffer into CPU addressing range, setting data must be aborted");
                __debugbreak(); //TODO(Tiago): find a better way to handle this situation
            }
        }

        Texture1DResource::Texture1DResource(
                GPUDevice& device,
                uint32 width,
                DXGI_FORMAT format,
                ResourceState initial_state,
                ResourceLayout layout,
                ResourceFlags flags,
                ResourceHeap heap)
        {
            this->width = width;
            this->flags = flags;
            this->format = format;
            this->layout = layout;
            this->state = initial_state;
            this->heap = heap;

            D3D12_RESOURCE_DESC resource_desc = {};
            resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resource_desc.Width = this->width;
            resource_desc.Height = 1;
            resource_desc.DepthOrArraySize = 1;
            resource_desc.MipLevels = 0;//TODO(Tiago):0 marks driver auto computation of mip count, 1 it does not, this should be a parameter
            //TODO(Tiago): can we have MSAA texture 1D?
            resource_desc.SampleDesc.Count = 1;
            resource_desc.SampleDesc.Quality = 0;
            resource_desc.Format = this->format;
            resource_desc.Layout = (D3D12_TEXTURE_LAYOUT)this->layout;
            resource_desc.Flags = (D3D12_RESOURCE_FLAGS)this->flags;

            D3D12_HEAP_PROPERTIES heap_properties = {};
            heap_properties.Type = (D3D12_HEAP_TYPE)this->heap.heap_type;
            heap_properties.CPUPageProperty = (D3D12_CPU_PAGE_PROPERTY)this->heap.cpu_page_property;
            heap_properties.MemoryPoolPreference = (D3D12_MEMORY_POOL)this->heap.memory_pool;
            heap_properties.CreationNodeMask = 0;
            heap_properties.VisibleNodeMask = 0;

            D3DCALL(device->CreateCommittedResource(
                    &heap_properties,
                    (D3D12_HEAP_FLAGS)this->heap.flags,
                    &resource_desc,
                    (D3D12_RESOURCE_STATES)this->state,
                    nullptr,
                    IID_PPV_ARGS(&this->resource)),
                "Texture1D Resource Created");
        }

        Texture2DResource::Texture2DResource(
                GPUDevice& device,
                uint32 width,
                uint32 height,
                DXGI_FORMAT format,
                ResourceState initial_state,
                ResourceLayout layout,
                ResourceFlags flags,
                ResourceHeap heap)
        {
            this->width = width;
            this->height = height;
            this->flags = flags;
            this->format = format;
            this->layout = layout;
            this->state = initial_state;
            this->heap = heap;

            D3D12_RESOURCE_DESC resource_desc = {};
            resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resource_desc.Width = this->width;
            resource_desc.Height = this->height;
            resource_desc.DepthOrArraySize = 1;
            resource_desc.MipLevels = 0;//TODO(Tiago):0 marks driver auto computation of mip count, 1 it does not, this should be a parameter
            //TODO(Tiago): can we have MSAA texture 1D?
            resource_desc.SampleDesc.Count = 1;
            resource_desc.SampleDesc.Quality = 0;
            resource_desc.Format = this->format;
            resource_desc.Layout = (D3D12_TEXTURE_LAYOUT)this->layout;
            resource_desc.Flags = (D3D12_RESOURCE_FLAGS)this->flags;

            D3D12_HEAP_PROPERTIES heap_properties = {};
            heap_properties.Type = (D3D12_HEAP_TYPE)this->heap.heap_type;
            heap_properties.CPUPageProperty = (D3D12_CPU_PAGE_PROPERTY)this->heap.cpu_page_property;
            heap_properties.MemoryPoolPreference = (D3D12_MEMORY_POOL)this->heap.memory_pool;
            heap_properties.CreationNodeMask = 0;
            heap_properties.VisibleNodeMask = 0;

            D3DCALL(device->CreateCommittedResource(
                    &heap_properties,
                    (D3D12_HEAP_FLAGS)this->heap.flags,
                    &resource_desc,
                    (D3D12_RESOURCE_STATES)this->state,
                    nullptr,
                    IID_PPV_ARGS(&this->resource)),
                "Texture2D Resource Created");
        }
        
        Texture2DResource::Texture2DResource(
                GPUDevice& device,
                uint32 width,
                uint32 height,
                DXGI_FORMAT format,
                const float rtv_clear_value[4],
                ResourceState initial_state,
                ResourceLayout layout,
                ResourceFlags flags,
                ResourceHeap heap)
        {
            this->width = width;
            this->height = height;
            this->flags = flags;
            this->format = format;
            this->layout = layout;
            this->state = initial_state;
            this->heap = heap;

            D3D12_RESOURCE_DESC resource_desc = {};
            resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resource_desc.Width = this->width;
            resource_desc.Height = this->height;
            resource_desc.DepthOrArraySize = 1;
            resource_desc.MipLevels = 0;//TODO(Tiago):0 marks driver auto computation of mip count, 1 it does not, this should be a parameter
            //TODO(Tiago): can we have MSAA texture 1D?
            resource_desc.SampleDesc.Count = 1;
            resource_desc.SampleDesc.Quality = 0;
            resource_desc.Format = this->format;
            resource_desc.Layout = (D3D12_TEXTURE_LAYOUT)this->layout;
            resource_desc.Flags = (D3D12_RESOURCE_FLAGS)this->flags;

            D3D12_HEAP_PROPERTIES heap_properties = {};
            heap_properties.Type = (D3D12_HEAP_TYPE)this->heap.heap_type;
            heap_properties.CPUPageProperty = (D3D12_CPU_PAGE_PROPERTY)this->heap.cpu_page_property;
            heap_properties.MemoryPoolPreference = (D3D12_MEMORY_POOL)this->heap.memory_pool;
            heap_properties.CreationNodeMask = 0;
            heap_properties.VisibleNodeMask = 0;

            D3D12_CLEAR_VALUE rtv_optimized_clear_value = {};
            rtv_optimized_clear_value.Format = this->format;
            memcpy(rtv_optimized_clear_value.Color, rtv_clear_value, 4*sizeof(float));

            D3DCALL(device->CreateCommittedResource(
                    &heap_properties,
                    (D3D12_HEAP_FLAGS)this->heap.flags,
                    &resource_desc,
                    (D3D12_RESOURCE_STATES)this->state,
                    &rtv_optimized_clear_value,
                    IID_PPV_ARGS(&this->resource)),
                "Texture2D Resource Created");
        }
        
        Texture2DResource::Texture2DResource(
                GPUDevice& device,
                uint32 width,
                uint32 height,
                DXGI_FORMAT format,
                float depth_clear_value,
                float stencil_clear_value,
                ResourceState initial_state,
                ResourceLayout layout,
                ResourceFlags flags,
                ResourceHeap heap)
        {
            this->width = width;
            this->height = height;
            this->flags = flags;
            this->format = format;
            this->layout = layout;
            this->state = initial_state;
            this->heap = heap;

            D3D12_RESOURCE_DESC resource_desc = {};
            resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resource_desc.Width = this->width;
            resource_desc.Height = this->height;
            resource_desc.DepthOrArraySize = 1;
            resource_desc.MipLevels = 0;//TODO(Tiago):0 marks driver auto computation of mip count, 1 it does not, this should be a parameter
            //TODO(Tiago): can we have MSAA texture 1D?
            resource_desc.SampleDesc.Count = 1;
            resource_desc.SampleDesc.Quality = 0;
            resource_desc.Format = this->format;
            resource_desc.Layout = (D3D12_TEXTURE_LAYOUT)this->layout;
            resource_desc.Flags = (D3D12_RESOURCE_FLAGS)this->flags;

            D3D12_HEAP_PROPERTIES heap_properties = {};
            heap_properties.Type = (D3D12_HEAP_TYPE)this->heap.heap_type;
            heap_properties.CPUPageProperty = (D3D12_CPU_PAGE_PROPERTY)this->heap.cpu_page_property;
            heap_properties.MemoryPoolPreference = (D3D12_MEMORY_POOL)this->heap.memory_pool;
            heap_properties.CreationNodeMask = 0;
            heap_properties.VisibleNodeMask = 0;

            D3D12_CLEAR_VALUE dsv_optimized_clear_value = {};
            dsv_optimized_clear_value.Format = this->format;
            dsv_optimized_clear_value.DepthStencil.Depth = depth_clear_value;
            dsv_optimized_clear_value.DepthStencil.Stencil = stencil_clear_value;

            D3DCALL(device->CreateCommittedResource(
                    &heap_properties,
                    (D3D12_HEAP_FLAGS)this->heap.flags,
                    &resource_desc,
                    (D3D12_RESOURCE_STATES)this->state,
                    &dsv_optimized_clear_value,
                    IID_PPV_ARGS(&this->resource)),
                "Texture2D Resource Created");
        }

        Texture3DResource::Texture3DResource(
                GPUDevice& device,
                uint32 width,
                uint32 height,
                uint32 depth,
                DXGI_FORMAT format,
                ResourceState initial_state,
                ResourceLayout layout,
                ResourceFlags flags,
                ResourceHeap heap)
        {
            this->width = width;
            this->height = height;
            this->depth = depth;
            this->flags = flags;
            this->format = format;
            this->layout = layout;
            this->state = initial_state;
            this->heap = heap;

            D3D12_RESOURCE_DESC resource_desc = {};
            resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resource_desc.Width = this->width;
            resource_desc.Height = this->height;
            resource_desc.DepthOrArraySize = this->depth;
            resource_desc.MipLevels = 0;//TODO(Tiago):0 marks driver auto computation of mip count, 1 it does not, this should be a parameter
            //TODO(Tiago): can we have MSAA texture 1D?
            resource_desc.SampleDesc.Count = 1;
            resource_desc.SampleDesc.Quality = 0;
            resource_desc.Format = this->format;
            resource_desc.Layout = (D3D12_TEXTURE_LAYOUT)this->layout;
            resource_desc.Flags = (D3D12_RESOURCE_FLAGS)this->flags;

            D3D12_HEAP_PROPERTIES heap_properties = {};
            heap_properties.Type = (D3D12_HEAP_TYPE)this->heap.heap_type;
            heap_properties.CPUPageProperty = (D3D12_CPU_PAGE_PROPERTY)this->heap.cpu_page_property;
            heap_properties.MemoryPoolPreference = (D3D12_MEMORY_POOL)this->heap.memory_pool;
            heap_properties.CreationNodeMask = 0;
            heap_properties.VisibleNodeMask = 0;

            D3DCALL(device->CreateCommittedResource(
                    &heap_properties,
                    (D3D12_HEAP_FLAGS)this->heap.flags,
                    &resource_desc,
                    (D3D12_RESOURCE_STATES)this->state,
                    nullptr,
                    IID_PPV_ARGS(&this->resource)),
                "Texture3D Resource Created");
        }

    }

}
