#include "ModelLoader.h"
#include <limits.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include <unordered_map>
#include <Core/Utils/Log.h>
#include <Core/Utils/StringHelpers.h>

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
        std::unordered_map<UINT, UINT> index_map;
        std::vector<Submesh> meshes;
        for (auto& shape : shapes)
        {
            std::vector<uint32> indices;
            uint32 mat_index = 0xFFFFFFFF;
            if(shape.mesh.material_ids.size() > 0)
            {
                mat_index = shape.mesh.material_ids[0];
            }
            for(uint64 i = 0; i < shape.mesh.indices.size(); ++i)
            {
                if(index_map.contains(shape.mesh.indices[i].vertex_index))
                {
                    //index is already present in the vertex array, just take its index
                    uint32 index = index_map[shape.mesh.indices[i].vertex_index];
                    indices.push_back(index);
                }
                else
                {
                    //index is not present in the vertex array, create new vertex and add it
                    uint64 position_index = shape.mesh.indices[i].vertex_index;
                    uint64 normal_index = shape.mesh.indices[i].normal_index;
                    uint64 uv_index = shape.mesh.indices[i].texcoord_index;

                    Vertex vert;
                    vert.position = XMFLOAT3(
                                attribute.vertices[position_index * 3],
                                attribute.vertices[position_index * 3 + 1],
                                attribute.vertices[position_index * 3 + 2]);
                    vert.color = XMFLOAT3(
                            attribute.colors[position_index * 3],
                            attribute.colors[position_index * 3 + 1],
                            attribute.colors[position_index * 3 + 1]);
                    vert.normal = XMFLOAT3(
                            attribute.normals[normal_index * 3],
                            attribute.normals[normal_index * 3 + 1],
                            attribute.normals[normal_index * 3 + 2]);
                    vert.texture_coordinates = XMFLOAT2(
                            attribute.texcoords[uv_index * 2],
                            1.0f - attribute.texcoords[uv_index * 2 + 1]);

                    vertices.push_back(vert);
                    index_map[shape.mesh.indices[i].vertex_index] = vertices.size() - 1;
                    indices.push_back(vertices.size() - 1);
                }
            }
            BlinPhongMaterial mat;
            std::string normal_map_path = "";
            if(mat_index != UINT_MAX)
            {
                mat = mats[mat_index];
                normal_map_path = materials[mat_index].normal_texname;
            }
            if(!normal_map_path.empty())
            {
                normal_map_path = basedir + "/" + normal_map_path;
            }
            meshes.emplace_back(device, command_list, indices, mat, Transform(), normal_map_path);
        }

        return Model(device, command_list, meshes, vertices);
    }

}
