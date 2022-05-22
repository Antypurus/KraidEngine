#include "VertexBuffer.h"

#include <d3d12.h>
#include <stdlib.h>

namespace Kraid
{
    namespace D3D12
    {

        D3D12_INPUT_LAYOUT_DESC Vertex::GenerateVertexDescription()
        {
            D3D12_INPUT_ELEMENT_DESC position_desc = {};
            position_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            position_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            position_desc.SemanticName = "POSITION";
            position_desc.AlignedByteOffset = 0;
            position_desc.InputSlot = 0;
            position_desc.SemanticIndex = 0;
            position_desc.InputSlot = 0;
            position_desc.InstanceDataStepRate = 0;

            D3D12_INPUT_ELEMENT_DESC normal_desc = {};
            normal_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            normal_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            normal_desc.SemanticName = "NORMAL";
            normal_desc.AlignedByteOffset = 12;
            normal_desc.InputSlot = 0;
            normal_desc.InstanceDataStepRate = 0;
            normal_desc.SemanticIndex = 0;

            D3D12_INPUT_ELEMENT_DESC color_desc = {};
            color_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            color_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            color_desc.SemanticName = "COLOR";
            color_desc.AlignedByteOffset = 24;
            color_desc.InputSlot = 0;
            color_desc.InstanceDataStepRate = 0;
            color_desc.SemanticIndex = 0;
            
            D3D12_INPUT_ELEMENT_DESC uv_desc = {};
            uv_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            uv_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
            uv_desc.SemanticName = "UV";
            uv_desc.AlignedByteOffset = 36;
            uv_desc.InputSlot = 0;
            uv_desc.InstanceDataStepRate = 0;
            uv_desc.SemanticIndex = 0;

            D3D12_INPUT_LAYOUT_DESC ret = {};
            ret.pInputElementDescs = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * 4);
            memcpy((void*)&ret.pInputElementDescs[0], (void*)&position_desc, sizeof(D3D12_INPUT_ELEMENT_DESC));
            memcpy((void*)&ret.pInputElementDescs[1], (void*)&normal_desc, sizeof(D3D12_INPUT_ELEMENT_DESC));
            memcpy((void*)&ret.pInputElementDescs[2], (void*)&color_desc, sizeof(D3D12_INPUT_ELEMENT_DESC));
            memcpy((void*)&ret.pInputElementDescs[3], (void*)&uv_desc, sizeof(D3D12_INPUT_ELEMENT_DESC));
            ret.NumElements = 4;
            return ret;
        }

    }
}
