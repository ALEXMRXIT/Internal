#pragma once
#include "framework.h"

#define BUFFER_SIZE 512

__forceinline HRESULT DXUTTrace(const CHAR* strFile, DWORD dwLine, HRESULT hr, const char* strMsg, bool bPopMsgBox) {
    return DXTrace(strFile, dwLine, hr, strMsg, bPopMsgBox);
}

__forceinline void DXUTOutputDebugStringW(LPCWSTR strMsg, ...) {
#if defined(DEBUG) || defined(_DEBUG)
    wchar_t strBuffer[BUFFER_SIZE];

    va_list args;
    va_start(args, strMsg);
    vswprintf_s(strBuffer, BUFFER_SIZE, strMsg, args);
    strBuffer[BUFFER_SIZE - 1] = L'\0';
    va_end(args);

    OutputDebugStringW(strBuffer);
#else
    UNREFERENCED_PARAMETER(strMsg);
#endif
}

__forceinline void DXUTOutputDebugStringA(LPCSTR strMsg, ...) {
#if defined(DEBUG) || defined(_DEBUG)
    char strBuffer[BUFFER_SIZE];

    va_list args;
    va_start(args, strMsg);
    vsprintf_s(strBuffer, BUFFER_SIZE, strMsg, args);
    strBuffer[BUFFER_SIZE - 1] = '\0';
    va_end(args);

    OutputDebugStringA(strBuffer);
#else
    UNREFERENCED_PARAMETER(strMsg);
#endif
}

#ifdef UNICODE
#define DXUTOutputDebugString DXUTOutputDebugStringW
#else
#define DXUTOutputDebugString DXUTOutputDebugStringA
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define DXUT_ERR(str,hr)           DXUTTrace(__FILE__, (DWORD)__LINE__, hr, str, false)
#define DXUT_ERR_MSGBOX(str,hr)    DXUTTrace(__FILE__, (DWORD)__LINE__, hr, str, true)
#define DXUTTRACE                  DXUTOutputDebugString
#else
#define DXUT_ERR(str,hr)           (hr)
#define DXUT_ERR_MSGBOX(str,hr)    (hr)
#define DXUTTRACE                  (__noop)
#endif