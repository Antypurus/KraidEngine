#include "Model.h"
#include "DearImGui/imgui.h"

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Rendering/GUI.h>

#include <string>
#include <sstream>

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

        this->texture = &Texture::LoadTexture(texture_path, device, command_list);
    }

    Submesh::Submesh(
        GPUDevice& device,
        GraphicsCommandList& command_list,
        const std::vector<uint32>& indices,
        BlinPhongMaterial* material,
        const Transform local_transform,
        const std::string& normal_map_path)
    {
        if(!normal_map_path.empty())
        {
            this->normal_map = &Texture::LoadTexture(normal_map_path, device, command_list);
        }

        this->index_buffer = IndexBuffer(device, indices, command_list);
        this->local_transform  = local_transform;
        this->material = material;

        Transform final_transform = this->local_transform;
        if(this->global_transform != nullptr)
        {
            final_transform = final_transform + *this->global_transform;
        }
        XMMATRIX model_matrix = final_transform.GetModelMatrix(); 
        this->model_cbuffer = ShaderParameter<XMMATRIX>(device, command_list, model_matrix);
    }

    void Submesh::SetGlobalTransformReference(Transform* global_transform)
    {
        this->global_transform = global_transform;
    }

    void Submesh::Draw(GraphicsCommandList &command_list, uint32 texture_slot, uint32 model_matrix_slot, uint32 normal_map_slot)
    {
        Transform final_transform = this->local_transform;
        if(this->global_transform != nullptr)
        {
            final_transform = final_transform + *this->global_transform;
        }
        XMMATRIX model_matrix = final_transform.GetModelMatrix();
        this->model_cbuffer.UpdateData(model_matrix, command_list);
        this->model_cbuffer.Bind(command_list, model_matrix_slot);

        this->index_buffer.Bind(command_list);
        if(this->material->texture != nullptr)
        {
            this->material->texture->BindDefaultSRV(command_list, texture_slot);
        }
        if(this->normal_map != nullptr)
        {
            this->normal_map->BindDefaultSRV(command_list, normal_map_slot);
        }
        command_list->DrawIndexedInstanced(this->index_buffer.index_count, 1, 0, 0, 0);
        
        std::stringstream ss;
        ss.clear();
        ss << this->index;  
        std::string name = ss.str();
        if(ImGui::CollapsingHeader(name.c_str()))
        {
            ImGui::SliderFloat3(("Translation " + name).c_str(), this->local_transform.translation, -100.0f, 100.0f);
        }
    }

    Model::Model(
        GPUDevice& device,
        GraphicsCommandList& command_list,
        const std::vector<Submesh>& meshes,
        const std::vector<Vertex>& vertices,
        std::vector<BlinPhongMaterial>&& materials,
        const Transform& global_transform)
    {
        this->global_transform = global_transform;
        this->global_vertex_buffer = VertexBuffer<Vertex>(device, command_list, vertices);
        this->submeshes = meshes;
        this->materials = std::move(materials);
        for (auto& mesh : this->submeshes)
        {
            mesh.SetGlobalTransformReference(&this->global_transform);
        }
        for (uint64 i = 0; i < this->submeshes.size(); ++i)
        {
            this->submeshes[i].index = i;
        }
    }

    void Model::Draw(GraphicsCommandList& command_list, uint32 texture_slot, uint32 model_matrix_slot, uint32 normal_map_slot)
    {
        if (this->global_vertex_buffer.vertex_buffer.resource != nullptr)//TODO(Tiago): figure out better way to detect unloaded model
        {
            this->global_vertex_buffer.Bind(command_list);
            for (auto& submesh : this->submeshes)
            {
                submesh.Draw(command_list, texture_slot,model_matrix_slot, normal_map_slot);
            }
        }
        else
        {
            LERROR("Attempted to draw not loaded model");
        }

        auto mesh = this->submeshes[0];
        if(ImGui::CollapsingHeader("Model Header"))
        {
            static float translation[3];
            ImGui::SliderFloat3("Translation", translation, -100.0f, 100.0f);

            this->global_transform.translation[0] = translation[0];
            this->global_transform.translation[1] = translation[1];
            this->global_transform.translation[2] = translation[2];

        }
    }

}
