#pragma once

#include <Core/types.h>
#include <string>
#include <functional>

namespace Kraid
{

    std::wstring GetAbsoluteFilepath(const std::wstring& filepath);

    struct Buffer
    {
        uint8* data;
        uint64 size;

        Buffer() = default;
        Buffer(uint8* data, uint64 size):data(data),size(size) {};
        Buffer(const Buffer& other)
        {
            this->data = (uint8*)malloc(other.size);
            memcpy(this->data, other.data, other.size);
            this->size = other.size;
        }
        ~Buffer()
        {
            free(data);
            data = nullptr;
            size = 0;
        }
    };

    struct File
    {
        HANDLE file_handle = nullptr;

        File() = default;
        File(const wchar_t* filepath, bool append = false);
        File(const wchar_t* filepath, const std::function<void(void)>& callback, bool append = false);
        bool Write(const uint8* data, uint64 size);
        bool Append(const uint8* data, uint64 size);
        Buffer Read();

        ~File();

        private:
        static inline [[nodiscard]] std::pair<wchar_t*, wchar_t*> SplitFilepah(const wchar_t* filepath);
        static inline [[nodiscard]] wchar_t* ExtractDirectoryPath(const wchar_t* filepath);
    };

}
