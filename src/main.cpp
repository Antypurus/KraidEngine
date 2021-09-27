#include <windows.h>
#include <iostream>

#include <dxgi1_6.h>
#include <wrl.h>

#include <Core/Core.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    using namespace hvrt;
    Window window(hInst, L"window1", 1280, 720);

    Microsoft::WRL::ComPtr<IDXGIFactory7> pFactory;
    CreateDXGIFactory2(NULL, IID_PPV_ARGS(&pFactory));

    UINT i = 0;
    IDXGIAdapter4* pAdapter;
    std::vector <IDXGIAdapter4*> vAdapters;
    while (pFactory->EnumAdapters(i, (IDXGIAdapter**)&pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC3 desc;
        pAdapter->GetDesc3(&desc);

        vAdapters.push_back(pAdapter);
        ++i;
    }

    while(window.open)
    {
    }

    return 0;
}
