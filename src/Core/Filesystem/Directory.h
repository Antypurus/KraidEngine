#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

namespace Kraid
{

    struct Directory
    {
    public:
        HANDLE directory_handle = nullptr;
        bool changes_obtained_once = false;//NOTE(Tiago): used to detect if the directory change list has been read at least once.
    public:
        Directory() = default;
        Directory(const wchar_t* path);
        std::vector<std::wstring> GetChangedFiles();
    };

    struct DirectoryWatcher
    {
    public:
        friend Directory;
        HANDLE wait_event_handle = nullptr;
        Directory watched_dir;
        std::unordered_map<std::wstring, std::vector<std::function<void(void)>>> callback;
    private:
    public:
        DirectoryWatcher(const wchar_t* diretory);
        static DirectoryWatcher& GetDirectoryWatcher(const wchar_t* directory);
    };

}
