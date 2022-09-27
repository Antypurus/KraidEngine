#include "AABB.h"

namespace Kraid
{

    AABB::AABB()
    {
        //TODO(Tiago):initialize the lower bound to the highest possible float value, such that any initial point will be smaller than it
        this->lower_bound[0] = FLT_MAX;
        this->lower_bound[1] = FLT_MAX;
        this->lower_bound[2] = FLT_MAX;
        //TODO(Tiago):initialize the upper bound to the smaller possible float vlaue, such that any initial point will be larger than it
        this->upper_bound[0] = -FLT_MAX;
        this->upper_bound[1] = -FLT_MAX;
        this->upper_bound[2] = -FLT_MAX;
    }

    void AABB::UpdateBounds(float x, float y, float z)
    {
        if(x < this->lower_bound[0]) lower_bound[0] = x;
        if(y < this->lower_bound[1]) lower_bound[1] = y;
        if(z < this->lower_bound[2]) lower_bound[2] = z;
        
        if(x > this->upper_bound[0]) upper_bound[0] = x;
        if(y > this->upper_bound[1]) upper_bound[1] = y;
        if(z > this->upper_bound[2]) upper_bound[2] = z;
    }

}
