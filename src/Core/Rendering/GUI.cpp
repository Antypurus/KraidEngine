#include "GUI.h"

#include <DearImGui/imgui.h>
#include <DearImGui/imgui_impl_dx12.h>
#include <DearImGui/imgui_impl_win32.h>

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Rendering/D3D12/CommandList.h>
#include <Core/Window/Window.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Kraid
{

    ImGUI::ImGUI(GPUDevice& device, Window& window)
    {
        this->gui_descriptor_heap = CBV_SRV_UAVDescriptorHeap(device, 10);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(window.window_handle);
        ImGui_ImplDX12_Init(
                device.device.Get(),
                2, DXGI_FORMAT_R8G8B8A8_UNORM,
                this->gui_descriptor_heap.descriptor_heap.Get(),
                this->gui_descriptor_heap->GetCPUDescriptorHandleForHeapStart(),
                this->gui_descriptor_heap->GetGPUDescriptorHandleForHeapStart());
        window.RegisterUniversalEventCallback(
                [](HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT
                {
                    return ImGui_ImplWin32_WndProcHandler(window_handle, message, wparam, lparam);
                }
        );
    }

    void ImGUI::StartFrame()
    {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
    }

    void ImGUI::EndFrame(GraphicsCommandList& command_list)
    {
        command_list.SetShaderResourceHeap(this->gui_descriptor_heap);
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list.command_list.Get());
    }

}
