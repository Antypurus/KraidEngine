#include "VertexBuffer.h"

namespace Kraid
{
    namespace D3D12
    {

        inline D3D12_INPUT_LAYOUT_DESC BasicVertex::GenerateVertexDescription() const
        {
            D3D12_INPUT_ELEMENT_DESC position_desc = {};
            position_desc.InputSlot = 0;
            position_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            position_desc.SemanticName = "POSITION";
            position_desc.SemanticIndex = 0;
            position_desc.InputSlot = 0;
            position_desc.AlignedByteOffset = 0;
            position_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            position_desc.InstanceDataStepRate = 0;

            D3D12_INPUT_LAYOUT_DESC ret = {};
            ret.pInputElementDescs = {
                &position_desc
            };
            ret.NumElements = 1;
            return ret;
        }

    }
}
