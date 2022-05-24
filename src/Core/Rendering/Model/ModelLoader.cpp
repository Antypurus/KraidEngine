#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include <Core/Utils/Log.h>

namespace Kraid
{

    std::pair<std::vector<float>,std::vector<uint32>> ModelLoader::LoadOBJModel(const StringView& filepath)
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

        std::vector<float> vertices = attribute.vertices;
        std::vector<uint32> indices;
        for (auto& shape : shapes)
        {
            uint64 index_offset = 0;
            for (uint64 i = 0; i < shape.mesh.num_face_vertices.size(); ++i)
            {
                switch(shape.mesh.num_face_vertices[i])
                {
                    case(1)://Point
                    {
                        float x = attribute.vertices[shape.mesh.indices[(index_offset)].vertex_index];
                        float y = attribute.vertices[shape.mesh.indices[(index_offset)].vertex_index + 1];
                        float z = attribute.vertices[shape.mesh.indices[(index_offset)].vertex_index + 2];
                        break;
                    }
                    case(2)://Line
                    {
                        for(int j = 0; j < 2; j++)
                        {
                            float x = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index];
                            float y = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index + 1];
                            float z = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index + 2];
                        }
                        break;
                    }
                    case(3)://Triangle
                    {
                        for(int j = 0; j < 3; j++)
                        {
                            float x = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index];
                            float y = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index + 1];
                            float z = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index + 2];
                            indices.push_back(shape.mesh.indices[(index_offset)+j].vertex_index);
                        }
                        break;
                    }
                    case(4)://Quad
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            float x = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index];
                            float y = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index + 1];
                            float z = attribute.vertices[shape.mesh.indices[(index_offset) + j].vertex_index + 2];
                        }
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
        }

        return std::make_pair(vertices,indices);
    }

}
