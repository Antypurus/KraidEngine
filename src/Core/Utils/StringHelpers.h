#pragma once

#include <Core/types.h>
#include <stdlib.h>
#include <string>

namespace Kraid
{

    inline wchar_t* to_unicode(const char* input, uint64 len = 0)
    {
        if(len == 0) len = strlen(input) + 1;

        wchar_t* output = (wchar_t*)malloc(len * sizeof(wchar_t));
        while(output == nullptr)
        {
            output = (wchar_t*)malloc(len * sizeof(wchar_t));
        }
        mbstowcs_s(&len, output, len, input, len - 1);

        return output;
    }

    inline std::string GetBasedir(const std::string& filepath)
    {
        size_t pos = filepath.find_last_of("/\\");
		return (std::string::npos == pos) ? "" : (filepath.substr(0, pos) + "/");
    }

}
