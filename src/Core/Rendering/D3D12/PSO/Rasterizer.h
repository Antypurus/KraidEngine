#pragma once

#include <Core/Rendering/D3D12/D3D12.h>

namespace Kraid
{

namespace D3D12
{

    enum class FillMode
    {
        Solid = D3D12_FILL_MODE_SOLID,
        Wirefram = D3D12_FILL_MODE_WIREFRAME
    };

    enum class CullingMode
    {
        None = D3D12_CULL_MODE_NONE,
        Backface = D3D12_CULL_MODE_BACK ,
        Frontface = D3D12_CULL_MODE_FRONT,
    };

    enum class WindingOrder
    {
        Clockwise = FALSE,
        CounterClockwise = TRUE
    };

    enum class DepthStencilComparisonFunction
    {
        Never = D3D12_COMPARISON_FUNC_NEVER,
        Always = D3D12_COMPARISON_FUNC_ALWAYS,
        Less = D3D12_COMPARISON_FUNC_LESS,
        LessOrEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,
        Equal = D3D12_COMPARISON_FUNC_EQUAL,
        NotEqual = D3D12_COMPARISON_FUNC_NOT_EQUAL,
        Greater = D3D12_COMPARISON_FUNC_GREATER,
        GreaterOrEqual = D3D12_COMPARISON_FUNC_GREATER_EQUAL
    };

    enum class StencilOperation
    {
        Keep = D3D12_STENCIL_OP_KEEP,
        Zero = D3D12_STENCIL_OP_ZERO,
        Replace = D3D12_STENCIL_OP_REPLACE,
        IncrementAndClamp = D3D12_STENCIL_OP_INCR_SAT,
        DecrementAndClamp = D3D12_STENCIL_OP_DECR_SAT,
        Invert = D3D12_STENCIL_OP_INVERT,
        Increment = D3D12_STENCIL_OP_INCR,
        Decrement = D3D12_STENCIL_OP_DECR
    };

    class Rasterizer
    {
    public:
        FillMode fill_mode = FillMode::Solid;
        CullingMode culling_mode = CullingMode::Backface;
        WindingOrder winding_order = WindingOrder::CounterClockwise;
        bool is_conservative_rasterization_enabled = false;

    public:
        Rasterizer(
                FillMode fill_mode = FillMode::Solid,
                bool enable_conservative_rasterization = false,
                CullingMode culling_mode = CullingMode::Backface,
                WindingOrder winding_order = WindingOrder::CounterClockwise)
            :fill_mode(fill_mode), culling_mode(culling_mode), winding_order(winding_order), is_conservative_rasterization_enabled(enable_conservative_rasterization){}

        inline D3D12_RASTERIZER_DESC CreateRasterizerDescritpion() const
        {
            D3D12_RASTERIZER_DESC ret = {};
            ret.FillMode = (D3D12_FILL_MODE)this->fill_mode;
            ret.CullMode = (D3D12_CULL_MODE)this->culling_mode;
            ret.FrontCounterClockwise = (BOOL)this->winding_order;
            ret.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            ret.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            ret.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            ret.DepthClipEnable = TRUE;
            ret.MultisampleEnable = FALSE;
            ret.AntialiasedLineEnable = FALSE;
            ret.ForcedSampleCount = 0;
            ret.ConservativeRaster = this->is_conservative_rasterization_enabled?D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON:D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
            return ret;
        };
    };

    class DepthStentilStage
    {
    public:
        DepthStencilComparisonFunction comparison_function = DepthStencilComparisonFunction::Less;
        bool is_depth_testing_enabled = false;
        bool is_sentil_testing_enabled = false;

    public:
        DepthStentilStage(
            bool enable_detph_testing = true,
            DepthStencilComparisonFunction depth_testing_comparison_function = DepthStencilComparisonFunction::Less,
            bool enable_stencil_testing = false):
            comparison_function(depth_testing_comparison_function),
            is_depth_testing_enabled(enable_detph_testing),
            is_sentil_testing_enabled(enable_stencil_testing)
        {};

        D3D12_DEPTH_STENCIL_DESC CreateDepthStentilStageDescription() const
        {
            D3D12_DEPTH_STENCIL_DESC ret = {};
            ret.DepthEnable = this->is_depth_testing_enabled;
            ret.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            ret.DepthFunc = (D3D12_COMPARISON_FUNC)this->comparison_function;
            ret.StencilEnable = this->is_sentil_testing_enabled;
            ret.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
            ret.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

            D3D12_DEPTH_STENCILOP_DESC op_desc = {};
            op_desc.StencilDepthFailOp = (D3D12_STENCIL_OP) StencilOperation::Keep;
            op_desc.StencilFailOp = (D3D12_STENCIL_OP) StencilOperation::Keep;
            op_desc.StencilPassOp = (D3D12_STENCIL_OP) StencilOperation::Keep;
            op_desc.StencilFunc = (D3D12_COMPARISON_FUNC) DepthStencilComparisonFunction::Always;

            ret.FrontFace = op_desc;
            ret.BackFace = op_desc;
            return ret;
        }
    };

}

}
