#pragma once

#include <Core/Rendering/D3D12/D3D12SDK.h>

#include <Core/types.h>
#include <Core/Rendering/D3D12/CommandQueue.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>

#include "DXGIFactory.h"

namespace Kraid
{

    namespace D3D12
    {

        struct GraphicsCommandList;

        using namespace Microsoft::WRL;

        struct GPUDevice;

        enum class TiledResourceSupportTier
        {
            None = D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED,
            Tier1 = D3D12_TILED_RESOURCES_TIER_1,
            Tier2 = D3D12_TILED_RESOURCES_TIER_2,
            Tier3 = D3D12_TILED_RESOURCES_TIER_3,
            Tier4 = D3D12_TILED_RESOURCES_TIER_4
        };

        enum class ConservativeRasterizationSupportTier
        {
            None = D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED,
            Tier1 = D3D12_CONSERVATIVE_RASTERIZATION_TIER_1,
            Tier2 = D3D12_CONSERVATIVE_RASTERIZATION_TIER_2,
            Tier3 = D3D12_CONSERVATIVE_RASTERIZATION_TIER_3
        };

        enum class ResourceBindingTier
        {
            Tier1 = D3D12_RESOURCE_BINDING_TIER_1,
            Tier2 = D3D12_RESOURCE_BINDING_TIER_2,
            Tier3 = D3D12_RESOURCE_BINDING_TIER_3
        };

        enum class DeviceCrossNodeSharingSupportTier
        {
            NotSupport = D3D12_CROSS_NODE_SHARING_TIER_NOT_SUPPORTED,
            EmulatedTier1 = D3D12_CROSS_NODE_SHARING_TIER_1_EMULATED,
            Tier1 = D3D12_CROSS_NODE_SHARING_TIER_1,
            Tier2 = D3D12_CROSS_NODE_SHARING_TIER_2,
            Tier3 = D3D12_CROSS_NODE_SHARING_TIER_3
        };

        enum class ResourceHeapSupportTier
        {
            Tier1 = D3D12_RESOURCE_HEAP_TIER_1,
            Tier2 = D3D12_RESOURCE_HEAP_TIER_2
        };

        class GPUFeatureSupport
        {
        public:
        // structure 0 data
            bool supports_doubles_in_shaders = false;
            bool supports_blending_logic_operations = false;
            bool supports_32_bit_shader_precision = true;
            bool supports_16_bit_shader_precision = false;
            bool supports_10_bit_shader_precision = false;
            TiledResourceSupportTier tiled_resource_support = TiledResourceSupportTier::None;
            ResourceBindingTier resource_binding_support = ResourceBindingTier::Tier1;
            bool supports_pixel_shader_stencil_reference_values = false;
            bool supports_extended_uav_format_list = false;
            bool supports_rasterizer_order_views = false;
            ConservativeRasterizationSupportTier conservative_rasterization_support = ConservativeRasterizationSupportTier::None;
            bool supports_64KB_texture_swizzle_pattern = false;
            DeviceCrossNodeSharingSupportTier cross_node_sharing_support = DeviceCrossNodeSharingSupportTier::NotSupport;
            bool supports_cross_node_resource_views_on_row_major_textures = false;
            bool VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation = false;//TODO(Tiago):rename this crap once we actually understand what it means
            ResourceHeapSupportTier resource_heap_support =  ResourceHeapSupportTier::Tier1;

        // structure 1 data
            bool supports_hlsl6_wave_operations = false;
            uint32 minimum_simd_lanes_support_for_wave_ops = 0;
            uint32 maximum_simd_lanes_support_for_wave_ops = 0;
            uint32 count_of_simd_lanes_for_wave_ops_on_hardware = 0;
            bool supports_expanded_computer_resourece_states = false; //TODO(Tiago): need to understand what the fuck this actually means, explaination on docs is not easy to understand
            bool supports_64bit_int_ops = false;

        // structure 10 data (go ask ms why this is out of order)


        public:
            GPUFeatureSupport() = default;
            GPUFeatureSupport(GPUDevice& device);

        private:
            void LogGPUFeatureSupport() const;
        };

        struct DescriptorHandleSize
        {
            uint32 rtv = 0;
            uint32 dsv = 0;
            uint32 cbv_srv_uav = 0;
            uint32 sampler = 0;
        };

        struct GPUDevice
        {
        public:
            ComPtr<ID3D12Device8> device = nullptr;
            DXGIFactory factory;

            //device command allocators
            ComPtr<ID3D12CommandAllocator> direct_command_allocator = nullptr;
            ComPtr<ID3D12CommandAllocator> bundle_command_allocator = nullptr;//NOTE(Tiago): frankly i dont even know if i will need this but if i dont i can just remove it later
            ComPtr<ID3D12CommandAllocator> copy_command_allocator = nullptr;
            ComPtr<ID3D12CommandAllocator> computer_comamnd_allocator = nullptr;
            ComPtr<ID3D12CommandAllocator> video_decode_command_allocator = nullptr;
            ComPtr<ID3D12CommandAllocator> video_process_command_allocator = nullptr;
#ifndef NDEBUG
            ComPtr<ID3D12Debug3> debug_layer = nullptr;
#endif
            //device command queues
            CommandQueue direct_command_queue;
            CommandQueue compute_command_queue;
            CommandQueue copy_command_queue;
            CommandQueue video_decode_command_queue;
            CommandQueue video_process_command_queue;

            SamplerDescriptorHeap sampler_descriptior_heap;
            CBV_SRV_UAVDescriptorHeap shader_resource_heap;

            DescriptorHandleSize descriptor_sizes;
            D3D_FEATURE_LEVEL feature_level;
            bool initialized = false;

        private:
            //GPU Device Global Instance
            static GPUDevice* g_gpu_instance;

        public:
            //specify the index of the gpu to be used
            GPUDevice(uint8 gpu_index = 0);
            GPUDevice(IDXGIAdapter4* adapter);
            ~GPUDevice();
            static inline GPUDevice& Instance() { return *g_gpu_instance; };
            static inline GPUDevice& GetInstance() { return *g_gpu_instance; };

            ID3D12Device8* operator->();
            GraphicsCommandList CreateGraphicsCommandList();
            CopyCommandList CreateCopyCommandList();
            ComputeCommandList CreateComputeCommandList();
            VideoProcessCommandList CreateVideoProcessCommandList();
            VideoDecodeCommandList CreateVideoDecodeCommandList();

        private:
            inline void CreateD3D12Device(uint8 gpu_index);
            inline void CreateD3D12DeviceFromAdapater(IDXGIAdapter4* adapter);
            inline void QueryDescriptorSizes();
            inline void CreateCommandAllocator();
            inline void CreateCommandQueues();
    //are where debug only function go
#ifndef NDEBUG
            inline void EnableDebugLayer();
#endif
        };

    }

}
