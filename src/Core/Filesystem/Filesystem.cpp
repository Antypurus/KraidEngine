#include "Filesystem.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>
#include <fileapi.h>

#include "Directory.h"

namespace Kraid
{

    std::wstring GetAbsoluteFilepath(const std::wstring& filepath)
    {
        wchar_t* absolute_path = (wchar_t*)malloc(MAX_PATH * sizeof(wchar_t));
        if(absolute_path == nullptr) {
            LERROR("Failed to allocate space for absolute filepath");
            return {};
        }
        DWORD absolute_path_len = GetFullPathNameW(filepath.c_str(), MAX_PATH, (wchar_t*)absolute_path, NULL);
        if(absolute_path_len > MAX_PATH)
        {
            free(absolute_path);
            absolute_path = (wchar_t*)malloc(absolute_path_len * sizeof(wchar_t));
            absolute_path_len = GetFullPathNameW(filepath.c_str(), absolute_path_len, absolute_path, nullptr);
        }
        absolute_path[absolute_path_len] = 0;
        return {std::move(absolute_path)};
    }

    File::File(const File& other)
    {
        this->file_handle = other.file_handle;
        this->refcount.Increment();
    }

    File::File(const wchar_t* filepath, bool append)
    {
        if(filepath == nullptr)
        {
            LERROR(L"Invalid filepath supplied");
            return;
        }

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
    
    File::File(const wchar_t* filepath, const std::function<void(void)>& callback, bool append)
    {
        if(filepath == nullptr)
        {
            LERROR(L"Invalid filepath supplied");
            return;
        }

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
            FILE_GENERIC_READ|FILE_GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            open_mode,
            nullptr);
        if(this->file_handle == INVALID_HANDLE_VALUE)
        {
            LERROR(FormatErrorMessage(GetLastError()));
            return;
        }
        LSUCCESS(L"File Opened");

        if(callback == nullptr)
        {
            LERROR(L"Invalid callback supplied. Callback has not been registered");
            return;
        }

        auto paths = File::SplitFilepah(filepath);
        DirectoryWatcher::GetDirectoryWatcher(paths.first).RegisterFileChangeCallback(paths.second, callback);

        free(paths.first);
        free(paths.second);
    }

    File& File::operator=(const File& other)
    {
        this->file_handle = other.file_handle;
        return *this;
    }

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

    Buffer File::Read()
    {
        SetFilePointer(this->file_handle, 0, NULL, FILE_BEGIN);

        uint64 file_size = this->GetSize();
        uint32 dummy_var = 0;
        uint8* buffer = nullptr;
        while(buffer == nullptr)
        {
            buffer = (uint8*)malloc(file_size + 1);
        }

        //NOTE(Tiago): the following loop reads teh file in chunks whose size can fit in a 32-bit value, allowing us to read a file whose value cannot fit in a 32-bit value
        uint64 total_amount_left_to_read = file_size;
        while(total_amount_left_to_read > UINT32_MAX)
        {
            uint32 amount_to_read = UINT32_MAX;
            bool result = ReadFile(this->file_handle, buffer + (file_size - total_amount_left_to_read), amount_to_read, (DWORD*)&dummy_var, NULL);
            if(result == false)
            {
                PRINT_WINERROR();
                free(buffer);
                return {};
            }
            total_amount_left_to_read -= UINT32_MAX;
        }
        bool result = ReadFile(this->file_handle, buffer + (file_size - total_amount_left_to_read), (uint32)total_amount_left_to_read, (DWORD*)&dummy_var, NULL);
        if(result == false)
        {
            PRINT_WINERROR();
            free(buffer);
            return {};
        }
         
        buffer[file_size] = 0;
        return {buffer, file_size};
    }

    uint64 File::GetSize() const 
    {
        uint32 file_size_high_word = 0;//NOTE(Tiago):contaisn the high 32-bits of data from a 64-bit filesize
        uint32 file_size_low_word = GetFileSize(this->file_handle, (DWORD*)&file_size_high_word);
        if(file_size_low_word == INVALID_FILE_SIZE)
        {
            PRINT_WINERROR();
            return 0;
        }
        return (((uint64)file_size_high_word << 32 )| file_size_low_word);
    }

    File::~File()
    {
        if(this->file_handle == INVALID_HANDLE_VALUE || this->file_handle == nullptr) return;

        refcount.Decrement();
        if(refcount.ShouldFree())
        {
            if(!CloseHandle(this->file_handle))
            {
                LERROR(FormatErrorMessage(GetLastError()));
                return;
            }
            LSUCCESS("File handle closed");
        }
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

    std::pair<wchar_t*, wchar_t*> File::SplitFilepah(const wchar_t* filepath)
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
        uint64 dirpath_len = dir_split_index + 1;
        uint64 filename_len = path_len - dir_split_index;
        wchar_t* dirpath = (wchar_t*)malloc(dirpath_len * sizeof(wchar_t) + 2);
        wchar_t* filename = (wchar_t*)malloc(filename_len * sizeof(wchar_t));
        memcpy(dirpath, filepath, dirpath_len * sizeof(wchar_t));
        memcpy(filename, filepath + dirpath_len, filename_len * sizeof(wchar_t));
        dirpath[dir_split_index + 1] = L'\0';
        return std::make_pair(dirpath, filename);
    }

}
