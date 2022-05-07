#include "PipelineStateObject.h"

namespace Kraid
{

namespace D3D12
{

    void GraphicsPipelineStateObject::Compile(GPUDevice& device)
    {
        D3D12_SHADER_BYTECODE null_bytecode = {};

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
        pso_desc.VS = this->vertex_shader != nullptr?this->vertex_shader->GetShaderBytecode():null_bytecode;
        pso_desc.PS = this->pixel_shader != nullptr?this->pixel_shader->GetShaderBytecode():null_bytecode;
        pso_desc.DS = this->domain_shader != nullptr?this->domain_shader->GetShaderBytecode(): null_bytecode;
        pso_desc.HS = this->hull_shader != nullptr?this->hull_shader->GetShaderBytecode():null_bytecode;
        pso_desc.GS = this->geometry_shader != nullptr?this->geometry_shader->GetShaderBytecode():null_bytecode;
    }

}

}
