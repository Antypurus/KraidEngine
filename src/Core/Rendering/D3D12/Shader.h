#pragma once

#include <Core/Rendering/D3D12/D3D12.h>

namespace Kraid
{

    namespace D3D12
    {

        enum ShaderType
        {

        };

        using namespace Microsoft::WRL;

        struct Shader
        {
            ComPtr<ID3DBlob> shader_bytecode = nullptr;

            Shader(const wchar_t* filepath, const char* entrypoint = "main");
        };

    }

}
