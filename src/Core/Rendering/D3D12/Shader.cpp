#include "Shader.h"

#include <d3dcompiler.h>

namespace Kraid
{

    namespace D3D12
    {

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

        void Shader::FXCCompile(const wchar_t* filepath, const char* target, const char* entrypoint)
        {

        }

    }

}
