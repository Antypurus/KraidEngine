#pragma once

#include "Core/Rendering/D3D12/CommandList.h"
#include <vector>
#include <Core/Rendering/D3D12/D3D12.h>

#include <Core/types.h>

namespace Kraid
{
    
    namespace D3D12
    {

        struct GPUDevice;
        struct CommandList;

        using namespace Microsoft::WRL;

        enum class ResourceFlags
        {
            None                        = D3D12_RESOURCE_FLAG_NONE,
            AllowRenderTarget           = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
            AllowDepthStencil           = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
            AllowUAV                    = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            DenySRV                     = D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
            AllowCrossAdapter           = D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER,
            AllowSimultaneousAccess     = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS,
            VideoDecodeReferenceOnly    = D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY,
            VideoEncodeReferenceOnly    = D3D12_RESOURCE_FLAG_VIDEO_ENCODE_REFERENCE_ONLY
        };

        enum class ResourceHeapType
        {
            Default     = D3D12_HEAP_TYPE_DEFAULT,
            Upload      = D3D12_HEAP_TYPE_UPLOAD,
            Readback    = D3D12_HEAP_TYPE_READBACK
            //TODO(Tiago): there are custom heaps but we are not implementing them right now
        };

        enum class ResourceState
        {
            Common                              = D3D12_RESOURCE_STATE_COMMON,
            VertexBuffer                        = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            ConstantBuffer                      = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            IndexBuffer                         = D3D12_RESOURCE_STATE_INDEX_BUFFER,
            RenderTarget                        = D3D12_RESOURCE_STATE_RENDER_TARGET,
            UnorderedAccess                     = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            DepthWrite                          = D3D12_RESOURCE_STATE_DEPTH_WRITE,
            DepthRead                           = D3D12_RESOURCE_STATE_DEPTH_READ,
            NonPixelShaderResource              = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            PixelShaderResource                 = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            StreamOutput                        = D3D12_RESOURCE_STATE_STREAM_OUT,
            IndirectArgument                    = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
            CopyDestination                     = D3D12_RESOURCE_STATE_COPY_DEST,
            CopySource                          = D3D12_RESOURCE_STATE_COPY_SOURCE,
            ResolveDestination                  = D3D12_RESOURCE_STATE_RESOLVE_DEST,
            ResolveSource                       = D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
            RaytracingAccelerationStructure     = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
            ShadingRateSource                   = D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE,
            GenericRead                         = D3D12_RESOURCE_STATE_GENERIC_READ,
            AllShaderResource                   = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
            Present                             = D3D12_RESOURCE_STATE_PRESENT,
            Predication                         = D3D12_RESOURCE_STATE_PREDICATION,
            VideoDecodeRead                     = D3D12_RESOURCE_STATE_VIDEO_DECODE_READ,
            VideoDecodeWrite                    = D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE,
            VideoProcessRead                    = D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ,
            VideoProcessWrite                   = D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE,
            VideoEncodeRead                     = D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ,
            VideoEncodeWrite                    = D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE
        };

        enum class ResourceLayout
        {
            Unknown             = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            RowMajor            = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            StandardSwizzle     = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE,
            UndefinedSwizzle    = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE
        };

        enum class ResourceHeapMemoryPool
        {
            Unknown     = D3D12_MEMORY_POOL_UNKNOWN,
            CPUMemory   = D3D12_MEMORY_POOL_L0, //NOTE(Tiago): CPU has greater bandwidth than GPU
            GPUMemory   = D3D12_MEMORY_POOL_L1  //NOTE(Tiago): GPU has the greater bandwidth and the CPU cant access it directly
        };

        enum class ResourceHeapCPUPagePropterty
        {
            Unknown         = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            NotAvailable    = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE,
            WriteCombine    = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE,
            WriteBack       = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK
        };

        enum class ResourceHeapFlags
        {
            None                            = D3D12_HEAP_FLAG_NONE,
            Shared                          = D3D12_HEAP_FLAG_SHARED,
            DenyBuffers                     = D3D12_HEAP_FLAG_DENY_BUFFERS,
            AllowDisplay                    = D3D12_HEAP_FLAG_ALLOW_DISPLAY,
            SharedCrossAdapter              = D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER,
            DenyRTAndDSTextures             = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES,
            OnlyAllowRTAndDSTextures        = D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES,
            HardwareProtected               = D3D12_HEAP_FLAG_HARDWARE_PROTECTED,
            AllowWriteWatch                 = D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH,
            AllowShaderAtomics              = D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS,
            CreateNotResident               = D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT,
            CreateNotZeroed                 = D3D12_HEAP_FLAG_CREATE_NOT_ZEROED,
            AllowAllBufferAndTextures       = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
            OnlyAllowBuffers                = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
        };

        //TODO(Tiago): I need to look into the memory packing of these structs, while i did try to consider it, im not sure
        //of the exact size of many of the member and probably there is a lot i can improve here
        struct ResourceHeap
        {
            ResourceHeapType                heap_type           = ResourceHeapType::Default;
            ResourceHeapMemoryPool          memory_pool         = ResourceHeapMemoryPool::Unknown;
            ResourceHeapCPUPagePropterty    cpu_page_property   = ResourceHeapCPUPagePropterty::Unknown;
            ResourceHeapFlags               flags               = ResourceHeapFlags::None;

