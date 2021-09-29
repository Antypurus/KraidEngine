#pragma once

#include <Core/types.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>

namespace hvrt
{

    using namespace Microsoft::WRL;

	struct GPUDevice
	{
        ComPtr<ID3D12Device8> device;
#ifndef NDEBUG
        ComPtr<ID3D12Debug3> debug_layer;
#endif
        D3D_FEATURE_LEVEL feature_level;

        //specify the index of the gpu to be used
        GPUDevice(uint8 gpu_index = 0);
        GPUDevice(IDXGIAdapter4* adapter);

        ID3D12Device8* operator->();

    private:
        inline void CreateD3D12Device(uint8 gpu_index);
        inline void CreateD3D12DeviceFromAdapater(IDXGIAdapter4* adapter);
        inline void EnableDebugLayer();
	};

}
