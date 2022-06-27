#pragma once

#include <DirectXMath.h>

namespace Kraid
{

    using namespace DirectX;

    class Transform
    {
    public:
        float translation[3] = {0,0,0};
        float scale[3] = {1,1,1};
        float rotation[3] = {0,0,0};

        Transform() = default;
        Transform(const Transform& other)
        {
            this->translation[0] = other.translation[0];
            this->translation[1] = other.translation[1];
            this->translation[2] = other.translation[2];
            
            this->rotation[0] = other.rotation[0];
            this->rotation[1] = other.rotation[1];
            this->rotation[2] = other.rotation[2];
            
            this->scale[0] = other.scale[0];
            this->scale[1] = other.scale[1];
            this->scale[2] = other.scale[2];
        }

        Transform(
                const float translation[3],
                const float scale[3],
                const float rotation[3])
        {
            this->translation[0] = translation[0];
            this->translation[1] = translation[1];
            this->translation[2] = translation[2];
            
            this->rotation[0] = rotation[0];
            this->rotation[1] = rotation[1];
            this->rotation[2] = rotation[2];
            
            this->scale[0] = scale[0];
            this->scale[1] = scale[1];
            this->scale[2] = scale[2];
        }

        XMMATRIX GetModelMatrix() const;
        Transform operator+(const Transform& other);
        Transform& operator=(const Transform& other)
        {
            this->translation[0] = other.translation[0];
            this->translation[1] = other.translation[1];
            this->translation[2] = other.translation[2];
            
            this->rotation[0] = other.rotation[0];
            this->rotation[1] = other.rotation[1];
            this->rotation[2] = other.rotation[2];
            
            this->scale[0] = other.scale[0];
            this->scale[1] = other.scale[1];
            this->scale[2] = other.scale[2];

            return *this;
        };
    };

}
