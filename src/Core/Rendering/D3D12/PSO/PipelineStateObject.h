#pragma once

#include "Rasterizer.h"
#include "../Shader.h"
#include <Core/Rendering/D3D12/VertexBuffer.h>

namespace Kraid
{

namespace D3D12
{

    class GraphicsPipelineStateObject
    {
    public:
        Rasterizer rasterizer;
        DepthStentilStage depth_stencil_stage;
        
        VertexShader* vertex_shader = nullptr;
        PixelShader* pixel_shader = nullptr;
        DomainShader* domain_shader = nullptr;
        HullShader* hull_shader = nullptr;
        GeometryShader* geometry_shader = nullptr;

        PrimitiveTopology topology_type = PrimitiveTopology::Undefined;
    public:
        GraphicsPipelineStateObject() = default;
    };

}

}
