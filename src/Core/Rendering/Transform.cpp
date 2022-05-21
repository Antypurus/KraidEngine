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

}
