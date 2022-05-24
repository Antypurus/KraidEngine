#pragma once

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Rendering/Transform.h>
#include <Core/Rendering/D3D12/VertexBuffer.h>
#include <Core/Rendering/D3D12/IndexBuffer.h>
#include <Core/Rendering/D3D12/Texture.h>

#include <vector>
#include <string>

//TODO(Tiago): at one point or another we might want to consider making API independent
namespace Kraid
{

    using namespace D3D12;

    class BlinPhongMaterial
    {
    public:
        XMFLOAT3 diffuse_coeficient = {0,0,0};
        XMFLOAT3 specular_coeficient = {0,0,0};
        XMFLOAT3 ambient_coeficient = {0,0,0};

        Texture texture;
        bool has_texture = false;

        BlinPhongMaterial(
            const XMFLOAT3& diffuse_coeficient  = {0,0,0},
            const XMFLOAT3& specular_coeficient = {0,0,0},
            const XMFLOAT3& ambient_coeficient = {0,0,0}
        );
        BlinPhongMaterial(
            GPUDevice& device,
            GraphicsCommandList& command_list,
            const std::string& texture_path,
            const XMFLOAT3& diffuse_coeficient  = {0,0,0},
            const XMFLOAT3& specular_coeficient = {0,0,0},
            const XMFLOAT3& ambient_coeficient = {0,0,0}
        );

    };

    class Submesh
    {
    public:
        IndexBuffer index_buffer;
        
        bool has_normal_map = false;
        Texture normal_map;

        Transform local_transform;
        Transform* global_transform = nullptr;
        BlinPhongMaterial material;

        PrimitiveTopology mesh_type = PrimitiveTopology::Triangle;

        Submesh() = default;
        Submesh(
            GPUDevice& device,
            GraphicsCommandList& command_list,
            const std::vector<uint32>& indices,
            BlinPhongMaterial material,
            const Transform& local_transform = Transform(),
            const std::string& normal_map_path = "",
            PrimitiveTopology mesh_type = PrimitiveTopology::Triangle
        );

        void SetGlobalTransformReference(Transform* global_transform);
    };

    class Model
    {
    public:
        VertexBuffer<Vertex> global_vertex_buffer;
        Transform global_transform;
        std::vector<Submesh> submeshes;

        Model() = default;
        Model(
            GPUDevice& device,
            GraphicsCommandList& command_list,
            const std::vector<Submesh>& meshes,
            const std::vector<Vertex>& vertices,
            const Transform& global_transform = Transform()
        );
    };

}
