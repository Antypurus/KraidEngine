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
#include <Core/Rendering/D3D12/ShaderParameter.h>

#include <Core/Rendering/Model/ModelLoader.h>

struct cbuffer
{
    DirectX::XMFLOAT4 color;
};

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace Kraid;
    using namespace Kraid::D3D12;

    Window window(hInst, L"Kraid Engine", 1280, 720);

    GPUDevice device;
    Fence main_fence(device, 0);
    GraphicsCommandList main_command_list(device);
    Swapchain swapchain(device, window, main_command_list);

    Model model = ModelLoader::LoadOBJModel(device,main_command_list,"./Resources/Models/bunny/bunny.obj");
    LSUCCESS("Model Done Loading");

    VertexShader vs(L"./shader.hlsl", "VSMain");
    PixelShader ps(L"./shader.hlsl", "PSMain");

    TextureSampler point_sampler(device, device.sampler_descriptior_heap, TextureSamplingMode::Point);
    TextureSampler linear_sampler(device, device.sampler_descriptior_heap, TextureSamplingMode::Linear);
    TextureSampler anisotropic_sampler(device, device.sampler_descriptior_heap, TextureSamplingMode::Anisotropic);

    cbuffer input;
    input.color = {1,0,0, 1};
    ShaderParameter<cbuffer> color_param = ShaderParameter<cbuffer>(device, main_command_list, input);

    RootSignature rs(device, {}, {},
            {//descriptor table array
                {//descriptor table for point sampling
                    {SamplerDescriptorTableEntry(0)}
                },
                {//descriptor table for linear sampler
                    {SamplerDescriptorTableEntry(1)}
                },
                {//descriptor table for anisotropic sampler
                    {SamplerDescriptorTableEntry(2)}
                },
                {//descriptor table for shader resource view
                    {SRVDescriptorTableEntry(0)}
                },
                {
                    {CBVDescriptorTableEntry(0)}
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

    //TODO(Tiago):needs cleanup
    ID3D12DescriptorHeap* heaps[] = {
        device.sampler_descriptior_heap.descriptor_heap.Get(),
        device.shader_resource_heap.descriptor_heap.Get()
    };
    main_command_list->SetDescriptorHeaps(2, heaps);
    point_sampler.Bind(main_command_list, 0);
    linear_sampler.Bind(main_command_list, 1);
    anisotropic_sampler.Bind(main_command_list, 2);

    input.color =  {0,1.0f,0,0 };
    color_param.UpdateData(input, main_command_list);
    while(window.open)
    {
        swapchain.StartFrame(main_command_list);

        pso.Bind(main_command_list);

        //TODO(Tiago):needs cleanup
        main_command_list->SetDescriptorHeaps(2, heaps);
        point_sampler.Bind(main_command_list, 0);
        linear_sampler.Bind(main_command_list, 1);
        anisotropic_sampler.Bind(main_command_list, 2);
        color_param.Bind(main_command_list, 4);
        
        model.Draw(main_command_list, 3);
        
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
