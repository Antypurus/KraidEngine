#pragma once

#include <Core/Rendering/D3D12/VertexBuffer.h>
#include <Core/Rendering/D3D12/IndexBuffer.h>
#include <Core/Rendering/D3D12/Texture.h>
#include <Core/Rendering/Transform.h>
#include <Core/Rendering/D3D12/ShaderParameter.h>

#include <Core/stdcpp.h>

//TODO(Tiago): at one point or another we might want to consider making API independent
namespace Kraid
{

    using namespace D3D12;

    namespace D3D12
    {
        struct GPUDevice;
        struct GraphicsCommandList;
    };

    class BlinPhongMaterial
    {
    public:
        XMFLOAT3 diffuse_coeficient = {0,0,0};
        XMFLOAT3 specular_coeficient = {0,0,0};
        XMFLOAT3 ambient_coeficient = {0,0,0};
        Texture* texture = nullptr;//NOTE(Tiago):just a reference

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
        
        inline BlinPhongMaterial& operator=(const BlinPhongMaterial& other)
        {
            this->texture = other.texture;
            this->diffuse_coeficient = other.diffuse_coeficient;
            this->ambient_coeficient = other.ambient_coeficient;
            this->specular_coeficient = other.specular_coeficient;

            return *this;
        }
    };

    class Submesh
    {
    public:
        IndexBuffer index_buffer;
        Texture* normal_map = nullptr;//NOTE(Tiago):just a reference
        Transform local_transform;
        Transform* global_transform = nullptr;
        BlinPhongMaterial* material;
        ShaderParameter<XMMATRIX> model_cbuffer;
        uint64 index;

        Submesh() = default;
        Submesh(
            GPUDevice& device,
            GraphicsCommandList& command_list,
            const std::vector<uint32>& indices,
            BlinPhongMaterial* material,
            const Transform local_transform = Transform(),
            const std::string& normal_map_path = ""
        );

        void SetGlobalTransformReference(Transform* global_transform);
        void Draw(GraphicsCommandList& command_list, uint32 texture_slot = 0,uint32 model_matrix_slot = 0, uint32 normal_map_slot = 0);
    };

    class Model
    {
    public:
        VertexBuffer<Vertex> global_vertex_buffer;
        Transform global_transform;
        std::vector<Submesh> submeshes;
        std::vector<BlinPhongMaterial> materials;

        Model() = default;
        Model(
            GPUDevice& device,
            GraphicsCommandList& command_list,
            const std::vector<Submesh>& meshes,
            const std::vector<Vertex>& vertices,
            std::vector<BlinPhongMaterial>&& materials,
            const Transform& global_transform = Transform()
        );
        void Draw(GraphicsCommandList& command_list, uint32 texture_slot = 0, uint32 model_matrix_slot = 0, uint32 normal_map_slot = 0);
    };

}
