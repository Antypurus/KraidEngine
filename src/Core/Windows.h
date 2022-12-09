#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "BuildDefs.h"
#include <Core/Windows.h>

inline wchar_t* FormatErrorMessage(HRESULT res)
{
	LPWSTR messageBuffer = nullptr;
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
	return messageBuffer;
}

#define CAT_(x,y) x ## y
#define CAT(x,y) CAT_(x,y)

#if LOGGING

#include <stdlib.h>
#define PRINT_WINERROR()\
{\
	wchar_t* CAT(error_message,__LINE__) = FormatErrorMessage(GetLastError());\
	LERROR(CAT(error_message,__LINE__));\
	LocalFree(CAT(error_message,__LINE__));\
}

#define D3DCALL(function,success_message, ...)\
	HRESULT CAT(result_code,__LINE__) = function;\
	if(CAT(result_code,__LINE__) < 0)\
	{\
		wchar_t* CAT(error_message,__LINE__) = FormatErrorMessage(CAT(result_code,__LINE__));\
		LERROR(CAT(error_message,__LINE__));\
		LocalFree(CAT(error_message,__LINE__));\
		__debugbreak(); \
	}\
	else\
	{\
		LSUCCESS(success_message, __VA_ARGS__);\
	}
#else
#define D3DCALL(x, success_message, ...) x;
#define PRINT_WINERROR()
#endif
