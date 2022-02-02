#include "Directory.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>
#include <Core/Threading/Thread.h>

namespace Kraid
{

    Directory::Directory(const wchar_t* path)
    {
        this->directory_handle = CreateFile(path,
            FILE_LIST_DIRECTORY | GENERIC_READ | GENERIC_WRITE ,
            FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);
        if(this->directory_handle == INVALID_HANDLE_VALUE)
        {
            PRINT_WINERROR();
        }
    }

    std::vector<std::wstring> Directory::GetChangedFiles()
    {
        uint64 entries_read = 0;
        FILE_NOTIFY_INFORMATION* changed_entries = (FILE_NOTIFY_INFORMATION*)malloc(1024);
        //NOTE(Tiago): there is a weird behavior where a write causes two change notifications to be launched for the same file, this seems to be related to metadata. The issue is that in teh first time that a change is detected, the second notification is not received and as such we have to have a way to handle that special corner case.
        bool result = ReadDirectoryChangesW(this->directory_handle, (void*)changed_entries, 1024, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, (DWORD*)&entries_read, NULL, NULL);
        if(this->changes_obtained_once)
        {
            result = ReadDirectoryChangesW(this->directory_handle, (void*)changed_entries, 1024, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, (DWORD*)&entries_read, NULL, NULL);
        }
        this->changes_obtained_once = true;
        if(result == false)
        {
            PRINT_WINERROR();
            return {};
        }

        std::vector<std::wstring> changed_files;
        for(uint64 i = 0; i < (entries_read / sizeof(FILE_NOTIFY_INFORMATION)); ++i)
        {
            wchar_t* filename = (wchar_t*)malloc(changed_entries[i].FileNameLength + 2);
            memset(filename, 0, changed_entries[i].FileNameLength + 2);
            memcpy(filename, changed_entries[i].FileName, changed_entries[i].FileNameLength);
            changed_files.emplace_back(filename);
            free(filename);
        }
        return changed_files;
    }

    DirectoryWatcher::DirectoryWatcher(const wchar_t* directory)
    {
        this->watched_dir = {directory};
        this->wait_event_handle = FindFirstChangeNotification(directory, false, FILE_NOTIFY_CHANGE_LAST_WRITE);
        if(this->wait_event_handle == INVALID_HANDLE_VALUE)
        {
            PRINT_WINERROR();
            return;
        }

        Thread watch_thread([this](void* args) -> DWORD
        {
            while(true)
            {
                uint32 wait_status = WaitForMultipleObjects(1, &this->wait_event_handle, TRUE, INFINITE);
                if(wait_status == WAIT_OBJECT_0)
                {
                    auto changed_files = this->watched_dir.GetChangedFiles();
                    for(auto&& file: changed_files)
                    {
                        wprintf(L"%ls\n", (wchar_t*)file.c_str());
                    }
                }
            }
            return 0;
        });
    
    }

}
