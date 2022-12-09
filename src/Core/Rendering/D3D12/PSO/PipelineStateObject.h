#pragma once

#include "Rasterizer.h"
#include "Blend.h"
#include "StreamOutput.h"
#include "../Shader.h"

#include <Core/Rendering/D3D12/VertexBuffer.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/RootSignature.h>
#include <Core/Threading/Lock.h>

namespace Kraid
{

namespace D3D12
{

    using namespace Microsoft::WRL;

    class GraphicsPipelineStateObject
    {
    public:
        //state information
        Blend blending;
        Rasterizer rasterizer;
        DepthStentilStage depth_stencil_stage;
        StreamingOutputBuffer streaming_output_buffer;
        PrimitiveTopology topology_type = PrimitiveTopology::Undefined;
        D3D12_INPUT_LAYOUT_DESC vertex_layout = {};

        //shaders
        VertexShader* vertex_shader = nullptr;
        PixelShader* pixel_shader = nullptr;
        DomainShader* domain_shader = nullptr;
        HullShader* hull_shader = nullptr;
        GeometryShader* geometry_shader = nullptr;

        //root signature
        RootSignature root_signature;

        //PSO Object
        ComPtr<ID3D12PipelineState> pso[2] = {nullptr, nullptr};

        //data stored for hot recompilation
        Mutex compilation_mutex;
        uint8 pso_index = 0;
        bool swap_pso_index = false;
    public:
        GraphicsPipelineStateObject() = default;
        GraphicsPipelineStateObject(
                VertexShader& vertex_shader,
                PixelShader& pixel_shader,
                const RootSignature& root_signature,
                PrimitiveTopology topology_type,
                D3D12_INPUT_LAYOUT_DESC vertex_layout,
                Rasterizer rasterizer = {},
                DepthStentilStage depth_stencil_stage = {},
                Blend blending = {},
                StreamingOutputBuffer so_buffer = {});
        ~GraphicsPipelineStateObject();
        void Compile();
        void Bind(GraphicsCommandList& command_list);
    };

}

}
