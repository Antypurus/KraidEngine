#pragma once

#include <d3d12.h>
namespace Kraid
{

namespace D3D12
{

    enum class BlendFactor
    {
        Zero                                = D3D12_BLEND_ZERO,
        One                                 = D3D12_BLEND_ONE,
        SourceColor                         = D3D12_BLEND_SRC_COLOR,
        InvertedSourceColor                 = D3D12_BLEND_INV_SRC_COLOR,
        SourceAlpha                         = D3D12_BLEND_SRC_ALPHA,
        InvertedSourceAlpha                 = D3D12_BLEND_INV_SRC_ALPHA,
        DestinationAlpha                    = D3D12_BLEND_DEST_ALPHA,
        InvertedDestinationAlpha            = D3D12_BLEND_INV_DEST_ALPHA,
        ClampedSourceAlpha                  = D3D12_BLEND_SRC_ALPHA_SAT,
        CommandListSpecified                = D3D12_BLEND_BLEND_FACTOR,
        CommandListSpecifiedButInverted     = D3D12_BLEND_INV_BLEND_FACTOR,
        //NOTE(Tiago): the following enum values are hard to understand from the MS documetation on D3D12_BLEND, the name are based on what i was able to understand from the documentation
        DualSourceColor                     = D3D12_BLEND_SRC1_COLOR,
        DualSourceColorInverted             = D3D12_BLEND_INV_SRC1_COLOR,
        DualSourceAlpha                     = D3D12_BLEND_SRC1_ALPHA,
        DualSourceAlphaInverted             = D3D12_BLEND_INV_SRC1_ALPHA
    };

    class RenderTargetBlending
    {
        bool is_blending_enabled = false;
        bool is_a_logical_operation_enabled = false;
    };

    class Blend
    {
        bool is_alpha_to_alpha_coverage_enabled = false;
        bool is_independent_blending_enabled = false;
        RenderTargetBlending rtv_blendign[8] = {};
    };

}

}
