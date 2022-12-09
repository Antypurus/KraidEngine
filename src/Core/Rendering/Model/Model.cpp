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

        this->texture = &Texture::LoadTexture(texture_path, command_list);
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
            this->normal_map = &Texture::LoadTexture(normal_map_path, command_list);
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
        this->model_cbuffer = ShaderParameter<XMMATRIX>(command_list, model_matrix);
    }

    void Submesh::SetGlobalTransformReference(Transform* global_transform)
    {
        this->global_transform = global_transform;
    }

    void Submesh::UpdateTransformCBuffer(GraphicsCommandList& command_list, uint32 model_matrix_slot)
    {
        if(this->local_transform.HasChanged() || (this->global_transform != nullptr && this->global_transform->HasChanged()))
        {
            this->local_transform.SetChangedFlag(false);
            Transform final_transform = this->local_transform;
            if(this->global_transform != nullptr)
            {
                final_transform = final_transform + *this->global_transform;
            }
            XMMATRIX model_matrix = final_transform.GetModelMatrix();
            this->model_cbuffer.UpdateData(model_matrix, command_list);
        }
        this->model_cbuffer.Bind(command_list, model_matrix_slot);
    }

    void Submesh::Draw(GraphicsCommandList &command_list, uint32 texture_slot, uint32 model_matrix_slot, uint32 normal_map_slot)
    {
        this->UpdateTransformCBuffer(command_list, model_matrix_slot);

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
    }

    void Submesh::DrawDebugGUI()
    {
        if(ImGui::TreeNode(("Submesh #" + std::to_string(this->index)).c_str()))
        {
            float position[3];
            float scale[3];
            float rotation[3];
            memcpy(position, this->local_transform.translation, 3 * sizeof(float));
            memcpy(scale, this->local_transform.scale, 3 * sizeof(float));
            memcpy(rotation, this->local_transform.rotation, 3 * sizeof(float));

            ImGui::SliderFloat3("Translation", position, -5, 5);
            ImGui::SliderFloat3("Scale", scale, 0.000001f, 10);
            ImGui::SliderFloat3("Rotation", rotation, 0, 2.14f);
            this->local_transform.SetTranslation(position[0], position[1], position[2]);
            this->local_transform.SetScale(scale[0], scale[1], scale[2]);
            this->local_transform.SetRotation(rotation[0], rotation[1], rotation[2]);

            if(ImGui::Button("Reset Transform"))
            {
                this->local_transform = {};
            }

            ImGui::TreePop();
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
        for(uint64 i = 0; i < vertices.size(); ++i)
        {
            this->global_aabb.UpdateBounds(
                        vertices[i].position.x,
                        vertices[i].position.y,
                        vertices[i].position.z);
        }
        float model_scale = this->global_aabb.ComputeUnitScaleTransform();
        this->global_transform.SetScale(
                    model_scale,
                    model_scale,
                    model_scale
                );

        std::tuple<float,float,float> model_center = this->global_aabb.ComputeUnitCenterPoint();
        this->global_transform.SetTranslation(
                std::get<0>(model_center),
                std::get<1>(model_center),
                std::get<2>(model_center));

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
            this->global_transform.SetChangedFlag(false);
        }
        else
        {
            LERROR("Attempted to draw not loaded model");
        }

        this->DrawDebugGUI();
    }

    void Model::DrawDebugGUI()
    {
        ImGui::Begin("Model Debug GUI");

        ImGui::Text("AABB Lower Bound x:%f y:%f z:%f",
                        this->global_aabb.lower_bound[0],
                        this->global_aabb.lower_bound[1],
                        this->global_aabb.lower_bound[2]);
        ImGui::Text("AABB Upper Bound x:%f y:%f z:%f",
                        this->global_aabb.upper_bound[0],
                        this->global_aabb.upper_bound[1],
                        this->global_aabb.upper_bound[2]);

        if(ImGui::CollapsingHeader("Global Model Transform"))
        {
            float position[3];
            float scale[3];
            float rotation[3];
            memcpy(position, this->global_transform.translation, 3 * sizeof(float));
            memcpy(scale, this->global_transform.scale, 3 * sizeof(float));
            memcpy(rotation, this->global_transform.rotation, 3 * sizeof(float));

            ImGui::SliderFloat3("Global Translation", position, -5, 5);
            ImGui::SliderFloat3("Global Scale", scale, 0.000001f, 10);
            ImGui::SliderFloat3("Global Rotation", rotation, 0, 2.14f);
            this->global_transform.SetTranslation(position[0], position[1], position[2]);
            this->global_transform.SetScale(scale[0], scale[1], scale[2]);
            this->global_transform.SetRotation(rotation[0], rotation[1], rotation[2]);

            if(ImGui::Button("Reset Transform"))
            {
                this->global_transform = {};
            }
        }

        for(auto& submesh: this->submeshes)
        {
            submesh.DrawDebugGUI();
        }

        ImGui::End();
    }

}
