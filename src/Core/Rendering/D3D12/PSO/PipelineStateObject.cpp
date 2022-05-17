#include "PipelineStateObject.h"

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/Windows.h>

#include <dxgiformat.h>
#include <stdlib.h>

namespace Kraid
{

namespace D3D12
{

    //NOTE(Tiago):should this maybe be in the index buffer or vertex buffer class
    enum class TriangleStripDiscontinuityIndicator
    {
        None = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
        Cut0xFFFF = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF,
        Cut0xFFFFFFFF = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
    };
    
    GraphicsPipelineStateObject::GraphicsPipelineStateObject(
                GPUDevice& device,
                VertexShader& vertex_shader,
                PixelShader& pixel_shader,
                const RootSignature& root_signature,
                PrimitiveTopology topology_type,
                D3D12_INPUT_LAYOUT_DESC vertex_layout,
                Rasterizer rasterizer,
                DepthStentilStage depth_stencil_stage,
                Blend blending,
                StreamingOutputBuffer so_buffer):
        vertex_shader(&vertex_shader),
        pixel_shader(&pixel_shader),
        root_signature(root_signature),
        topology_type(topology_type),
        vertex_layout(vertex_layout),
        rasterizer(rasterizer),
        depth_stencil_stage(depth_stencil_stage),
        blending(blending),
        streaming_output_buffer(so_buffer)
    {
        vertex_shader.RegisterShaderRecompilationNotificationCallback([this, &device](){
                    this->Compile(device);
                });
        pixel_shader.RegisterShaderRecompilationNotificationCallback([this, &device](){
                    this->Compile(device);
                });

        this->Compile(device);
    }

    GraphicsPipelineStateObject::~GraphicsPipelineStateObject()
    {
        free((void*)this->vertex_layout.pInputElementDescs);
    }

    void GraphicsPipelineStateObject::Compile(GPUDevice& device)
    {

        D3D12_SHADER_BYTECODE null_bytecode = {};

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
        pso_desc.pRootSignature = this->root_signature.root_signature.Get();
        pso_desc.VS = this->vertex_shader != nullptr?this->vertex_shader->GetShaderBytecode():null_bytecode;
        pso_desc.PS = this->pixel_shader != nullptr?this->pixel_shader->GetShaderBytecode():null_bytecode;
        pso_desc.DS = this->domain_shader != nullptr?this->domain_shader->GetShaderBytecode(): null_bytecode;
        pso_desc.HS = this->hull_shader != nullptr?this->hull_shader->GetShaderBytecode():null_bytecode;
        pso_desc.GS = this->geometry_shader != nullptr?this->geometry_shader->GetShaderBytecode():null_bytecode;

        pso_desc.StreamOutput = this->streaming_output_buffer.GetStreamingOutputBufferDescritpion();
        pso_desc.BlendState = this->blending.GetBlendingDescription();
        pso_desc.SampleMask = UINT_MAX;//TODOT(Tiago): what is this
        pso_desc.RasterizerState = this->rasterizer.CreateRasterizerDescritpion();
        pso_desc.DepthStencilState = this->depth_stencil_stage.CreateDepthStentilStageDescription();
        pso_desc.InputLayout = this->vertex_layout;
        pso_desc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE) this->topology_type;
        pso_desc.IBStripCutValue = (D3D12_INDEX_BUFFER_STRIP_CUT_VALUE) TriangleStripDiscontinuityIndicator::None;
        pso_desc.NumRenderTargets = 1;//TODO(Tiago):hardcoded right now
        //setting rtv formats
        {
            pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        }
        //TODO(Tiago):we need some way to specify this when a triangle strip is being used
        pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        pso_desc.SampleDesc.Count = 1;
        pso_desc.SampleDesc.Quality = 0;
        pso_desc.NodeMask = 0;
        pso_desc.CachedPSO = {};
        pso_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        uint8 pso_index = (this->pso_index + 1)%2;
        D3DCALL(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&this->pso[pso_index])), "Pipeline State Object Compiled");

        this->compilation_mutex.Lock();
        this->swap_pso_index = true;
        this->compilation_mutex.Unlock();
    }

    void GraphicsPipelineStateObject::Bind(GraphicsCommandList& command_list)
    {
        this->compilation_mutex.Lock();
        if(this->swap_pso_index)
        {
            this->pso_index = (this->pso_index + 1)%2;
            this->swap_pso_index = false;
        }
        this->compilation_mutex.Unlock();

        if(this->pso[this->pso_index] != nullptr)
        {
            command_list->SetGraphicsRootSignature(this->root_signature.root_signature.Get());
            command_list->SetPipelineState(this->pso[this->pso_index].Get());
            D3D12_PRIMITIVE_TOPOLOGY topology;
            switch (this->topology_type)
            {
                case(PrimitiveTopology::Undefined):
                {
                    topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
                    break;
                }
                case(PrimitiveTopology::Point):
                {
                    topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                    break;
                }
                case(PrimitiveTopology::Line):
                {
                    topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                    break;
                }
                case(PrimitiveTopology::Triangle):
                {
                    topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                    break;
                }
                case(PrimitiveTopology::Patch):
                {
                    topology = D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
                    break;
                }
            }
            command_list->IASetPrimitiveTopology(topology);
        }
    }

}

}
