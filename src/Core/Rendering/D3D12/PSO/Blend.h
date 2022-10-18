#pragma once

#include <Core/Rendering/D3D12/D3D12SDK.h>
#include <Core/types.h>

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

    enum class BlendingOperation
    {
        Add                 = D3D12_BLEND_OP_ADD,
        Subtract            = D3D12_BLEND_OP_SUBTRACT,
        ReverseSubtract     = D3D12_BLEND_OP_REV_SUBTRACT,
        Min                 = D3D12_BLEND_OP_MIN,
        Max                 = D3D12_BLEND_OP_MAX
    };

    enum class LogicOperation
    {
        Clear               = D3D12_LOGIC_OP_CLEAR,
        Set                 = D3D12_LOGIC_OP_SET,
        Copy                = D3D12_LOGIC_OP_COPY,
        InvertedCopy        = D3D12_LOGIC_OP_COPY_INVERTED,
        NoOp                = D3D12_LOGIC_OP_NOOP,
        Invert              = D3D12_LOGIC_OP_INVERT,
        AND                 = D3D12_LOGIC_OP_AND,
        NAND                = D3D12_LOGIC_OP_NAND,
        OR                  = D3D12_LOGIC_OP_OR,
        NOR                 = D3D12_LOGIC_OP_NOR,
        XOR                 = D3D12_LOGIC_OP_XOR,
        NXOR                = D3D12_LOGIC_OP_EQUIV,
        ANDOnNegatedOp2     = D3D12_LOGIC_OP_AND_REVERSE,
        ANDOnNegatedOp1     = D3D12_LOGIC_OP_AND_INVERTED,
        OROnNegatedOp2      = D3D12_LOGIC_OP_OR_REVERSE,
        OROnNegatedOp1      = D3D12_LOGIC_OP_OR_INVERTED
    };

    enum class ColorWriteMask: uint8
    {
        EnableRed       = D3D12_COLOR_WRITE_ENABLE_RED,
        EnableBlue      = D3D12_COLOR_WRITE_ENABLE_BLUE,
        EnableGreen     = D3D12_COLOR_WRITE_ENABLE_GREEN,
        EnableAlpha     = D3D12_COLOR_WRITE_ENABLE_ALPHA,
        EnableAll       = D3D12_COLOR_WRITE_ENABLE_ALL
    };

    class RenderTargetBlending
    {
    public:
        bool is_blending_enabled = false;
        bool is_a_logical_operation_enabled = false;
        BlendFactor source_color_blending = BlendFactor::Zero;
        BlendFactor destination_color_blending = BlendFactor::Zero;
        BlendingOperation color_blending_operation = BlendingOperation::Min;
        BlendFactor source_alpha_blending = BlendFactor::Zero;
        BlendFactor destination_alpha_blending = BlendFactor::Zero;
        BlendingOperation alpha_blending_operation = BlendingOperation::Min;
        LogicOperation blending_logic_operation = LogicOperation::Set;
        ColorWriteMask write_mask = ColorWriteMask::EnableAll;

        RenderTargetBlending() = default;

        inline D3D12_RENDER_TARGET_BLEND_DESC GetRenderTargetBlendingDescription() const
        {
            D3D12_RENDER_TARGET_BLEND_DESC ret = {};
            ret.BlendEnable = this->is_blending_enabled;
            ret.LogicOpEnable = this->is_a_logical_operation_enabled;
            ret.SrcBlend = (D3D12_BLEND) this->source_color_blending;
            ret.DestBlend = (D3D12_BLEND) this->destination_color_blending;
            ret.BlendOp = (D3D12_BLEND_OP) this->color_blending_operation;
            ret.SrcBlendAlpha = (D3D12_BLEND) this->source_alpha_blending;
            ret.DestBlendAlpha = (D3D12_BLEND) this->destination_alpha_blending;
            ret.BlendOpAlpha = (D3D12_BLEND_OP) this->alpha_blending_operation;
            ret.LogicOp = (D3D12_LOGIC_OP) this->blending_logic_operation;
            ret.RenderTargetWriteMask = (UINT8)this->write_mask;

            return ret;
        }
    };

    class Blend
    {
    public:
        bool is_alpha_to_alpha_coverage_enabled = false;
        bool is_independent_blending_enabled = false;
        RenderTargetBlending rtv_blending[8] = {};

        inline D3D12_BLEND_DESC GetBlendingDescription() const
        {
            D3D12_BLEND_DESC ret = {};
            ret.AlphaToCoverageEnable = this->is_alpha_to_alpha_coverage_enabled;
            ret.IndependentBlendEnable = this->is_independent_blending_enabled;
            for(uint8 i = 0; i < 8; ++i)
            {
                ret.RenderTarget[i] = this->rtv_blending[i].GetRenderTargetBlendingDescription();
            }

            return ret;
        }
    };

}

}
