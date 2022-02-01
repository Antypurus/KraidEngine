#pragma once

#include <cstdio>

#include "../types.h"
#include "../Windows.h"
#include "../BuildDefs.h"

#ifndef NDEBUG
    #define LOGGING_CONSOLE
#endif

#if LOGGING
namespace Kraid
{

    struct LogConsole
    {
        HANDLE stdin_handle;
        HANDLE stdout_handle;
        FILE* stdin_stream = nullptr;
        FILE* stdout_stream = nullptr;
        FILE* stderr_stream = nullptr;

    public:
        static void LogInfo(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...);
        static void LogInfo(const wchar_t* source_file, uint32 source_line, const char* message_format, ...);
        static void LogWarning(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...);
        static void LogWarning(const wchar_t* source_file, uint32 source_line, const char* message_format, ...);
        static void LogError(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...);
        static void LogError(const wchar_t* source_file, uint32 source_line, const char* message_format, ...);
        static void LogSuccess(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...);
        static void LogSuccess(const wchar_t* source_file, uint32 source_line, const char* message_format, ...);

    protected:
        static LogConsole instance;

        LogConsole();
        ~LogConsole();
    };

}
#endif

#if defined(__clang__)
    #define WidenHelper(x)  L##x
    #define Widen(x)        WidenHelper(x)
    #define __FILENAME__ Widen(__FILE_NAME__)
#else
    #define __FILENAME_INTERNAL__ (wcsrchr(__FILEW__, '\\') ? wcsrchr(__FILEW__, '\\') + 1 : __FILEW__)
    #define __FILENAME__ (wcsrchr(__FILENAME_INTERNAL__, '/') ? wcsrchr(__FILENAME_INTERNAL__, '/') + 1 : __FILENAME_INTERNAL__)
#endif

#if LOGGING
#define LINFO(Message, ...) Kraid::LogConsole::LogInfo(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#define LWARNING(Message, ...) Kraid::LogConsole::LogWarning(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#define LERROR(Message, ...) Kraid::LogConsole::LogError(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#define LSUCCESS(Message, ...) Kraid::LogConsole::LogSuccess(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#else
#define LINFO(Message, ...)
#define LWARNING(Message, ...)
#define LERROR(Message, ...)
#define LSUCCESS(Message, ...)
#endif
