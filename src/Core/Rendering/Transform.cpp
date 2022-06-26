#include "Transform.h"
#include <DirectXMath.h>

namespace Kraid
{

    XMMATRIX Transform::GetModelMatrix() const
    {
        XMMATRIX ret = XMMatrixIdentity();

        FXMVECTOR x_axis = {1,0,0};
        FXMVECTOR y_axis = {0,1,0};
        FXMVECTOR z_axis = {0,0,1};
        ret *= XMMatrixRotationAxis(x_axis, this->rotation.x); 
        ret *= XMMatrixRotationAxis(y_axis, this->rotation.y); 
        ret *= XMMatrixRotationAxis(z_axis, this->rotation.z);

        ret *= XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);

        ret *= XMMatrixTranslation(this->translation.x, this->translation.y, this->translation.z);

        return ret;
    }

    Transform Transform::operator+(const Transform& other)
    {
        Transform ret = *this;

        ret.translation.x += other.translation.x;
        ret.translation.y += other.translation.y;
        ret.translation.z += other.translation.z;

        ret.scale.x += other.scale.x;
        ret.scale.y += other.scale.y;
        ret.scale.z += other.scale.z;

        ret.rotation.x += other.rotation.x;
        ret.rotation.y += other.rotation.y;
        ret.rotation.z += other.rotation.z;

        return ret;
    }

}
