#pragma once

#include <Core/Rendering/D3D12/D3D12.h>

enum class FillMode
{
    Solid = D3D12_FILL_MODE_SOLID,
    Wirefram = D3D12_FILL_MODE_WIREFRAME
};

enum class CullingMode
{
    None = D3D12_CULL_MODE_NONE,
    Backface = D3D12_CULL_MODE_BACK ,
    Frontface = D3D12_CULL_MODE_FRONT,
};

namespace Kraid
{

    class Rasterizer
    {

    public:
        inline D3D12_RASTERIZER_DESC CreateRasterizerDescritpion() const
        {
            D3D12_RASTERIZER_DESC ret = {};
            return ret;
        };
    };

}
