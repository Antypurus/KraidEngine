#pragma once

#include <tuple>

namespace Kraid
{

    class AABB
    {
    public:
        float lower_bound[3];
        float upper_bound[3];
    public:
        AABB();
        void UpdateBounds(float x, float y, float z);
        float ComputeUnitScaleTransform() const;
        std::tuple<float,float,float> ComputeUnitCenterPoint() const;
    };

}
