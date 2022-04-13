#pragma once

#include <string.h>
#include <Core/types.h>
#include "ArrayView.h"

namespace Kraid
{
    
    class StringView:public ArrayView<const char>
    {
    public:
        StringView(const char* string):ArrayView(string, strlen(string)){};
        StringView(const char* string, uint64 length): ArrayView(string, length){};
        StringView(const std::string& string): ArrayView(string.data(), string.length()){};

        const char* Get() const {return this->data;};
    };

    class WideStringView:public ArrayView<const wchar_t>
    {
    public:
        WideStringView(const wchar_t* string): ArrayView(string, wcslen(string)){};
        WideStringView(const wchar_t* string, uint64 length): ArrayView(string, length){};
        WideStringView(const std::wstring& string): ArrayView(string.data(), string.length()){};

        const wchar_t* Get() const { return this->data; };
    };

}
