#include "Core/Rendering/D3D12/RootSignature.h"
#include <windows.h>
#include <iostream>

#include <Core/Core.h>
#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/Fence.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Rendering/D3D12/Swapchain.h>
#include <Core/Rendering/D3D12/Resource/DescriptorHeap.h>
#include <Core/Rendering/D3D12/Resource/Resource.h>
#include <Core/Rendering/D3D12/VertexBuffer.h>
#include <Core/Rendering/D3D12/IndexBuffer.h>
#include <Core/Rendering/D3D12/Shader.h>
#include <Core/Utils/Log.h>
#include <Core/SystemInformation.h>
#include <Core/Filesystem/Filesystem.h>
#include <Core/Filesystem/Directory.h>

#include <Core/Rendering/D3D12/PSO/PipelineStateObject.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace Kraid;
    using namespace Kraid::D3D12;
    
    Window window(hInst, L"Kraid Engine", 1280, 720);

    GPUDevice device;
    Fence main_fence(device, 0);

    GraphicsCommandList main_command_list(device);
    Swapchain swapchain(device, window, main_command_list);

    IndexBuffer ib = IndexBuffer(device, {0,1,2}, main_command_list);
    ib.Bind(main_command_list);

    VertexShader shader(L"./shader.hlsl", "VSMain");
    PixelShader ps(L"./shader.hlsl", "PSMain");

    RootSignature rs(device);
    GraphicsPipelineStateObject pso(shader, ps, rs, PrimitiveTopology::Triangle, BasicVertex::GenerateVertexDescription());
    pso.Compile(device);
    
    while(window.open)
    {
    }

    return 0;
}
