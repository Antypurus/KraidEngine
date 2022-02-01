#pragma once

#include <Core/Rendering/D3D12/D3D12.h>
#include <vector>

namespace Kraid
{

    namespace D3D12
    {

        using namespace Microsoft::WRL;

        struct DXGIFactory
        {
            ComPtr<IDXGIFactory7> factory;

            DXGIFactory();
            ~DXGIFactory() = default;

            IDXGIFactory7* operator->();
            std::vector<ComPtr<IDXGIAdapter4>> GetGPUList() const;
            void LogGPUList() const;
        };

    }

}
