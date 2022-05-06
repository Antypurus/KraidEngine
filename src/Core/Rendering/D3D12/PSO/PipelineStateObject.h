#pragma once

#include "Rasterizer.h"
#include "Blend.h"
#include "../Shader.h"
#include <Core/Rendering/D3D12/VertexBuffer.h>

namespace Kraid
{

namespace D3D12
{

    class GraphicsPipelineStateObject
    {
    public:
        //state information
        Blend blending;
        Rasterizer rasterizer;
        DepthStentilStage depth_stencil_stage;
        PrimitiveTopology topology_type = PrimitiveTopology::Undefined;
        D3D12_INPUT_LAYOUT_DESC vertex_layout = {};
       
        //shaders
        VertexShader* vertex_shader = nullptr;
        PixelShader* pixel_shader = nullptr;
        DomainShader* domain_shader = nullptr;
        HullShader* hull_shader = nullptr;
        GeometryShader* geometry_shader = nullptr;

    public:
        GraphicsPipelineStateObject() = default;
    };

}

}
