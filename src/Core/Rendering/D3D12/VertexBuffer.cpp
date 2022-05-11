#include "VertexBuffer.h"

#include <d3d12.h>
#include <stdlib.h>

namespace Kraid
{
    namespace D3D12
    {

        D3D12_INPUT_LAYOUT_DESC BasicVertex::GenerateVertexDescription()
        {
            D3D12_INPUT_ELEMENT_DESC position_desc = {};
            position_desc.InputSlot = 0;
            position_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            position_desc.SemanticName = "POSITION";
            position_desc.SemanticIndex = 0;
            position_desc.InputSlot = 0;
            position_desc.AlignedByteOffset = 0;
            position_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            position_desc.InstanceDataStepRate = 0;

            D3D12_INPUT_LAYOUT_DESC ret = {};
            ret.pInputElementDescs = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC));
            memcpy((void*)&ret.pInputElementDescs[0], (void*)&position_desc, sizeof(D3D12_INPUT_ELEMENT_DESC));
            ret.NumElements = 1;
            return ret;
        }

    }
}
