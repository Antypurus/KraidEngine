#include "Directory.h"

#include <Core/Windows.h>
#include <Core/Utils/Log.h>

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
        PRINT_WINERROR();
    }

}
