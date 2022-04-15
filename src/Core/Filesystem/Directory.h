#pragma once

#include <Core/Threading/Thread.h>
#include <Core/Threading/Lock.h>
#include <unordered_map>
#include <functional>
#include <vector>

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
        ~Directory();
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
        Mutex file_callback_mutex = {};
    private:
        inline static std::unordered_map<std::wstring, DirectoryWatcher> directory_watcher_instances = {};
        inline static Mutex watcher_instance_mutex = {};
    public:
        DirectoryWatcher() = default;
        DirectoryWatcher(const wchar_t* diretory);
        bool operator==(const DirectoryWatcher& other);
        void RegisterFileChangeCallback(const std::wstring& filename, const std::function<void(void)>& callback);
        static DirectoryWatcher& GetDirectoryWatcher(const std::wstring& directory);
    };

}
