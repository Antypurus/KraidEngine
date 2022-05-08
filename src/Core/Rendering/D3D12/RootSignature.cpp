#include "RootSignature.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

namespace Kraid
{

namespace D3D12
{

    void DescriptorTableRootParameter::AddEntry(DescriptorTableEntryType entry_type, uint8 register_slot, uint8 space_slot)
    {
        D3D12_DESCRIPTOR_RANGE descriptor = {};
        descriptor.NumDescriptors = 1;
        descriptor.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        descriptor.RangeType = (D3D12_DESCRIPTOR_RANGE_TYPE) entry_type;
        descriptor.BaseShaderRegister = register_slot;
        descriptor.RegisterSpace = space_slot;

        this->descriptor_table.push_back(descriptor);
    }

    void DescriptorTableRootParameter::AddCBVEntry(uint8 register_slot, uint8 space_slot)
    {
        this->AddEntry(DescriptorTableEntryType::CBV, register_slot, space_slot);
    }

    void DescriptorTableRootParameter::AddSRVEntry(uint8 register_slot, uint8 space_slot)
    {
        this->AddEntry(DescriptorTableEntryType::SRV, register_slot, space_slot);
    }

    void DescriptorTableRootParameter::AddUAVEntry(uint8 register_slot, uint8 space_slot)
    {
        this->AddEntry(DescriptorTableEntryType::UAV, register_slot, space_slot);
    }

    void DescriptorTableRootParameter::AddSamplerEntry(uint8 register_slot, uint8 space_slot)
    {
        this->AddEntry(DescriptorTableEntryType::Sampler, register_slot, space_slot);
    }

    DescriptorTableRootParameter::DescriptorTableRootParameter(const std::vector<DescriptorTableEntry>& descriptor_table_entries, ShaderVisibility visibility)
    {
        this->visibility = visibility;
        for(auto& entry: descriptor_table_entries)
        {
            this->descriptor_table.push_back(entry.GetDescriptorTableEntryDescription());
        }
    }

    D3D12_ROOT_PARAMETER DescriptorTableRootParameter::GetRootParameterDescription() const
    {
        D3D12_ROOT_DESCRIPTOR_TABLE descriptor_table_desc = {};
        descriptor_table_desc.NumDescriptorRanges = this->descriptor_table.size();
        descriptor_table_desc.pDescriptorRanges = this->descriptor_table.data();

        D3D12_ROOT_PARAMETER ret = {};
        ret.ShaderVisibility = (D3D12_SHADER_VISIBILITY) this->visibility;
        ret.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        ret.DescriptorTable = descriptor_table_desc;

        return ret;
    }

    RootSignature::RootSignature(
            GPUDevice& device,
            const std::vector<ConstantRootParameter>& constant_root_parameters,
            const std::vector<RootDescriptorParameter>& root_descriptor_parameters,
            const std::vector<DescriptorTableRootParameter>& descriptor_table_root_parameters):
        constant_root_parameters(constant_root_parameters),
        root_descriptor_parameters(root_descriptor_parameters),
        descriptor_table_root_parameters(descriptor_table_root_parameters)
    {
        this->Compile(device);
    }

    void RootSignature::Compile(GPUDevice& device)
    {
        const uint64 root_parameter_count = 
            this->constant_root_parameters.size() +
            this->root_descriptor_parameters.size() +
            this->descriptor_table_root_parameters.size();
        std::vector<D3D12_ROOT_PARAMETER> root_parameters(root_parameter_count);
        
        for(auto& root_parameter: this->constant_root_parameters)
        {
            root_parameters.push_back(root_parameter.GetRootParameterDescription());
        }
        for(auto& root_parameter: this->root_descriptor_parameters)
        {
            root_parameters.push_back(root_parameter.GetRootParameterDescription());
        }
        for(auto& root_parameter: this->descriptor_table_root_parameters)
        {
            root_parameters.push_back(root_parameter.GetRootParameterDescription());
        }

        D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
        root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        root_signature_desc.NumStaticSamplers = 0;
        root_signature_desc.pStaticSamplers = nullptr;
        root_signature_desc.NumParameters = root_parameter_count;
        root_signature_desc.pParameters = root_parameters.data();

        ComPtr<ID3DBlob> serialized_root_signature = nullptr;
        ComPtr<ID3DBlob> errors = nullptr;
        D3DCALL(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1_1, &serialized_root_signature, &errors), "Root Signature Serialized");
        if(errors != nullptr)
        {
            LERROR("%s", errors->GetBufferPointer());
            errors->Release();
        }

        D3DCALL(device->CreateRootSignature(
                    0, //NODEMASK
                    serialized_root_signature->GetBufferPointer(),
                    serialized_root_signature->GetBufferSize(),
                    IID_PPV_ARGS(&this->root_signature)), "Root Signature Created");
    }

}

}
