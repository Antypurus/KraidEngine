#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include <parallel_hashmap/phmap.h>

#include <Core/Utils/Log.h>
#include <Core/Utils/StringHelpers.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>

namespace Kraid
{

    Model ModelLoader::LoadOBJModel(
        GPUDevice& device,
        GraphicsCommandList& command_list,
        const StringView& filepath)
    {
        tinyobj::attrib_t attribute;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warnings;
        std::string errors;

        std::string basedir = GetBasedir(filepath.Get());
        bool result = tinyobj::LoadObj(&attribute, &shapes, &materials, &warnings, &errors, filepath.Get(), basedir.c_str());
        if(result == false)
        {
            LERROR("Load failed");
            return Model();
        }

        LINFO("tinyobj vertex size: %d bytes", attribute.vertices.capacity() * sizeof(tinyobj::real_t));

        std::vector<BlinPhongMaterial> mats;
        for(auto& material: materials)
        {
            std::string texture_path = material.diffuse_texname;
            XMFLOAT3 diffuse_coeficient = XMFLOAT3(material.diffuse);
            XMFLOAT3 specular_coeficient = XMFLOAT3(material.specular);
            XMFLOAT3 ambient_coeficient = XMFLOAT3(material.ambient);

            if(!texture_path.empty())
            {
                mats.emplace_back(device, command_list, basedir + "/" + texture_path, diffuse_coeficient, specular_coeficient, ambient_coeficient);
            }
            else
            {
                mats.emplace_back(diffuse_coeficient, specular_coeficient, ambient_coeficient);
            }
        }

        std::vector<Vertex> vertices;
        std::vector<Submesh> submeshes;
        phmap::flat_hash_map<Vertex, uint64> vertex_index_map;
        std::unordered_map<uint64, std::vector<uint32>> submesh_indices;//NOTE(Tiago):material id -> indices, submeshes are defined by their material
        for (auto& shape : shapes)
        {
            uint64 index_offset = 0;
            for(uint64 i = 0; i < shape.mesh.num_face_vertices.size(); i++)
            {
                uint8 vertices_in_face =  shape.mesh.num_face_vertices[i];
                for(uint8 j = 0; j < vertices_in_face; ++j)
                {
                    uint64 vertex_index = shape.mesh.indices[index_offset + j].vertex_index;
                    uint64 normal_index = shape.mesh.indices[index_offset + j].normal_index;
                    uint64 uv_index = shape.mesh.indices[index_offset + j].texcoord_index;
                    uint64 material_id = shape.mesh.material_ids[i];
            
                    Vertex vert;
                    vert.position = XMFLOAT3(
                            attribute.vertices[vertex_index * 3],
                            attribute.vertices[vertex_index * 3 + 1],
                            attribute.vertices[vertex_index * 3 + 2]);
                    vert.color = XMFLOAT3(
                            attribute.colors[vertex_index * 3],
                            attribute.colors[vertex_index * 3 + 1],
                            attribute.colors[vertex_index * 3 + 1]);
                    vert.normal = XMFLOAT3(
                            attribute.normals[normal_index * 3],
                            attribute.normals[normal_index * 3 + 1],
                            attribute.normals[normal_index * 3 + 2]);
                    vert.texture_coordinates = XMFLOAT2(
                            attribute.texcoords[uv_index * 2],
                            1.0f - attribute.texcoords[uv_index * 2 + 1]);

                    if(vertex_index_map.contains(vert))
                    {
                        uint64 index = vertex_index_map[vert];
                        submesh_indices[material_id].push_back(index);
                    }
                    else
                    {
                        vertices.push_back(vert);
                        submesh_indices[material_id].push_back(vertices.size() - 1);
                        vertex_index_map[vert] = vertices.size() - 1;
                    }

                }
                index_offset += vertices_in_face;
            }
        }

        for(auto& submesh: submesh_indices)
        {
            uint64 material_id = submesh.first;
            std::string normal_map_path = materials[material_id].normal_texname;
            if(!normal_map_path.empty())
            {
                normal_map_path = basedir + "/" + normal_map_path;
            }
            submeshes.emplace_back(device, command_list, submesh.second, &mats[material_id], Transform(), normal_map_path);
        }

        return Model(device, command_list, submeshes, vertices, std::move(mats));
    }

}
