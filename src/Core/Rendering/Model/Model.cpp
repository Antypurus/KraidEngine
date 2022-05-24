#include "Model.h"

namespace Kraid
{

    BlinPhongMaterial::BlinPhongMaterial(
        const XMFLOAT3& diffuse_coeficient,
        const XMFLOAT3& specular_coeficient,
        const XMFLOAT3& ambient_coeficient)
    {
        this->ambient_coeficient = ambient_coeficient;
        this->diffuse_coeficient = diffuse_coeficient;
        this->specular_coeficient = specular_coeficient;
        this->has_texture = false;
    }

    BlinPhongMaterial::BlinPhongMaterial(
        GPUDevice& device,
        GraphicsCommandList& command_list,
        const std::string& texture_path,
        const XMFLOAT3& diffuse_coeficient,
        const XMFLOAT3& specular_coeficient,
        const XMFLOAT3& ambient_coeficient)
    {
        this->ambient_coeficient = ambient_coeficient;
        this->specular_coeficient = specular_coeficient;
        this->diffuse_coeficient = diffuse_coeficient;

        this->has_texture = true;
        this->texture = Texture(texture_path, device, command_list);
    }

    Submesh::Submesh(
        GPUDevice& device,
        GraphicsCommandList& command_list,
        const std::vector<uint32>& indices,
        BlinPhongMaterial material,
        const Transform& local_transform,
        const std::string& normal_map_path,
        PrimitiveTopology mesh_type)
    {
        if(!normal_map_path.empty())
        {
            this->has_normal_map = true;
            this->normal_map = Texture(normal_map_path, device, command_list);
        }
        else
        {
            this->has_normal_map = false;
        }

        this->index_buffer = IndexBuffer(device, indices, command_list);
        this->local_transform  = local_transform;
        this->material = material;
        this->mesh_type = mesh_type;
    }

    void Submesh::SetGlobalTransformReference(Transform* global_transform)
    {
        this->global_transform = global_transform;
    }

    Model::Model(
        GPUDevice& device,
        GraphicsCommandList& command_list,
        const std::vector<Submesh>& meshes,
        const std::vector<Vertex>& vertices,
        const Transform& global_transform)
    {
        this->global_transform = global_transform;
        this->global_vertex_buffer = VertexBuffer<Vertex>(device, command_list, vertices);
        this->submeshes = meshes;
        for(auto& mesh: this->submeshes)
        {
            mesh.SetGlobalTransformReference(&this->global_transform);
        }
    }

}
