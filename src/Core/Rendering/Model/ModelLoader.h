#pragma once

#include <Core/DataStructures/StringView.h>
#include <Core/types.h>
#include <tuple>

namespace Kraid
{

    class ModelLoader
    {
    public:
        static std::pair<std::vector<float>,std::vector<uint32>> LoadOBJModel(const StringView& filepath);
    };

}
