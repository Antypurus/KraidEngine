#include "Filesystem.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>
#include <stdio.h>
#include <string.h>
#include <string>

namespace Kraid
{

    wchar_t* GetAbsoluteFilepath(const wchar_t* filepath)
    {
        wchar_t* absolute_path = (wchar_t*)malloc(MAX_PATH);
        DWORD absolute_path_len = GetFullPathNameW(filepath, MAX_PATH, (wchar_t*)absolute_path, nullptr);
        if(absolute_path_len > MAX_PATH)
        {
            free(absolute_path);
            absolute_path = (wchar_t*)malloc(absolute_path_len + 1);
            absolute_path_len = GetFullPathNameW(filepath, absolute_path_len + 1, absolute_path, nullptr);
        }
        absolute_path[absolute_path_len] = 0;
        std::wstring ret = {std::move(absolute_path)};
        return (wchar_t*)absolute_path;
    }

    File::File(const wchar_t* filepath, bool append)
    {
        uint32 open_mode;
        if(append)
        {
            open_mode = OPEN_ALWAYS;
        }
        else
        {
            open_mode = CREATE_ALWAYS;
        }
        this->file_handle = CreateFile2(filepath, 
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            open_mode,
            nullptr);
        if(this->file_handle == INVALID_HANDLE_VALUE)
        {
            LERROR(FormatErrorMessage(GetLastError()));
            return;
        }
        LSUCCESS(L"File Opened");
    }
    
    File::File(const wchar_t* filepath, void(*callback)(void), bool append)
    {
        wchar_t* directory_path = this->ExtractDirectoryPath(filepath);
        //open directory
        HANDLE dir_handle = CreateFile(directory_path,
            FILE_LIST_DIRECTORY | GENERIC_READ | GENERIC_WRITE ,
            FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);
        
        if(dir_handle == INVALID_HANDLE_VALUE)
        {
            LERROR(FormatErrorMessage(GetLastError()));
            return;
        }

        //create wait event
        HANDLE event_handle = FindFirstChangeNotificationW(directory_path, false, FILE_NOTIFY_CHANGE_LAST_WRITE);
        if(event_handle == INVALID_HANDLE_VALUE)
        {
            LERROR("Failed to create even handle to wait for file write notifications");
            return;
        }
        while(true)
        {
            uint32 wait_status = WaitForMultipleObjects(1, &event_handle, TRUE, INFINITE);
            switch(wait_status)
            {
                case(WAIT_OBJECT_0):
                {
                    static bool first_loop = true;
                    DWORD entries_read = 0;

                    FILE_NOTIFY_INFORMATION* change_entries = (FILE_NOTIFY_INFORMATION*)malloc(1024);
                    bool res = ReadDirectoryChangesW(dir_handle, (void*)change_entries, 1024, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, (DWORD*)&entries_read,NULL, NULL);
                    if(!first_loop)
                    {
                        res = ReadDirectoryChangesW(dir_handle, (void*)change_entries, 1024, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, (DWORD*)&entries_read,NULL, NULL);
                    }
                    first_loop = false;
                    if(!res)
                    {
                        LERROR(FormatErrorMessage(GetLastError()));
                        continue;
                    }

                    for(uint32 i = 0; i < (entries_read / sizeof(FILE_NOTIFY_INFORMATION)); ++i)
                    {
                        for(uint32 j = 0; j < change_entries[i].FileNameLength; ++j)
                        {
                            printf("%c", ((char*)change_entries[i].FileName)[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    typedef void (*write_callback)(uint32, uint32, OVERLAPPED*);
    bool File::Write(const uint8* data, uint64 size)
    {
        SetFilePointer(this->file_handle, 0, NULL, FILE_BEGIN);
        //NOTE(Tiago):not using the EX version leads to cleaner code in my opinion, at no loss to functionality
        uint32 written = 0;
        bool result = WriteFile(
                this->file_handle,
                (void*)data, size,
                (DWORD*)&written,
                NULL);
        if(!result)
        {
            wchar_t* message = FormatErrorMessage(GetLastError());
            LERROR(message);
            free(message);
            return false;
        }
        uint32 result_code = WaitForSingleObjectEx(this->file_handle, INFINITE, true);
        if(result_code == WAIT_FAILED)
        {
            wchar_t* message = FormatErrorMessage(GetLastError());
            LERROR(message);
            free(message);
            return false;
        }
        LSUCCESS("Data Written To File");
        return true;
    }

    bool File::Append(const uint8 *data, uint64 size)
    {
        SetFilePointer(this->file_handle, 0, NULL, FILE_END);
        uint32 written = 0;
        bool result = WriteFile(
                this->file_handle,
                (void*)data, size,
                (DWORD*)&written,
                NULL);
        if(!result)
        {
            wchar_t* message = FormatErrorMessage(GetLastError());
            LERROR(message);
            free(message);
            return false;
        }
        uint32 result_code = WaitForSingleObjectEx(this->file_handle, INFINITE, true);
        if(result_code == WAIT_FAILED)
        {
            wchar_t* message = FormatErrorMessage(GetLastError());
            LERROR(message);
            free(message);
            return false;
        }
        LSUCCESS("Data Written To File");
        return true;
    }

    File::~File()
    {
        if(this->file_handle == INVALID_HANDLE_VALUE || this->file_handle == nullptr) return;
        if(!CloseHandle(this->file_handle))
        {
            LERROR(FormatErrorMessage(GetLastError()));
            return;
        }
        LSUCCESS("File handle closed");
    }

    wchar_t* File::ExtractDirectoryPath(const wchar_t* filepath)
    {
        uint32 path_len = wcslen(filepath);
        uint32 dir_split_index = 0;
        for(int32 i = path_len; i >= 0; i--)
        {
            if(filepath[i] == L'\\' || filepath[i] == L'/')
            {
                dir_split_index = i;
                break;
            }
        }
        wchar_t* ret = (wchar_t*)malloc((dir_split_index + 2) * sizeof(wchar_t));
        memcpy(ret, filepath, (dir_split_index + 1) * sizeof(wchar_t));
        ret[dir_split_index + 1] = L'\0';
        return ret;
    }

}
