#include <Core/Core.h>
#include <dxgidebug.h>
#include <chrono>
#include <functional>
using namespace std::chrono;

using namespace DirectX;
struct cbuffer
{

    XMMATRIX view_matrix;
    XMMATRIX projection_matrix;
};

#define rad(x) (x*DirectX::XM_PI)/180.0f

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace Kraid;
    using namespace Kraid::D3D12;
#if 1
    Window window(hInst, L"Kraid Engine", 1280, 720);
    Camera camera(window,{ 0, 0, 0 }, { 0,0,1 } );

    GPUDevice device;
    Fence main_fence;
    GraphicsCommandList main_command_list;
    Swapchain swapchain(window, main_command_list);

    Model model = ModelLoader::LoadOBJModel("./Resources/Models/vokselia_spawn/vokselia_spawn.obj", main_command_list);

    VertexShader vs(L"./Shaders/shader.hlsl", "VSMain");
    PixelShader ps(L"./Shaders/shader.hlsl", "PSMain");

    TextureSampler point_sampler(device.sampler_descriptior_heap, TextureSamplingMode::Point);
    TextureSampler linear_sampler(device.sampler_descriptior_heap, TextureSamplingMode::Linear);
    TextureSampler anisotropic_sampler(device.sampler_descriptior_heap, TextureSamplingMode::Anisotropic);

    cbuffer input;
    XMMATRIX projection_matrix = swapchain.ProjectionMatrix();
    XMMATRIX view_matrix = camera.ViewMatrix();
    input.view_matrix = view_matrix;
    input.projection_matrix = projection_matrix;
    ShaderParameter<cbuffer> color_param = ShaderParameter<cbuffer>(main_command_list, input);
    color_param.parameter_buffer->SetName(L"Global Input Parameter");

    RootSignature rs({}, {},
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
                },
                {
                    {CBVDescriptorTableEntry(1)}
                }
            });
    GraphicsPipelineStateObject pso(vs, ps, rs, PrimitiveTopology::Triangle, Vertex::GenerateVertexDescription());

    main_command_list.Close();
    main_command_list.Execute();
    //TODO(Tiago):this could be cleaned up into a wait for command list completion
    main_fence.Increment(device.direct_command_queue);
    main_fence.WaitForCompletion();
    main_command_list.Reset();

    pso.Bind(main_command_list);

    main_command_list.SetSamplerHeap(device.sampler_descriptior_heap);
    main_command_list.SetShaderResourceHeap(device.shader_resource_heap);

    point_sampler.Bind(main_command_list, 0);
    linear_sampler.Bind(main_command_list, 1);
    anisotropic_sampler.Bind(main_command_list, 2);

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    CircularBuffer<float> frame_times(400);

    HANDLE m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    ImGUI gui(window);
    while(window.open)
    {
        float highest_time = 0;
        float lowest_time = FLT_MAX;
        float cummulative_frame_time = 0;
        for(uint64 i = 0; i < frame_times.size; ++i)
        {
            float frame_time = frame_times.buffer[i];
            cummulative_frame_time += frame_time;
            if(frame_time > highest_time) highest_time = frame_time;
            if(frame_time < lowest_time) lowest_time = frame_time;
        }
        float average_frame_time = cummulative_frame_time / ((float)frame_times.size);
        float average_fps = 1.0f / (average_frame_time/(1000.0f));
        std::string average_fps_string = "Average FPS: " + std::to_string(average_fps);

        start = high_resolution_clock::now();

        swapchain.StartFrame(main_command_list);

        gui.StartFrame();

        ImGui::Begin("Frame time");
        ImGui::PlotLines("", frame_times.buffer, frame_times.size, 0, average_fps_string.c_str(), lowest_time, highest_time, ImVec2(0.0f, 100.0f));
        ImGui::End();

        input.view_matrix = camera.ViewMatrix();
        input.projection_matrix = swapchain.ProjectionMatrix();
        color_param.UpdateData(input, main_command_list);

        pso.Bind(main_command_list);

        main_command_list.SetShaderResourceHeap(device.shader_resource_heap);
        main_command_list.SetSamplerHeap(device.sampler_descriptior_heap);
        point_sampler.Bind(main_command_list, 0);
        linear_sampler.Bind(main_command_list, 1);
        anisotropic_sampler.Bind(main_command_list, 2);
        color_param.Bind(main_command_list, 4);

        model.Draw(main_command_list, 3, 5);

        gui.EndFrame(main_command_list);

        swapchain.EndFrame(main_command_list);

        main_command_list.Close();
        main_command_list.Execute();
        swapchain.Present();

        main_fence.Increment(device.direct_command_queue);
        main_fence.WaitForCompletion();

        main_command_list.Reset();

        end = high_resolution_clock::now();

        auto elapsed = duration_cast<nanoseconds>(end - start).count();
        float frame_time = elapsed/1000000.0f;
        frame_times.push(frame_time);
    }
#else
    Job job;
    job.job = [](void*){ printf("Hello world"); };
    while(true)
    {
        JobSystem::AddJob(job, JobPriority::Low);
    }
    while(true){};
#endif
    return 0;
}
