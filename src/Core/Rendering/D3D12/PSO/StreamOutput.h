#pragma once

#include <Core/Rendering/D3D12/D3D12.h>

namespace Kraid
{

namespace D3D12
{

    //TODO(Tiago):streaming output description needs a proper design , for now we will just use it as placeholder

    class StreamingOutputBuffer
    {
    public:
        D3D12_STREAM_OUTPUT_DESC so_description = {};

        StreamingOutputBuffer() = default;
    };

}

}
