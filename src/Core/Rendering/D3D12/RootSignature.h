#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/types.h>
#include <Core/stdcpp.h>

namespace Kraid
{

namespace D3D12
{

    struct GPUDevice;

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
            :visibility(visibility),
            register_slot(register_slot),
            space_slot(space_slot),
            constant_buffer_size(buffer_size)
        {}

        inline D3D12_ROOT_PARAMETER GetRootParameterDescription() const
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

    class RootDescriptorParameter
    {
    public:
        uint8 register_slot = 0;
        uint8 space_slot = 0;
        ShaderVisibility visibility = ShaderVisibility::All;
        D3D12_ROOT_PARAMETER_TYPE descriptor_type;

        RootDescriptorParameter() = default;
    protected:
        RootDescriptorParameter(uint8 register_slot, uint8 space_slot, D3D12_ROOT_PARAMETER_TYPE descriptor_type, ShaderVisibility visibility)
            :register_slot(register_slot),
            space_slot(space_slot),
            visibility(visibility),
            descriptor_type(descriptor_type)
        {};

    public:
        inline D3D12_ROOT_PARAMETER GetRootParameterDescription() const
        {
            D3D12_ROOT_DESCRIPTOR root_descriptor = {};
            root_descriptor.RegisterSpace = this->space_slot;
            root_descriptor.ShaderRegister = this->register_slot;
            
            D3D12_ROOT_PARAMETER ret = {};
            ret.ParameterType = this->descriptor_type;
            ret.Descriptor = root_descriptor;
            ret.ShaderVisibility = (D3D12_SHADER_VISIBILITY) this->visibility;

            return ret;
        }
    };

    class CBVRootDescriptorPararmeter: public RootDescriptorParameter
    {
    public:
        CBVRootDescriptorPararmeter(uint8 register_slot = 0, uint8 space_slot = 0, ShaderVisibility visibility = ShaderVisibility::All)
            :RootDescriptorParameter(register_slot, space_slot, D3D12_ROOT_PARAMETER_TYPE_CBV, visibility) {};
    };

    class SRVRootDescriptorParameter: public RootDescriptorParameter
    {
    public:
        SRVRootDescriptorParameter(uint8 register_slot = 0, uint8 space_slot = 0, ShaderVisibility visibility = ShaderVisibility::All)
        :RootDescriptorParameter(register_slot, space_slot, D3D12_ROOT_PARAMETER_TYPE_SRV, visibility) {};
    };

    class UAVRootDescriptorParameter: public RootDescriptorParameter
    {
    public:
        UAVRootDescriptorParameter(uint8 register_slot = 0, uint8 space_slot = 0, ShaderVisibility visibility = ShaderVisibility::All)
        :RootDescriptorParameter(register_slot, space_slot, D3D12_ROOT_PARAMETER_TYPE_UAV, visibility) {};
    };

    enum class DescriptorTableEntryType
    {
        CBV = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        SRV = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        UAV = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
        Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER
    };

    class DescriptorTableEntry
    {
    public:
        uint8 register_slot = 0;
        uint8 space_slot = 0;
        uint64 entry_buffer_size = 1;
        DescriptorTableEntryType type;

        DescriptorTableEntry() = default;
        DescriptorTableEntry(uint8 register_slot, uint8 space_slot, uint64 entry_buffer_size, DescriptorTableEntryType entry_type):
            register_slot(register_slot),
            space_slot(space_slot),
            entry_buffer_size(entry_buffer_size),
            type(entry_type) {};

        inline D3D12_DESCRIPTOR_RANGE GetDescriptorTableEntryDescription() const
        {
            D3D12_DESCRIPTOR_RANGE ret = {};
            ret.NumDescriptors = this->entry_buffer_size;
            ret.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            ret.RangeType = (D3D12_DESCRIPTOR_RANGE_TYPE) this->type;
            ret.BaseShaderRegister = this->register_slot;
            ret.RegisterSpace = this->space_slot;

            return ret;
        }
    };

    class CBVDescriptorTableEntry: public DescriptorTableEntry
    {
    public:
        CBVDescriptorTableEntry(uint8 register_slot = 0, uint8 space_slot = 0, uint64 entry_buffer_size = 1):
            DescriptorTableEntry(register_slot, space_slot, entry_buffer_size, DescriptorTableEntryType::CBV) {};
    };

    class SRVDescriptorTableEntry: public DescriptorTableEntry
    {
    public:
        SRVDescriptorTableEntry(uint8 register_slot = 0, uint8 space_slot = 0, uint64 entry_buffer_size = 1):
            DescriptorTableEntry(register_slot, space_slot, entry_buffer_size, DescriptorTableEntryType::SRV) {};
    };

    class UAVDescriptorTableEntry: public DescriptorTableEntry
    {
    public:
        UAVDescriptorTableEntry(uint8 register_slot = 0, uint8 space_slot = 0, uint64 entry_buffer_size = 1):
            DescriptorTableEntry(register_slot, space_slot, entry_buffer_size, DescriptorTableEntryType::UAV) {};
    };

    class SamplerDescriptorTableEntry: public DescriptorTableEntry
    {
    public:
        SamplerDescriptorTableEntry(uint8 register_slot = 0, uint8 space_slot = 0, uint64 entry_buffer_size = 1):
            DescriptorTableEntry(register_slot, space_slot, entry_buffer_size, DescriptorTableEntryType::Sampler) {};
    };

    class DescriptorTableRootParameter
    {
    public:
        ShaderVisibility visibility = ShaderVisibility::All;
        std::vector<D3D12_DESCRIPTOR_RANGE> descriptor_table;

        DescriptorTableRootParameter(const std::vector<DescriptorTableEntry>& descriptor_table_entries = {}, ShaderVisibility visibility = ShaderVisibility::All);
        void AddCBVEntry(uint8 register_slot = 0, uint8 space_slot = 0);
        void AddSRVEntry(uint8 register_slot = 0, uint8 space_slot = 0);
        void AddUAVEntry(uint8 register_slot = 0, uint8 space_slot = 0);
        void AddSamplerEntry(uint8 register_slot = 0, uint8 space_slot = 0);

        D3D12_ROOT_PARAMETER GetRootParameterDescription() const;

    private:
        void inline AddEntry(DescriptorTableEntryType entry_type, uint8 register_slot = 0, uint8 space_slot = 0);
    };

    //TODO(Tiago):add support for static samplers
    class RootSignature
    {
    public:
        ComPtr<ID3D12RootSignature> root_signature = nullptr;
        std::vector<ConstantRootParameter> constant_root_parameters;
        std::vector<RootDescriptorParameter> root_descriptor_parameters;
        std::vector<DescriptorTableRootParameter> descriptor_table_root_parameters;

        RootSignature(
                const std::vector<ConstantRootParameter>& constant_root_parameters = {},
                const std::vector<RootDescriptorParameter>& root_descriptor_parameters = {},
                const std::vector<DescriptorTableRootParameter>& descriptor_table_root_parameters = {});
    private:
        void inline Compile(GPUDevice& device);
    };

}

}
