#include "RootSignature.h"
#include <d3d12.h>

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

}

}
