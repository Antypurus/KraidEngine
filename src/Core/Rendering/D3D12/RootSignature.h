#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/types.h>
#include <d3d12.h>

namespace Kraid
{

namespace D3D12
{

    using namespace Microsoft::WRL;

    enum class RooParameterType
    {
        RootConstant = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
        DescriptorTable = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        UAVDescriptor = D3D12_ROOT_PARAMETER_TYPE_UAV,
        SRCDescriptor = D3D12_ROOT_PARAMETER_TYPE_SRV,
        CBVDescriptor = D3D12_ROOT_PARAMETER_TYPE_CBV
    };

    enum class ShaderVisibility
    {
        All             = D3D12_SHADER_VISIBILITY_ALL,
        Vertex          = D3D12_SHADER_VISIBILITY_VERTEX,
        Hull            = D3D12_SHADER_VISIBILITY_HULL,
        Domain          = D3D12_SHADER_VISIBILITY_DOMAIN,
        Geometry        = D3D12_SHADER_VISIBILITY_GEOMETRY,
        Pixel           = D3D12_SHADER_VISIBILITY_PIXEL,
        Amplification   = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
        Mesh            = D3D12_SHADER_VISIBILITY_MESH
    };

    class ConstantRootParameter
    {
    public:
        ShaderVisibility visibility = ShaderVisibility::All;
        uint8 register_slot = 0;
        uint8 space_slot = 0;
        uint64 constant_buffer_size = 1;

        ConstantRootParameter(uint8 register_slot = 0, uint8 space_slot = 0, uint64 buffer_size = 1, ShaderVisibility visibility = ShaderVisibility::All)
            :register_slot(register_slot),
            space_slot(space_slot),
            constant_buffer_size(buffer_size),
            visibility(visibility)
        {}

        inline D3D12_ROOT_PARAMETER GetRootParameterDescriptiton() const
        {
            D3D12_ROOT_CONSTANTS constant_root_parameter_desc = {};
            constant_root_parameter_desc.Num32BitValues = this->constant_buffer_size;
            constant_root_parameter_desc.RegisterSpace = this->register_slot;
            constant_root_parameter_desc.ShaderRegister = this->space_slot;

            D3D12_ROOT_PARAMETER ret = {};
            ret.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            ret.Constants = constant_root_parameter_desc;
            ret.ShaderVisibility = (D3D12_SHADER_VISIBILITY) this->visibility;

            return ret;
        }
    };

    class DescriptorRootParameter
    {
    public:
        uint8 register_slot = 0;
        uint8 space_slot = 0;
        D3D12_ROOT_PARAMETER_TYPE descriptor_type;

        inline D3D12_ROOT_PARAMETER GetRootParameterDescription() const
        {

        }
    };

    class RootSignature
    {
    public:
        ComPtr<ID3D12RootSignature> root_signature = nullptr;
    };

}

}
