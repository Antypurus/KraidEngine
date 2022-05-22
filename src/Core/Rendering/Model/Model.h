#pragma once

#include <Core/Rendering/Transform.h>
#include <Core/Rendering/D3D12/VertexBuffer.h>
#include <Core/Rendering/D3D12/IndexBuffer.h>
#include <vector>

namespace Kraid
{

    class BlinPhongMaterial
    {

    };

    class Submesh
    {
    };

    class Model
    {
        D3D12::VertexBuffer<D3D12::Vertex> global_vertex_buffer;
        Transform global_transform;
        std::vector<Submesh> submeshes;
    };

}
