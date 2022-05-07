#pragma once

#include "Rasterizer.h"
#include "Blend.h"
#include "StreamOutput.h"
#include "../Shader.h"

#include <Core/Rendering/D3D12/VertexBuffer.h>
#include <Core/Rendering/D3D12/GPUDevice.h>

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

        //PSO Object
        ComPtr<ID3D12PipelineState> pso = nullptr;
    public:
        GraphicsPipelineStateObject() = default;
        void Compile(GPUDevice& device);
    };

}

}
