#include "GUI.h"

#include <DearImGui/imgui.h>
#include <DearImGui/imgui_impl_dx12.h>
#include <DearImGui/imgui_impl_win32.h>

#include <Core/Rendering/D3D12/GPUDevice.h>
#include <Core/Window/Window.h>

namespace Kraid
{

    ImGUI::ImGUI(GPUDevice& device, Window& window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();
    }

}
