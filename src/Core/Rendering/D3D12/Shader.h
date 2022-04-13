#pragma once

#include <Core/types.h>
#include <Core/Filesystem/Filesystem.h>
#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/DataStructures/StringView.h>

#include <string>
#include <dxcapi.h>
#include <d3d12shader.h>

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

        struct DXCShaderCompiler
        {
        public:
            ComPtr<IDxcUtils> utils;
            ComPtr<IDxcCompiler3> compiler;
            ComPtr<IDxcIncludeHandler> include_handler;
        private:
            static DXCShaderCompiler m_instance;

        public:
            static DXCShaderCompiler& GetShaderCompiler() { return m_instance; };
        private:
            DXCShaderCompiler();
        };

        struct Shader
        {
            File shader_file;
            ComPtr<ID3DBlob> shader_bytecode = nullptr;
            std::string target = "";

            Shader(const WideStringView& filepath, ShaderType type, ShaderModel sm, const StringView& entrypoint = "main");

        private:
            void FXCCompile(const WideStringView& filepath, const StringView& target, const StringView& entrypoint);
            void DXCCompile(const WideStringView& filepath, const StringView& target, const StringView& entrypoint);
            static std::string CreateTargetString(ShaderType type, ShaderModel sm);
            
        };

    }

}
