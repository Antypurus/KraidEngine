#pragma once

#include <Core/types.h>
#include <Core/Filesystem/Filesystem.h>
#include <Core/Rendering/D3D12/D3D12.h>
#include <string>

namespace Kraid
{

    namespace D3D12
    {

        enum ShaderType
        {
            Compute,
            RayTracing,
            Pixel,
            Vertex,
            Geometry,
            Domain,
            Hull
        };

        enum ShaderModel
        {
            SM5_0,
            SM6_0,
            SM6_1,
            SM6_2,
            SM6_3,
            SM6_4,
            SM6_5,
            SM6_6
        };

        using namespace Microsoft::WRL;

        struct Shader
        {
            ComPtr<ID3DBlob> shader_bytecode = nullptr;
            std::string target = "";

            Shader(const wchar_t* filepath, ShaderType type, ShaderModel sm, const char* entrypoint = "main");

        private:
            void FXCCompile(const wchar_t* filepath, const char* target, const char* entrypoint);
            void DXCCompile(const wchar_t* filepath, const char* target, const char* entrypoint);
            static std::string CreateTargetString(ShaderType type, ShaderModel sm);
            
        };

    }

}
