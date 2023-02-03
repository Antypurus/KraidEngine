#pragma once

#include <Core/Threading/Thread.h>
#include <Core/Threading/Lock.h>
#include <Core/stdcpp.h>

#include <Core/DataStructures/StringView.h>

namespace Kraid
{
    class DirectoryWatcher;

    //TODO(Tiago): improve structure packing of both of these structures, especially directory, it wastes a ton of memory with the bool for the read changes loop
    class Directory
    {
    private:
        HANDLE m_directory_handle = nullptr;
        bool m_changes_obtained_once = false;//NOTE(Tiago): used to detect if the directory change list has been read at least once.
    public:
        Directory() = default;
        Directory(const wchar_t* path);
        ~Directory();
        std::vector<std::wstring> GetChangedFiles();

        friend DirectoryWatcher;
    };

    class DirectoryWatcher
    {
    friend Directory;
    private:
        HANDLE m_wait_event_handle = nullptr;
        Directory m_watched_dir;
        Thread m_watch_thread;
        std::unordered_map<std::wstring, std::vector<std::function<void(void)>>> m_file_change_callbacks;
        Mutex m_file_callback_mutex = {};

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
