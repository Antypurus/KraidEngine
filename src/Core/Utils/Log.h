#pragma once

#include <cstdio>

#include "../types.h"
#include "../Windows.h"

#define LOGGING 1

#ifndef NDEBUG
    #define LOGGING_CONSOLE
#endif

#if LOGGING
namespace hvrt
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

#define __FILENAME__ (wcsrchr(__FILEW__, '\\') ? wcsrchr(__FILEW__, '\\') + 1 : __FILEW__)

#if LOGGING
#define LINFO(Message, ...) hvrt::LogConsole::LogInfo(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#define LWARNING(Message, ...) hvrt::LogConsole::LogWarning(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#define LERROR(Message, ...) hvrt::LogConsole::LogError(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#define LSUCCESS(Message, ...) hvrt::LogConsole::LogSuccess(__FILENAME__, __LINE__, Message, __VA_ARGS__);
#else
#define LINFO(Message, ...)
#define LWARNING(Message, ...)
#define LERROR(Message, ...)
#define LSUCCESS(Message, ...)
#endif
