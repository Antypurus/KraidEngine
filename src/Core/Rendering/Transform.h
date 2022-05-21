#pragma once

#include <DirectXMath.h>

namespace Kraid
{

    using namespace DirectX;

    class Transform
    {
    public:
        XMFLOAT3 translation = XMFLOAT3(0,0,0);
        XMFLOAT3 scale = XMFLOAT3(1,1,1);
        XMFLOAT3 rotation = XMFLOAT3(0,0,0);

        Transform() = default;
        Transform(
                XMFLOAT3 translation,
                XMFLOAT3 scale,
                XMFLOAT3 rotation)
            :translation(translation),scale(scale),rotation(rotation){};

        XMMATRIX GetModelMatrix() const;
    };

}
