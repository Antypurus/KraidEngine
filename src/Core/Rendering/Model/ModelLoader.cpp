#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include <Core/Utils/Log.h>

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

        bool result = tinyobj::LoadObj(&attribute, &shapes, &materials, &warnings, &errors, filepath.Get());
        if(result == false)
        {
            LERROR("Load failed");
        }

        std::vector<Vertex> vertices;
        for(uint64 i = 0; i < (attribute.vertices.size()/3); i++)
        {
            Vertex vert = {};
            vert.position = XMFLOAT3(
                                attribute.vertices[i * 3],
                                attribute.vertices[i * 3 + 1],
                                attribute.vertices[i * 3 + 2]);
            vert.color = XMFLOAT3(
                                attribute.colors[i * 3],
                                attribute.colors[i * 3 + 1],
                                attribute.colors[i * 3 + 2]);
            //vert.normal = XMFLOAT3(
            //                    attribute.normals[i * 3],
            //                    attribute.normals[i * 3 + 1],
            //                    attribute.normals[i * 3 + 2]);
            vert.texture_coordinates = XMFLOAT2(
                                            attribute.texcoords[i * 2],
                                            attribute.texcoords[i * 2 + 1]);
            vertices.push_back(vert);
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
                mats.emplace_back(device, command_list, texture_path, diffuse_coeficient, specular_coeficient, ambient_coeficient);
            }
            else
            {
                mats.emplace_back(diffuse_coeficient, specular_coeficient, ambient_coeficient);
            }
        }

        std::vector<Submesh> meshes;
        for (auto& shape : shapes)
        {
            std::vector<uint32> indices;
            uint64 index_offset = 0;
            uint64 mat_index = 0;
            for (uint64 i = 0; i < shape.mesh.num_face_vertices.size(); ++i)
            {
                switch(shape.mesh.num_face_vertices[i])
                {
                    case(1)://Point
                    {
                        break;
                    }
                    case(2)://Line
                    {
                        break;
                    }
                    case(3)://Triangle
                    {
                        break;
                    }
                    case(4)://Quad
                    {
                        break;
                    }
                    default://N-Gon
                    {
                        LERROR("N-gons are not supported currentlyl");
                        break;
                    }
                }
                index_offset += shape.mesh.num_face_vertices[i];
            }
            for (uint64 j = 0; j < shape.mesh.indices.size(); ++j)
            {
                indices.push_back(shape.mesh.indices[j].vertex_index);
            }
            meshes.emplace_back(device, command_list, indices, BlinPhongMaterial(), Transform());
        }

        return Model(device, command_list, meshes, vertices);
    }

}
