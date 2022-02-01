#pragma once

#include <Core/types.h>

namespace Kraid
{

    wchar_t* GetAbsoluteFilepath(const wchar_t* filepath);

    struct File
    {
        HANDLE file_handle = nullptr;

        File() = default;
        File(const wchar_t* filepath, bool append = false);
        File(const wchar_t* filepath, void(*callback)(void), bool append = false);
        bool Write(const uint8* data, uint64 size);
        bool Append(const uint8* data, uint64 size);
        ~File();

        private:
        static [[nodiscard]] wchar_t* ExtractDirectoryPath(const wchar_t* filepath);
    };

}
