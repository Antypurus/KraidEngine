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
        D3D_FEATURE_LEVEL feature_level;

        //we determine the best gpu to use
        GPUDevice();

        //specify the index of the gpu to be used
        GPUDevice(uint8 gpu_index);
        GPUDevice(IDXGIAdapter4* adapter);
	};

}
