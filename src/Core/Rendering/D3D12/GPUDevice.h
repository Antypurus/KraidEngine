#pragma once

#include <Core/types.h>
#include <d3d12.h>
#include <wrl.h>

namespace hvrt
{

    using namespace Microsoft::WRL;

	struct GPUDevice
	{
        ComPtr<ID3D12Device9> device;
	};

}
