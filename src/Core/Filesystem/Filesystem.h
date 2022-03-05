#pragma once

#include <Core/types.h>
#include <string>
#include <functional>

#include <Core/DataStructures/Buffer.h>

namespace Kraid
{

    std::wstring GetAbsoluteFilepath(const std::wstring& filepath);

    struct File
    {
        HANDLE file_handle = nullptr;

        File() = default;
        File(const wchar_t* filepath, bool append = false);
        File(const wchar_t* filepath, const std::function<void(void)>& callback, bool append = false);
        bool Write(const uint8* data, uint64 size);
        bool Append(const uint8* data, uint64 size);
        Buffer Read();
        uint64 GetSize() const;

        ~File();

        private:
        static inline [[nodiscard]] std::pair<wchar_t*, wchar_t*> SplitFilepah(const wchar_t* filepath);
        static inline [[nodiscard]] wchar_t* ExtractDirectoryPath(const wchar_t* filepath);
    };

}
