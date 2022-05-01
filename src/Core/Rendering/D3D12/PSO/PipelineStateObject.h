#pragma once

#include "Rasterizer.h"

namespace Kraid
{

namespace D3D12
{

    class GraphicsPipelineStateObject
    {
    public:
        Rasterizer rasterizer;
        DepthStentilStage depth_stencil_stage;

    public:
        GraphicsPipelineStateObject() = default;
    };

}

}
