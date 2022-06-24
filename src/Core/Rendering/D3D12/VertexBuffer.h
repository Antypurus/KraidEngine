#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/stdcpp.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Utils/Log.h>
#include <Core/Windows.h>

namespace Kraid
{
    namespace D3D12
    {

        using namespace DirectX;

        enum class PrimitiveTopology
        {
            Undefined   = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
            Point       = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
            Line        = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
            Triangle    = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            Patch       = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
        };

        //NOTE(Tiago): I really really dont want to have to use virtual fuctions, as such, I would rather have to create a struct for each type of vertex
        //than use some inheritance thing and a virtual function to generate the input layout for d3d12. Its going to be a bit of extra work but reallistically
        //there is only so many sensible permutation of a vertex layout.
        struct Vertex
        {
            XMFLOAT3 position;
            XMFLOAT3 normal;
            XMFLOAT3 color;
            XMFLOAT2 texture_coordinates;

            Vertex(const Vertex& other)
            {
                this->position = other.position;
                this->normal = other.normal;
                this->color = other.color;
                this->texture_coordinates = other.texture_coordinates;
            }
            Vertex(
                XMFLOAT3 position = {0,0,0},
                XMFLOAT3 normal = {0,0,0},
                XMFLOAT3 color = {0,0,0},
                XMFLOAT2 texture_coordinates = {0,0}
            ):position(position), normal(normal), color(color), texture_coordinates(texture_coordinates) {};
            static D3D12_INPUT_LAYOUT_DESC GenerateVertexDescription();

            bool operator==(const Vertex& other) const
            {
                { 
                    XMVECTOR original = {position.x,position.y,position.z,1.0f};
                    XMVECTOR other_vec = {other.position.x,other.position.y,other.position.z,1.0f};
                    if(!XMVector3Equal(original,other_vec))
                    {
                        return false;
                    }
		        }
                { 
                    XMVECTOR original = {normal.x,normal.y,normal.z,1.0f};
                    XMVECTOR other_vec = {other.normal.x,other.normal.y,other.normal.z,1.0f};
                    if(!XMVector3Equal(original,other_vec))
                    {
                        return false;
                    }
		        }

		        {
                    XMVECTOR original = { texture_coordinates.x,texture_coordinates.y,1.0f,1.0f };
                    XMVECTOR other_vec = { other.texture_coordinates.x,other.texture_coordinates.y,1.0f,1.0f };
                    if (!XMVector3Equal(original, other_vec))
                    {
                        return false;
                    }
                }
                return true;
            }
        };

        template<typename T>
        struct VertexBuffer
        {
            BufferResource vertex_buffer;
            UploadBufferResource upload_buffer;
            PrimitiveTopology topology_type = PrimitiveTopology::Triangle;

            VertexBuffer() = default;

            VertexBuffer(GPUDevice& device,
                        GraphicsCommandList& command_list,
                        const std::vector<T>& vertices,
                        PrimitiveTopology topology_type = PrimitiveTopology::Triangle)
            {
                this->topology_type = topology_type;

                this->upload_buffer = UploadBufferResource(device, vertices.size() * sizeof(T));
                this->upload_buffer->SetName(L"Vertex Upload Buffer");
                this->vertex_buffer = BufferResource(device, vertices.size() * sizeof(T), ResourceState::CopyDestination);
                this->vertex_buffer->SetName(L"Vertex Buffer");

                this->upload_buffer.SetBufferData(vertices);
                this->upload_buffer.CopyTo(vertex_buffer, command_list);

                this->vertex_buffer.TransitionStateTo(ResourceState::GenericRead, command_list);

                //NOTE(Tiago): the upload buffer needs to be kept alive until the copy is executed, a fence can be used for this
            }

            D3D12_VERTEX_BUFFER_VIEW inline GenerateVertexBufferViewDescription()
            {
                D3D12_VERTEX_BUFFER_VIEW vbuffer_view = {};
                vbuffer_view.BufferLocation = this->vertex_buffer.resource->GetGPUVirtualAddress();
                vbuffer_view.SizeInBytes = this->vertex_buffer.width;
                vbuffer_view.StrideInBytes = sizeof(T);

                return vbuffer_view;
            }

            void Bind(GraphicsCommandList& command_list, uint32 slot=0)
            {
                D3D12_VERTEX_BUFFER_VIEW vbuffer_description = this->GenerateVertexBufferViewDescription();
                command_list->IASetVertexBuffers(slot, 1, &vbuffer_description);
            }
        };

    }

}

static inline void hash_combine(size_t& seed, size_t value)
{
	value += 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= value;
}

namespace std
{
    template<> struct hash<Kraid::D3D12::Vertex>
    {
        size_t operator()(const Kraid::D3D12::Vertex& vertex) const
        {
            size_t seed = 0;
			hash_combine(seed,*((uint32*)&vertex.position.x));
			hash_combine(seed,*((uint32*)&vertex.position.y));
			hash_combine(seed,*((uint32*)&vertex.position.z));
            
            hash_combine(seed,*((uint32*)&vertex.normal.x));
			hash_combine(seed,*((uint32*)&vertex.normal.y));
			hash_combine(seed,*((uint32*)&vertex.normal.z));
            
            hash_combine(seed,*((uint32*)&vertex.texture_coordinates.x));
			hash_combine(seed,*((uint32*)&vertex.texture_coordinates.y));

			return seed;
        }
    };
}
