#include "GPUDevice.h"

#include <unordered_map>

#include "Core/Utils/Log.h"
#include "DXGIFactory.h"

namespace hvrt
{
    const D3D_FEATURE_LEVEL feature_level_query_order_list[5] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    const std::unordered_map<D3D_FEATURE_LEVEL, std::string> feature_level_lookup =
    {
        {D3D_FEATURE_LEVEL_11_0, "11.0"},
        {D3D_FEATURE_LEVEL_11_1, "11.1"},
        {D3D_FEATURE_LEVEL_12_0, "12.0"},
        {D3D_FEATURE_LEVEL_12_1, "12.1"},
        {D3D_FEATURE_LEVEL_12_2, "12.2"},
    };

    GPUDevice::GPUDevice()
    {
        DXGIFactory factory;
        std::vector<ComPtr<IDXGIAdapter4>> gpu_list = factory.GetGPUList();

        for(uint8 i = 0; i < sizeof(feature_level_query_order_list); ++i)
        {
            HRESULT res = D3D12CreateDevice(gpu_list[0].Get(), feature_level_query_order_list[i], IID_PPV_ARGS(&this->device));
            if(res != S_OK)
            {
                continue;
            }
            else
            {
                this->feature_level = feature_level_query_order_list[i];
                LSUCCESS("Create D3D12 device with feature level %s", feature_level_lookup.at(this->feature_level).c_str());
                break;
            }
        }
    }

}