            //TODO(Tiago): should i just move this to the cpp?
            constexpr ResourceHeap(ResourceHeapType heap_type                   = ResourceHeapType::Default,
                                ResourceHeapMemoryPool memory_pool              = ResourceHeapMemoryPool::Unknown,
                                ResourceHeapCPUPagePropterty cpu_page_property  = ResourceHeapCPUPagePropterty::Unknown,
                                ResourceHeapFlags flags                         = ResourceHeapFlags::None):
                heap_type(heap_type),
                memory_pool(memory_pool),
                cpu_page_property(cpu_page_property),
                flags(flags)
            {};
            ResourceHeap(const ResourceHeap& other_heap);
            ResourceHeap& operator=(const ResourceHeap& other_heap);
        };

        struct Resource
        {
            ComPtr<ID3D12Resource2>     resource    = nullptr;
            uint32                      width       = 0;//NOTE(Tiago):all types of resources need a width/size in bytes.
            DXGI_FORMAT                 format      = DXGI_FORMAT_UNKNOWN;//NOTE(Tiago):all resources must have a format
            ResourceFlags               flags       = ResourceFlags::None;//NOTE(Tiago):all resources have a set of flags
            ResourceState               state       = ResourceState::Common;//NOTE(Tiago):all resources have a current state
            ResourceLayout              layout      = ResourceLayout::Unknown;//NOTE(Tiago):all resources have a layout, by default the driver should decide it
            ResourceHeap                heap        = {};//NOTE(Tiago):all resources are in a heap

        public:
            Resource() = default;
            ID3D12Resource2* operator->();
            void TransitionStateTo(ResourceState state, CommandList& command_list);
        };

        struct BufferResource :public Resource
        {
            BufferResource() = default;
            BufferResource(GPUDevice& device,
                    uint32 size,
                    ResourceState initial_state     = ResourceState::Common,
                    ResourceFlags flags             = ResourceFlags::None,
                    ResourceHeap heap               = {});

            //TODO(Tiago):we might want to create a specialized copy for each resource and in each resource.
            void CopyTo(Resource& destination_resource, GraphicsCommandList& command_list);
        };

        struct UploadBufferResource: public BufferResource
        {
            UploadBufferResource() = default;
            UploadBufferResource(GPUDevice& defice,
                    uint32 size,
                    ResourceState initial_state     = ResourceState::GenericRead,
                    ResourceFlags flags             = ResourceFlags::None);
            
            //TODO(Tiago): at some point we might want to allow for the addressing range to be specified
            void SetBufferData(const uint8* data, uint64 size);
            template<typename T> inline void SetBufferData(const std::vector<T>& data)
            {
                this->SetBufferData((uint8*)data.data(), data.size() * sizeof(T));
            }
        };

        struct Texture1DResource :public Resource
        {
            Texture1DResource() = default;
            Texture1DResource(GPUDevice& device,
                    uint32 width,
                    DXGI_FORMAT format,
                    ResourceState initial_state     = ResourceState::Common,
                    ResourceLayout layout           = ResourceLayout::Unknown,
                    ResourceFlags flags             = ResourceFlags::None,
                    ResourceHeap heap               = {});
        };

        struct Texture2DResource :public Resource
        {
            uint32 height;//2D texture need a height on top of width

            Texture2DResource() = default;
            Texture2DResource(GPUDevice& device,
                    uint32 width,
                    uint32 height,
                    DXGI_FORMAT format,
                    ResourceState initial_state     = ResourceState::Common,
                    ResourceLayout layout           = ResourceLayout::Unknown,
                    ResourceFlags flags             = ResourceFlags::None,
                    ResourceHeap heap               = {});
            //Only used for render target textures which require a float[4] optimized clear value
            Texture2DResource(GPUDevice& device,
                    uint32 width,
                    uint32 height,
                    DXGI_FORMAT format,
                    const float rtv_clear_value[4],
                    ResourceState initial_state     = ResourceState::Common,
                    ResourceLayout layout           = ResourceLayout::Unknown,
                    ResourceFlags flags             = ResourceFlags::AllowRenderTarget,
                    ResourceHeap heap               = {});
            //Only used for depth stencil texture which require two floats for the depth and stencil optmized clear values
            Texture2DResource(GPUDevice& device,
                    uint32 width,
                    uint32 height,
                    DXGI_FORMAT format,
                    float depth_clear_value,
                    float stencil_clear_value,
                    ResourceState initial_state     = ResourceState::Common,
                    ResourceLayout layout           = ResourceLayout::Unknown,
                    ResourceFlags flags             = ResourceFlags::AllowDepthStencil,
                    ResourceHeap heap               = {});

        };

        struct Texture3DResource :public Resource
        {
            //3D texture need a height and depth(z-axis) on top of the width
            uint32 height;
            uint32 depth;

            Texture3DResource() = default;
            Texture3DResource(GPUDevice& device,
                    uint32 width,
                    uint32 height,
                    uint32 depth,
                    DXGI_FORMAT format,
                    ResourceState initial_state     = ResourceState::Common,
                    ResourceLayout layout           = ResourceLayout::Unknown,
                    ResourceFlags flags             = ResourceFlags::None,
                    ResourceHeap heap               = {});
        };

    }

}
