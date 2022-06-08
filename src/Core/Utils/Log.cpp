#include "Log.h"

#if LOGGING //only available if logging is enabled
namespace Kraid
{
    LogConsole LogConsole::instance = {};

    #ifdef LOGGING_CONSOLE // these implementatios are used when we use an explicitly created console for logging
    LogConsole::LogConsole()
    {
        AllocConsole();

        this->stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
        this->stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

        freopen_s(&this->stdin_stream, "CONIN$", "r", stdin);//replace stdin with the current console input buffer
        freopen_s(&this->stdout_stream, "CONOUT$", "w", stdout);//replace stdout with the current console output buffer
        freopen_s(&this->stderr_stream, "CONOUT$", "w", stderr);//replace stderr with the current console output buffer
    }

    LogConsole::~LogConsole()
    {
        FreeConsole();
    }

    void LogConsole::LogInfo(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        
        wprintf(L"[INFO]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vwprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    
    void LogConsole::LogInfo(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        
        wprintf(L"[INFO]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    void LogConsole::LogWarning(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        
        wprintf(L"[WARNING]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vwprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    void LogConsole::LogWarning(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        
        wprintf(L"[WARNING]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    void LogConsole::LogError(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        
        wprintf(L"[ERROR]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vwprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    void LogConsole::LogError(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        
        wprintf(L"[ERROR]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    void LogConsole::LogSuccess(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        
        wprintf(L"[SUCCESS]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vwprintf(message_format, va_args);
        va_end(va_args);


        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    void LogConsole::LogSuccess(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        
        wprintf(L"[SUCCESS]@%ls:%d => ",source_file,source_line);

        va_list va_args;
        va_start(va_args, message_format);
        vprintf(message_format, va_args);
        va_end(va_args);

        printf("\n");

        SetConsoleTextAttribute(instance.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    
    #else//these implementations are used when we are not using an explicit console and will log to the debugger console

    //NOTE(Tiago): there are some weird memory bound issues here due to the buffer sizes of wide strings. I still dont know why its happening as the math seems to add up, for now i just doubled the string buffer sizes to work around that, since this is only a debug/logging thing it should not be an issue but it would be nice to find out what is happening
    static std::pair<wchar_t*, char*> CreateOutputBuffer(const wchar_t* source_file, uint32 source_line, const char* message_format, va_list args)
    {
        //wide part buffer
        uint64 wneeded = _snwprintf(nullptr, 0, L"@%ls:%d => ", source_file, source_line) + 1;
        wchar_t* wide_buffer = (wchar_t*)malloc((wneeded+1)*2);//cant figure out why i need extra memory but i do otherwise i crash
        if (wide_buffer != nullptr)
        {
            swprintf_s(wide_buffer, wneeded, L"@%ls:%d => ", source_file, source_line);
        }
        
        uint64 needed = vsnprintf(nullptr, 0, message_format, args) + 1;
        char* ascii_buffer = (char*)malloc(needed*2);
        if (ascii_buffer != nullptr)
        {
            vsprintf_s(ascii_buffer, needed, message_format, args);
        }

        return std::make_pair(wide_buffer, ascii_buffer);
    }

    static wchar_t* CreateOutputBuffer(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, va_list args)
    {
        uint64 header_needed = _snwprintf(nullptr, 0, L"@%ls:%d => ", source_file, source_line);
        uint64 message_needed = _vsnwprintf(nullptr, 0, message_format, args) + 1;

        wchar_t* buffer = (wchar_t*)malloc((header_needed + message_needed)*2);
        if(buffer != nullptr)
        {
            //we have to tell the function that we have enough space for the null terminator in the first part of the copy, this is done by adding one to header_needed
            swprintf_s(buffer, header_needed + 1, L"@%ls:%d => ", source_file, source_line);
            vswprintf_s(buffer + header_needed, message_needed, message_format, args);
        }

        return buffer;
    }

    LogConsole::LogConsole():stdin_handle(NULL),stdout_handle(NULL)
    {
    }

    LogConsole::~LogConsole()
    {
    }

    void LogConsole::LogInfo(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        wchar_t* output_buffer = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[INFO]");
        OutputDebugStringW(output_buffer);
        OutputDebugStringA("\n");

        free(output_buffer);
    }
    
    void LogConsole::LogInfo(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        auto [header, message] = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[INFO]");
        OutputDebugStringW(header);
        OutputDebugStringA(message);
        OutputDebugStringA("\n");

        free(header);
        free(message);
    }

    void LogConsole::LogWarning(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        wchar_t* output_buffer = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[WARNING]");
        OutputDebugStringW(output_buffer);
        OutputDebugStringA("\n");

        free(output_buffer);
    }

    void LogConsole::LogWarning(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        auto [header, message] = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[WARNING]");
        OutputDebugStringW(header);
        OutputDebugStringA(message);
        OutputDebugStringA("\n");

        free(header);
        free(message);
    }

    void LogConsole::LogError(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        wchar_t* output_buffer = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[ERROR]");
        OutputDebugStringW(output_buffer);
        OutputDebugStringA("\n");

        free(output_buffer);
    }

    void LogConsole::LogError(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        auto [header, message] = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[ERROR]");
        OutputDebugStringW(header);
        OutputDebugStringA(message);
        OutputDebugStringA("\n");

        free(header);
        free(message);
    }

    void LogConsole::LogSuccess(const wchar_t* source_file, uint32 source_line, const wchar_t* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        wchar_t* output_buffer = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[SUCCESS]");
        OutputDebugStringW(output_buffer);
        OutputDebugStringA("\n");

        free(output_buffer);
    }

    void LogConsole::LogSuccess(const wchar_t* source_file, uint32 source_line, const char* message_format, ...)
    {
        va_list va_args;
        va_start(va_args, message_format);
        auto [header, message] = CreateOutputBuffer(source_file, source_line, message_format, va_args);
        va_end(va_args);

        OutputDebugStringA("[SUCCESS]");
        OutputDebugStringW(header);
        OutputDebugStringA(message);
        OutputDebugStringA("\n");

        free(header);
        free(message);
    }

    #endif
}
#endif
