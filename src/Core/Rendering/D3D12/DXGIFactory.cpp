#include "DXGIFactory.h"

#include <Core/types.h>
#include <Core/Utils/Log.h>

namespace hvrt
{

    DXGIFactory::DXGIFactory()
    {
#ifndef NDEBUG
        CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG , IID_PPV_ARGS(&this->factory));
#else
        CreateDXGIFactory2(NULL, IID_PPV_ARGS(&this->factory));
#endif
    }

    IDXGIFactory7* DXGIFactory::operator->()
    {
        return this->factory.Get();
    }

    std::vector<ComPtr<IDXGIAdapter4>> DXGIFactory::GetGPUList() const
    {
        std::vector<ComPtr<IDXGIAdapter4>> gpu_list;

        uint8 gpu_index = 0;
        IDXGIAdapter4* adapter = nullptr;
        while(this->factory->EnumAdapters1(gpu_index, (IDXGIAdapter1**)&adapter) != DXGI_ERROR_NOT_FOUND)
        {
            gpu_list.push_back(adapter);
            //release the local reference to the adapater which is no longer needed
            adapter->Release();
            adapter = nullptr;
            
            gpu_index++;
        }

        return gpu_list;
    }

    void DXGIFactory::LogGPUList() const
    {
        std::vector<ComPtr<IDXGIAdapter4>> gpu_list = this->GetGPUList();

        LINFO("GPU List:");
        for(uint8 i = 0; i < gpu_list.size(); ++i)
        {
            DXGI_ADAPTER_DESC3 gpu_desc;
            gpu_list[i]->GetDesc3(&gpu_desc);

            LINFO(L"GPU %d:\n \tName:%s \n \tDedicated VRAM:%d MB\n", i, gpu_desc.Description, gpu_desc.DedicatedVideoMemory / (1024 * 1024));
        }
    }

}
