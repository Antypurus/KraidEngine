#pragma once

#include <Core/types.h>
#include <string>
#include <functional>

#include <Core/DataStructures/Buffer.h>
#include <Core/DataStructures/RefCounter.h>

namespace Kraid
{

    std::wstring GetAbsoluteFilepath(const std::wstring& filepath);

    struct File
    {
    public:
        HANDLE file_handle = nullptr;
        ReferenceCounter refcount;

    public:
        File() = default;
        File(const File& other);
        File(const wchar_t* filepath, bool append = false);
        File(const wchar_t* filepath, const std::function<void(void)>& callback, bool append = false);
        File& operator=(const File& other);
        bool Write(const uint8* data, uint64 size);
        bool Append(const uint8* data, uint64 size);
        Buffer Read();
        uint64 GetSize() const;

        ~File();

        private:
        [[nodiscard]] static inline std::pair<wchar_t*, wchar_t*> SplitFilepah(const wchar_t* filepath);
        [[nodiscard]] static inline wchar_t* ExtractDirectoryPath(const wchar_t* filepath);
    };

}
