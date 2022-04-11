#include "Shader.h"

#include <combaseapi.h>
#include <d3dcompiler.h>
#include <dxcapi.h>
#include <vector>

#include <Core/Windows.h>

namespace Kraid
{

    namespace D3D12
    {

        DXCShaderCompiler DXCShaderCompiler::m_instance = {};
        DXCShaderCompiler::DXCShaderCompiler()
        {
            D3DCALL(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&this->utils)), "DXC Utils Created");
            D3DCALL(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&this->compiler)), "DXC Compiler Created");
            this->utils->CreateDefaultIncludeHandler(&this->include_handler);
        }

        std::string Shader::CreateTargetString(ShaderType type, ShaderModel sm)
        {
            #define VERSION_APPEND_SIZE 4
            char target[8];//NOTE(Tiago):8bytes (2/3 for the type) + _ + 1 for version major + _ + 1 for version minor + \0 
            uint8 type_size = 0;

            if(type == ShaderType::RayTracing)
            {
                type_size = 3;
            }
            else 
            {
                type_size = 2;
            }
    
            switch(type)
            {
                case(ShaderType::Compute):
                {
                    target[0] = 'c';
                    target[1] = 's';
                    break;
                }
                
                case(ShaderType::RayTracing):
                {
                    target[0] = 'l';
                    target[1] = 'i';
                    target[2] = 'b';
                    break;
                }
                
                case(ShaderType::Pixel):
                {
                    target[0] = 'p';
                    target[1] = 's';
                    break;
                }
                
                case(ShaderType::Vertex):
                {
                    target[0] = 'v';
                    target[1] = 's';
                    break;
                }

                case(ShaderType::Geometry):
                {
                    target[0] = 'g';
                    target[1] = 's';
                    break;
                }

                case(ShaderType::Domain):
                {
                    target[0] = 'd';
                    target[1] = 's';
                    break;
                }
                
                case(ShaderType::Hull):
                {
                    target[0] = 'h';
                    target[1] = 's';
                    break;
                }

                default:break;
            }

            target[type_size] = '_';

            switch(sm)
            {
                case(ShaderModel::SM5_0):
                {
                    target[type_size + 1] = '5';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '0';
                    break;
                }

                case(ShaderModel::SM6_0):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '0';
                    break;
                }

                case(ShaderModel::SM6_1):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '1';
                    break;
                }

                case(ShaderModel::SM6_2):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '2';
                    break;
                }
    
                case(ShaderModel::SM6_3):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '3';
                    break;
                }

                case(ShaderModel::SM6_4):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '4';
                    break;
                }

                case(ShaderModel::SM6_5):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '5';
                    break;
                }

                case(ShaderModel::SM6_6):
                {
                    target[type_size + 1] = '6';
                    target[type_size + 2] = '_';
                    target[type_size + 3] = '6';
                    break;
                }
                
                default:break;
            }

            target[type_size + VERSION_APPEND_SIZE] = 0;

            return target;
        }

        Shader::Shader(const wchar_t* filepath, ShaderType type, ShaderModel sm, const char* entrypoint)
        {
            this->target = CreateTargetString(type, sm);
            if(sm == ShaderModel::SM5_0)
            {
                FXCCompile(filepath, this->target.c_str(), entrypoint); 
            }
            else
            {
                DXCCompile(filepath, this->target.c_str(), entrypoint);
            }
        }

        void Shader::FXCCompile(const wchar_t* filepath, const char* target, const char* entrypoint)
        {
            this->shader_file = File(filepath, [this]() {
                    LINFO("Shader changes detected, recompiling...");
                    LWARNING("Shader recompilatin functionality not yet implemented, shader bytecode not actually changed");
                    
                    Buffer shader_code = this->shader_file.Read();
                }, true);

            Buffer shader_code = shader_file.Read();
            ComPtr<ID3DBlob> error_message = nullptr;

            D3DCALL(D3DCompile2(
                    shader_code.data,
                    shader_code.size,
                    nullptr,
                    nullptr,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    entrypoint,
                    target,
                    D3DCOMPILE_OPTIMIZATION_LEVEL0,
                    0,
                    0,
                    nullptr,
                    0,
                    this->shader_bytecode.GetAddressOf(),
                    error_message.GetAddressOf()
                ), "Shader has finished compiling");

            if (error_message != nullptr)
            {
                LERROR((char*)error_message->GetBufferPointer());
            }
            return;
        }


        //TODO(Tiago): I need to once again figure out ASCII -> UNICODE conversion routines that I can use for these cases, or figure out what the incompatibilities between the two compilers are in these regards
        void Shader::DXCCompile(const wchar_t* filepath, const char* target, const char* entrypoint)
        {
            this->shader_file = File(filepath, [this]() {
                    LINFO("Shader changed detected, recompiling...");
                    LWARNING("Shader recompilation functionality not yet implemented, shader bytecode not actually changed");

                    Buffer shader_code = this->shader_file.Read();
                }, true);

            std::vector<wchar_t*> arguments;
            arguments.push_back((wchar_t*)filepath);
            //entrypoint
            arguments.push_back((wchar_t*)L"-E");
            arguments.push_back((wchar_t*)L"main");
            //target
            arguments.push_back((wchar_t*)L"-T");
            arguments.push_back((wchar_t*)L"ps_6_0");

            Buffer shader_code = shader_file.Read();
            DxcBuffer source;
            source.Ptr = shader_code.data;
            source.Size = shader_code.size;
            source.Encoding = DXC_CP_ACP;

            ComPtr<IDxcResult> results;
            DXCShaderCompiler::GetShaderCompiler().compiler->Compile(
                    &source,
                    (const wchar_t**)arguments.data(),
                    arguments.size(),
                    DXCShaderCompiler::GetShaderCompiler().include_handler.Get(),
                    IID_PPV_ARGS(&results));

            ComPtr<IDxcBlobUtf8> errors = nullptr;
            results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
            if(errors != nullptr && errors->GetStringLength() != 0)
            {
                LERROR("%s", errors->GetStringPointer());
            }

            return;
        }

    }

}
