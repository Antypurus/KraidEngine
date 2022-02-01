#pragma once

namespace Kraid
{

    struct Directory
    {
    public:
        HANDLE directory_handle = nullptr;
    public:
        Directory() = default;
        Directory(const wchar_t* path);
    };

    struct DirectoryWatcher
    {
    public:
        static DirectoryWatcher& GetDirectoryWatcher(const wchar_t* directory);
    private:
        DirectoryWatcher();
    };

}
