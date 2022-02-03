#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <Core/Threading/Thread.h>

namespace Kraid
{
    //TODO(Tiago): improve structure packing of both of these structures, especially directory, it wastes a ton of memory with the bool for the read changes loop
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
    friend Directory;
    public:
        HANDLE wait_event_handle = nullptr;
        Directory watched_dir;
        Thread watch_thread;
        std::unordered_map<std::wstring, std::vector<std::function<void(void)>>> file_change_callbacks;
    private:
        inline static std::unordered_map<std::wstring, DirectoryWatcher> directory_watcher_instances = {};
    public:
        DirectoryWatcher() = default;
        DirectoryWatcher(const wchar_t* diretory);
        bool operator==(const DirectoryWatcher& other);
        void RegisterFileChangeCallback(const std::wstring& filename, const std::function<void(void)>& callback);
        static DirectoryWatcher& GetDirectoryWatcher(const std::wstring& directory);
    };

}
