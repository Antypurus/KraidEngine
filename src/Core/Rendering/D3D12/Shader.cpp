#include "Shader.h"

#include <combaseapi.h>
#include <dxcapi.h>
#include <vector>

#include <Core/Windows.h>
#include <Core/Utils/StringHelpers.h>

namespace Kraid
{

    namespace D3D12
    {

        const std::unordered_map<ShaderCompileFlags, const wchar_t*> DXCCompilerFlags =
        {
            {ShaderCompileFlags::Debug, L"-Zi -Qembed_debug"},
            {ShaderCompileFlags::SkipValidation, L"-Vd"},
            {ShaderCompileFlags::SkipOptimization, L"-Od"},
            {ShaderCompileFlags::PackMatrixRowMajor, L"-Zpr"},
            {ShaderCompileFlags::PackMatrixColumnMajor, L"-Zpc"},
            {ShaderCompileFlags::PartialPrecision, L"-enable-16bit-types"},
            {ShaderCompileFlags::AvoidFlowControl, L"-Gfa"},
            {ShaderCompileFlags::EnableStrictness, L"-Ges"},
            {ShaderCompileFlags::IEEEStrictness, L"-Gis"},
            {ShaderCompileFlags::EnableBackwardsCompatibility, L"-GeC"},
            {ShaderCompileFlags::O0, L"-O0"},
            {ShaderCompileFlags::O1, L"-O1"},
            {ShaderCompileFlags::O2, L"-O2"},
            {ShaderCompileFlags::O3, L"-O3"},
            {ShaderCompileFlags::WarningsAreErrors, L"-WX"},
            {ShaderCompileFlags::ResourcesMayAlias, L"-res_may_alias"}, 
            {ShaderCompileFlags::EnableUnboundedDescriptorTables, L""},
            {ShaderCompileFlags::AllResourcesBound, L"-all_resources_bound"},
            {ShaderCompileFlags::DebugNameForSource, L"-Zss"},
            {ShaderCompileFlags::DebugNameForBinary, L"-Zsb"}
        };

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

        ShaderMacro::ShaderMacro(const std::string& name, const std::string& value):name(name), value(value)
        {
        }

        ShaderMacro::ShaderMacro(const std::string&& name, const std::string&& value):name(std::move(name)), value(std::move(value))
        {
        }

        std::wstring ShaderMacro::DXCMacroFormat() const
        {
            std::wstring ret = L"";
            
            wchar_t* wname = to_unicode(this->name.data(), this->name.length());
            wchar_t* wvalue = to_unicode(this->value.data(), this->value.length());

            ret = std::wstring(wname) + L"=" + std::wstring(wvalue);

            free(wname);
            free(wvalue);

            return ret;
        }

        Shader::Shader(
                const WideStringView& filepath,
                ShaderType type,
                ShaderModel sm,
                const StringView& entrypoint,
                const std::vector<ShaderCompileFlags>& flags,
                const std::vector<ShaderMacro>& defines,
                const std::wstring& name)
        {
            this->shader_model = sm;
            this->entrypoint = entrypoint.Get();
            this->target = CreateTargetString(type, sm);
            this->compilation_flags = flags;
            this->shader_defines = defines;

        #ifndef NDEBUG
            this->compilation_flags.push_back(ShaderCompileFlags::Debug);
            this->compilation_flags.push_back(ShaderCompileFlags::O0);
            this->shader_defines.push_back({"DEBUG", "1"});
        #else
            this->compilation_flags.push_back(ShaderCompileFlags::O3);
            this->shader_defines.push_back({"DEBUG", "0"});
        #endif

            if(name.empty())
            {
                this->shader_name = filepath.Get();
            }
            else
            {
                this->shader_name = name;
            }

            this->shader_file = File(filepath.Get(), [this]()
                {
                    LINFO("Shader source code change detected, recompiling shader...");
                    this->Recompile();
                }, true);
            Buffer shader_source = this->shader_file.Read();
            this->Compile(shader_source);
        }

        void Shader::Compile(const Buffer& shader_source)
        {
            if(this->shader_model == ShaderModel::SM5_0)
            {
                FXCCompile(shader_source); 
            }
            else
            {
                DXCCompile(shader_source);
            }
        }

        void Shader::Recompile()
        {
            Buffer shader_code = this->shader_file.Read();
            this->Compile(shader_code);
        }

        void Shader::FXCCompile(const Buffer& shader_source)
        {
            ComPtr<ID3DBlob> error_message = nullptr;

            std::vector<D3D_SHADER_MACRO> macros = std::vector<D3D_SHADER_MACRO>(this->shader_defines.size());
            for(auto& define: this->shader_defines)
            {
                D3D_SHADER_MACRO macro = {};
                macro.Name = define.name.data();
                macro.Definition = define.value.data();
                macros.push_back(macro);
            }
            D3D_SHADER_MACRO end_macro = {};
            end_macro.Name = nullptr;
            end_macro.Definition = nullptr;
            macros.push_back(end_macro);

            //compilation flags
            uint32 compilation_flags = 0;
            for(auto flag: this->compilation_flags)
            {
                compilation_flags |= (uint32)flag;
            }

            D3DCALL(D3DCompile2(
                    shader_source.data,
                    shader_source.size,
                    nullptr,
                    macros.data(),
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    this->entrypoint.data(),
                    this->target.data(),
                    compilation_flags,
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

        void Shader::DXCCompile(const Buffer& shader_source)
        {
            std::vector<std::wstring> define_strings = std::vector<std::wstring>(this->shader_defines.size());
            std::vector<wchar_t*> arguments;

            arguments.push_back((wchar_t*)this->shader_name.data());
            //entrypoint
            wchar_t* entrypoint_w = to_unicode(this->entrypoint.data());
            arguments.push_back((wchar_t*)L"-E");
            arguments.push_back((wchar_t*)entrypoint_w);
            //target
            wchar_t* target_w = to_unicode(this->target.data());
            arguments.push_back((wchar_t*)L"-T");
            arguments.push_back((wchar_t*)target_w);
            //defines
            for(auto& define:this->shader_defines)
            {
                define_strings.push_back(std::move(define.DXCMacroFormat()));
                arguments.push_back((wchar_t*)L"-D");
                arguments.push_back((wchar_t*)define_strings[define_strings.size() - 1].data());
            }
            //compile defines
            for(auto& flag:this->compilation_flags)
            {
                arguments.push_back((wchar_t*)DXCCompilerFlags.at(flag));
            }

            Buffer shader_code = shader_file.Read();
            DxcBuffer source;
            source.Ptr = shader_source.data;
            source.Size = shader_source.size;
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

            free(entrypoint_w);
            free(target_w);
            return;
        }

    }

}
