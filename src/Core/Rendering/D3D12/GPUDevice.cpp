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

    GPUDevice::GPUDevice(uint8 gpu_index)
    {
#ifndef NDEBUG
        this->EnableDebugLayer();
#endif

        this->CreateD3D12Device(gpu_index);
    }

    GPUDevice::GPUDevice(IDXGIAdapter4* adapater)
    {
#ifndef NDEBUG
        this->EnableDebugLayer();
#endif

        this->CreateD3D12DeviceFromAdapater(adapater);
    }

    inline void GPUDevice::CreateD3D12Device(uint8 gpu_index)
    {
        DXGIFactory factory;
        std::vector<ComPtr<IDXGIAdapter4>> gpu_list = factory.GetGPUList();
        this->CreateD3D12DeviceFromAdapater(gpu_list[gpu_index].Get());
    }

    inline void GPUDevice::CreateD3D12DeviceFromAdapater(IDXGIAdapter4* adapter)
    {
        constexpr uint64 feature_level_count = sizeof(feature_level_query_order_list) / sizeof(D3D_FEATURE_LEVEL);
        for(uint8 i = 0; i < feature_level_count; ++i)
        {
            HRESULT res = D3D12CreateDevice(adapter, feature_level_query_order_list[i], IID_PPV_ARGS(&this->device));
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

    inline void GPUDevice::EnableDebugLayer()
    {
        HRESULT res = D3D12GetDebugInterface(IID_PPV_ARGS(&this->debug_layer));
        this->debug_layer->EnableDebugLayer();
        this->debug_layer->SetEnableGPUBasedValidation(true);
        LSUCCESS("D3D12 debug layer enabled");
    }

    ID3D12Device8* GPUDevice::operator->()
    {
        return this->device.Get();
    }

}
