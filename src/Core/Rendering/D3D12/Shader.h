#pragma once

#include <Core/types.h>
#include <Core/Filesystem/Filesystem.h>
#include <Core/Rendering/D3D12/D3D12.h>
#include <Core/DataStructures/StringView.h>

#include <string>
#include <dxcapi.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <unordered_map>

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

        enum class ShaderCompileFlags
        {
            Debug                           = D3DCOMPILE_DEBUG,
            SkipValidation                  = D3DCOMPILE_SKIP_VALIDATION,
            SkipOptimization                = D3DCOMPILE_SKIP_OPTIMIZATION,
            PackMatrixRowMajor              = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
            PackMatrixColumnMajor           = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR,
            PartialPrecision                = D3DCOMPILE_PARTIAL_PRECISION,
            AvoidFlowControl                = D3DCOMPILE_AVOID_FLOW_CONTROL,
            EnableStrictness                = D3DCOMPILE_ENABLE_STRICTNESS,
            IEEEStrictness                  = D3DCOMPILE_IEEE_STRICTNESS,
            EnableBackwardsCompatibility    = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY,
            O0                              = D3DCOMPILE_OPTIMIZATION_LEVEL0,
            O1                              = D3DCOMPILE_OPTIMIZATION_LEVEL1,
            O2                              = D3DCOMPILE_OPTIMIZATION_LEVEL2,
            O3                              = D3DCOMPILE_OPTIMIZATION_LEVEL3,
            WarningsAreErrors               = D3DCOMPILE_WARNINGS_ARE_ERRORS,
            ResourcesMayAlias               = D3DCOMPILE_RESOURCES_MAY_ALIAS,
            EnableUnboundedDescriptorTables = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES,
            AllResourcesBound               = D3DCOMPILE_ALL_RESOURCES_BOUND,
            DebugNameForSource              = D3DCOMPILE_DEBUG_NAME_FOR_SOURCE,
            DebugNameForBinary              = D3DCOMPILE_DEBUG_NAME_FOR_BINARY
        };

        using namespace Microsoft::WRL;

        class DXCShaderCompiler
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

        class ShaderMacro
        {
        public:
            std::string name;
            std::string value;

            ShaderMacro(const std::string& name, const std::string& value);
            ShaderMacro(const std::string&& name, const std::string&& value);

            std::wstring DXCMacroFormat() const;
        };

        class Shader
        {
        public:
            ComPtr<ID3DBlob> shader_bytecode = nullptr;
            File shader_file;               //NOTE(Tiago):needs to be stored for hot-recompilation
            std::wstring shader_name = L"";
            std::string target = "";        //NOTE(Tiago):needs to be stored for hot-recompilation
            std::string entrypoint = "";    //NOTE(Tiago):needs to be stored for hot-recompilation
            ShaderModel shader_model;       //TODO(Tiago):needs to be stored for hot-recompilation
            std::vector<ShaderMacro> shader_defines; //NOTE(Tiago):needs to be stored for hot-recompilation

            Shader(
                    const WideStringView& filepath,
                    ShaderType type,
                    ShaderModel sm,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L""
                );

        private:
            void Recompile();
            void Compile(const Buffer& shader_source);
            void FXCCompile(const Buffer& shader_source);
            void DXCCompile(const Buffer& shader_source);
            static std::string CreateTargetString(ShaderType type, ShaderModel sm);
            
        };

        class PixelShader: public Shader
        {
            PixelShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::Pixel, sm, defines, entrypoint, name){};
        };

        class VertexShader: public Shader
        {
            VertexShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::Vertex, sm, defines, entrypoint, name){};
        };

        class GeometryShader: public Shader
        {
            GeometryShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::Geometry, sm, defines, entrypoint, name){};
        };

        class ComputeShader: public Shader
        {
            ComputeShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::Compute, sm, defines, entrypoint, name){};
        };

        class DomainShader: public Shader
        {
            DomainShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::Domain, sm, defines, entrypoint, name){};
        };

        class HullShader: public Shader
        {
            HullShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::Hull, sm, defines, entrypoint, name){};
        };

        class RayTracingShader: public Shader
        {
            RayTracingShader(
                    const WideStringView& filepath,
                    const std::vector<ShaderMacro>& defines = {},
                    const StringView& entrypoint = "main",
                    const std::wstring& name = L"",
                    ShaderModel sm = SM6_0
                ):Shader(filepath, ShaderType::RayTracing, sm, defines, entrypoint, name){};
        };

    }

}
