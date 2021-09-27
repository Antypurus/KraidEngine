#pragma once

#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>

namespace hvrt
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
