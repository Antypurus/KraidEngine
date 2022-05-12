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
#include <Core/SystemInformation.h>
#include <Core/Filesystem/Filesystem.h>
#include <Core/Filesystem/Directory.h>
#include <Core/Rendering/D3D12/PSO/PipelineStateObject.h>
#include "Core/Rendering/D3D12/RootSignature.h"
#include <Core/Windows.h>
#include <Core/Utils/Log.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace Kraid;
    using namespace Kraid::D3D12;
    
    Window window(hInst, L"Kraid Engine", 1280, 720);

    GPUDevice device;
    Fence main_fence(device, 0);

    GraphicsCommandList main_command_list(device);
    Swapchain swapchain(device, window, main_command_list);

    std::vector<BasicVertex> vertices = { {0,0,0.5},{0.5,-0.5,0.5},{1.0,0,0.5} };
    VertexBuffer vb = VertexBuffer<BasicVertex>(device, main_command_list, vertices);
    IndexBuffer ib = IndexBuffer(device, {2,1,0}, main_command_list);
    ib.Bind(main_command_list);

    VertexShader vs(L"./shader.hlsl", "VSMain");
    PixelShader ps(L"./shader.hlsl", "PSMain");

    RootSignature rs(device);
    GraphicsPipelineStateObject pso(device, vs, ps, rs, PrimitiveTopology::Triangle, BasicVertex::GenerateVertexDescription());

    main_command_list->Close();
    ID3D12CommandList* list[] = {main_command_list.command_list.Get()};
    device.direct_command_queue->ExecuteCommandLists(1, list);

    uint64 new_value = main_fence->GetCompletedValue() + 1;
    device.direct_command_queue->Signal(main_fence.fence.Get(), new_value);
    while(main_fence->GetCompletedValue() < new_value) {}

    device.direct_command_allocator->Reset();
    main_command_list->Reset(device.direct_command_allocator.Get(), pso.pso.Get());

    while(window.open)
    {
        swapchain.StartFrame(main_command_list);

        vb.Bind(main_command_list);
        ib.Bind(main_command_list);

        pso.Bind(main_command_list);

        //TODO(Tiago):needs cleanup
        main_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        main_command_list->DrawIndexedInstanced(3, 1, 0, 0, 0);

        swapchain.EndFrame(main_command_list);

        //TODO(Tiago):needs cleanup
        main_command_list->Close();
        device.direct_command_queue->ExecuteCommandLists(1, list);

        swapchain.Present();
     
        //TODO(Tiago): neeeds cleaup
        new_value = main_fence->GetCompletedValue() + 1;
        device.direct_command_queue->Signal(main_fence.fence.Get(), new_value);
        while(main_fence->GetCompletedValue() < new_value) {}
   
        device.direct_command_allocator->Reset();
        main_command_list->Reset(device.direct_command_allocator.Get(), pso.pso.Get());
    }

    return 0;
}
