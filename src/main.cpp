#include <d3dcommon.h>
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
#include <Core/Filesystem/Filesystem.h>
#include <Core/Filesystem/Directory.h>
#include <Core/Rendering/D3D12/PSO/PipelineStateObject.h>
#include "Core/Rendering/D3D12/RootSignature.h"
#include <Core/Windows.h>
#include <Core/Utils/Log.h>
#include <Core/Rendering/D3D12/Texture.h>

#include <Core/Rendering/Model/ModelLoader.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace Kraid;
    using namespace Kraid::D3D12;

    Window window(hInst, L"Kraid Engine", 1280, 720);

    GPUDevice device;
    Fence main_fence(device, 0);
    GraphicsCommandList main_command_list(device);
    Swapchain swapchain(device, window, main_command_list);

    Model model = ModelLoader::LoadOBJModel(device,main_command_list,"./Resources/Models/vokselia_spawn/vokselia_spawn.obj");

    VertexShader vs(L"./shader.hlsl", "VSMain");
    PixelShader ps(L"./shader.hlsl", "PSMain");

    TextureSampler point_sampler(device, device.sampler_descriptior_heap, TextureSamplingMode::Point);

    DescriptorTableRootParameter descriptor_table;
    descriptor_table.AddSamplerEntry(0, 0);
    RootSignature rs(device, {}, {},
            {//descriptor table array
                {//descriptor table 0
                    descriptor_table
                }
            });
    GraphicsPipelineStateObject pso(device, vs, ps, rs, PrimitiveTopology::Triangle, Vertex::GenerateVertexDescription());

    main_command_list.Close();
    main_command_list.Execute();
    //TODO(Tiago):this could be cleaned up into a wait for command list completion
    main_fence.Increment(device.direct_command_queue);
    main_fence.WaitForCompletion();
    main_command_list.Reset();

    pso.Bind(main_command_list);
    ID3D12DescriptorHeap* heaps[1] = {device.sampler_descriptior_heap.descriptor_heap.Get()};
    main_command_list->SetDescriptorHeaps(1, heaps);
    point_sampler.Bind(main_command_list, 0);

    while(window.open)
    {
        swapchain.StartFrame(main_command_list);

        pso.Bind(main_command_list);

        model.Draw(main_command_list);
        
        swapchain.EndFrame(main_command_list);

        main_command_list.Close();
        main_command_list.Execute();
        swapchain.Present();
     
        main_fence.Increment(device.direct_command_queue);
        main_fence.WaitForCompletion();
         
        main_command_list.Reset();
    }

    return 0;
}
