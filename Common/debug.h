#pragma once
#include "framework.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DXUT_ERRW(str, hr, ...)         handleErrorLogW(__FILEW__, __LINE__, hr, str, __VA_ARGS__)
#define DXUT_ERR_MSGBOXW(str, hr, ...)  handleErrorLogAndShowW(__FILEW__, __LINE__, hr, str, __VA_ARGS__)
#define DXUT_ERR(str, hr, ...)          handleErrorLogA(__FILE__, __LINE__, hr, str, __VA_ARGS__)
#define DXUT_ERR_MSGBOX(str, hr, ...)   handleErrorLogAndShowA(__FILE__, __LINE__, hr, str, __VA_ARGS__)
#else
#define DXUT_ERRW(str, hr, ...)         (hr)
#define DXUT_ERR_MSGBOXW(str, hr, ...)  (hr)
#define DXUT_ERR(str, hr, ...)          (hr)
#define DXUT_ERR_MSGBOX(str, hr, ...)   (hr)
#endif

#pragma warning(push)
#pragma warning(disable: 6328)

#if defined(_WIN64)
#define CONTEXT_TYPE CONTEXT
#define RtlCaptureContext RtlCaptureContext
#define SymFromAddrT SymFromAddrW
#define CaptureStackBackTrace RtlCaptureStackBackTrace
#else
#define CONTEXT_TYPE CONTEXT
#define RtlCaptureContext RtlCaptureContext
#define SymFromAddrT SymFromAddr
#define CaptureStackBackTrace CaptureStackBackTrace
#endif

#define MAX_MESSAGE_LENGTH 1024
#define MAX_FINAL_MESSAGE_LENGTH 2048
#define MAX_STACK_FRAMES 64
#define NAME_BUFFER_SIZE 256

static __forceinline void WriteRegistersToLogA(FILE* logFile, CONTEXT_TYPE* context) {
    fprintf(logFile, "Registers:\n");
#if defined(_WIN64)
    fprintf(logFile,
        "    RAX: 0x%016llX, RBX: 0x%016llX, RCX: 0x%016llX, RDX: 0x%016llX\n",
        context->Rax, context->Rbx, context->Rcx, context->Rdx);
    fprintf(logFile,
        "    RSI: 0x%016llX, RDI: 0x%016llX, RBP: 0x%016llX, RSP: 0x%016llX\n",
        context->Rsi, context->Rdi, context->Rbp, context->Rsp);
    fprintf(logFile,
        "    RIP: 0x%016llX\n", context->Rip);
#else
    fprintf(logFile,
        "    EAX: 0x%08X, EBX: 0x%08X, ECX: 0x%08X, EDX: 0x%08X\n",
        context->Eax, context->Ebx, context->Ecx, context->Edx);
    fprintf(logFile,
        "    ESI: 0x%08X, EDI: 0x%08X, EBP: 0x%08X, ESP: 0x%08X\n",
        context->Esi, context->Edi, context->Ebp, context->Esp);
    fprintf(logFile,
        "    EIP: 0x%08X\n", context->Eip);
#endif
    fputc('\n', logFile);
}

static __forceinline void WriteRegistersToLogW(FILE* logFile, CONTEXT_TYPE* context) {
    fwprintf(logFile, L"Registers:\n");
#if defined(_WIN64)
    fwprintf(logFile,
        L"    RAX: 0x%016llX, RBX: 0x%016llX, RCX: 0x%016llX, RDX: 0x%016llX\n",
        context->Rax, context->Rbx, context->Rcx, context->Rdx);
    fwprintf(logFile,
        L"    RSI: 0x%016llX, RDI: 0x%016llX, RBP: 0x%016llX, RSP: 0x%016llX\n",
        context->Rsi, context->Rdi, context->Rbp, context->Rsp);
    fwprintf(logFile,
        L"    RIP: 0x%016llX\n", context->Rip);
#else
    fwprintf(logFile,
        L"    EAX: 0x%08X, EBX: 0x%08X, ECX: 0x%08X, EDX: 0x%08X\n",
        context->Eax, context->Ebx, context->Ecx, context->Edx);
    fwprintf(logFile,
        L"    ESI: 0x%08X, EDI: 0x%08X, EBP: 0x%08X, ESP: 0x%08X\n",
        context->Esi, context->Edi, context->Ebp, context->Esp);
    fwprintf(logFile,
        L"    EIP: 0x%08X\n", context->Eip);
#endif
    fputwc(L'\n', logFile);
}

static __forceinline void WriteFlagsToLogA(FILE* logFile, CONTEXT_TYPE* context) {
    fprintf(logFile, "Flags:\n");
#if defined(_WIN64)
    fprintf(logFile, "    RFLAGS: 0x%016llX\n", context->EFlags);
#else
    fprintf(logFile, "    EFLAGS: 0x%08X\n", context->EFlags);
#endif
    fputc('\n', logFile);
}

static __forceinline void WriteFlagsToLogW(FILE* logFile, CONTEXT_TYPE* context) {
    fwprintf(logFile, L"Flags:\n");
#if defined(_WIN64)
    fwprintf(logFile, L"    RFLAGS: 0x%016llX\n", context->EFlags);
#else
    fwprintf(logFile, L"    EFLAGS: 0x%08X\n", context->EFlags);
#endif
    fputwc(L'\n', logFile);
}

static __forceinline void WriteThreadAndProcessInfoToLogA(FILE* logFile) {
    HANDLE hThread = GetCurrentThread();
    HANDLE hProcess = GetCurrentProcess();

    fprintf(logFile, "Thread and Process Information:\n");
    fprintf(logFile, "    Current Thread ID: %lu\n", GetThreadId(hThread));
    fprintf(logFile, "    Current Process ID: %lu\n", GetProcessId(hProcess));
    fputc('\n', logFile);
}

static __forceinline void WriteThreadAndProcessInfoToLogW(FILE* logFile) {
    HANDLE hThread = GetCurrentThread();
    HANDLE hProcess = GetCurrentProcess();

    fwprintf(logFile, L"Thread and Process Information:\n");
    fwprintf(logFile, L"    Current Thread ID: %lu\n", GetThreadId(hThread));
    fwprintf(logFile, L"    Current Process ID: %lu\n", GetProcessId(hProcess));
    fputwc(L'\n', logFile);
}

static __forceinline void WriteStackTraceA(FILE* logFile) {
    HANDLE hProcess = GetCurrentProcess();
    SymInitialize(hProcess, NULL, TRUE);

    void* stack[MAX_STACK_FRAMES];
    USHORT frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, stack, NULL);

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(1, sizeof(SYMBOL_INFO) + NAME_BUFFER_SIZE * sizeof(CHAR));
    if (symbol) {
        symbol->MaxNameLen = NAME_BUFFER_SIZE - 1;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        for (USHORT i = 0; i < frames; i++) {
            DWORD64 address = (DWORD64)stack[i];
            if (!SymFromAddr(hProcess, address, NULL, symbol)) {
                DWORD error = GetLastError();
                fprintf(logFile, "%u: Unable to retrieve symbol for address 0x%016llX (Error: 0x%08X)\n",
                    frames - i - 1, address, error);
            }
            else {
                fprintf(logFile, "%u: %s - 0x%016llX\n",
                    frames - i - 1, symbol->Name, symbol->Address);
            }
        }
        free(symbol);
    }
    SymCleanup(hProcess);
}

static __forceinline void WriteStackTraceW(FILE* logFile) {
    HANDLE hProcess = GetCurrentProcess();
    SymInitialize(hProcess, NULL, TRUE);

    void* stack[MAX_STACK_FRAMES];
    USHORT frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, stack, NULL);

    SYMBOL_INFOW* symbol = (SYMBOL_INFOW*)calloc(1, sizeof(SYMBOL_INFOW) + NAME_BUFFER_SIZE * sizeof(WCHAR));
    if (symbol) {
        symbol->MaxNameLen = NAME_BUFFER_SIZE - 1;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFOW);

        for (USHORT i = 0; i < frames; i++) {
            DWORD64 address = (DWORD64)stack[i];
            if (!SymFromAddrW(hProcess, address, NULL, symbol)) {
                DWORD error = GetLastError();
                fwprintf(logFile, L"%u: Unable to retrieve symbol for address 0x%016llX (Error: 0x%08X)\n",
                    frames - i - 1, address, error);
            }
            else {
                fwprintf(logFile, L"%u: %s - 0x%016llX\n",
                    frames - i - 1, symbol->Name, symbol->Address);
            }
        }
        free(symbol);
    }
    SymCleanup(hProcess);
}

static __forceinline void writeLogA(const char* errorMessage) {
    FILE* logFile = fopen("log.txt", "a");
    if (logFile) {
        fputs(errorMessage, logFile);
        fputc('\n', logFile);

        CONTEXT_TYPE context;
        RtlCaptureContext(&context);
        WriteRegistersToLogA(logFile, &context);
        WriteFlagsToLogA(logFile, &context);
        WriteThreadAndProcessInfoToLogA(logFile);
        WriteStackTraceA(logFile);
        fclose(logFile);
    }
}

static __forceinline void writeLogW(const wchar_t* errorMessage) {
    FILE* logFile = _wfopen(L"log.txt", L"a");
    if (logFile) {
        fputws(errorMessage, logFile);
        fputwc(L'\n', logFile);

        CONTEXT_TYPE context;
        RtlCaptureContext(&context);
        WriteRegistersToLogW(logFile, &context);
        WriteFlagsToLogW(logFile, &context);
        WriteThreadAndProcessInfoToLogW(logFile);
        WriteStackTraceW(logFile);
        fclose(logFile);
    }
}

static __forceinline void handleErrorLogA(const char* file, int line, HRESULT hr, const char* message, ...) {
    char errorMessage[MAX_MESSAGE_LENGTH];
    va_list args;
    va_start(args, message);
    vsnprintf(errorMessage, sizeof(errorMessage), message, args);
    va_end(args);

    char finalMessage[MAX_FINAL_MESSAGE_LENGTH];
    snprintf(finalMessage, sizeof(finalMessage),
        "File: %s\nLine: %d\nHRESULT: 0x%08X\nError System String: %s\nError System Description: %s\nMessage: %s\n",
        file, line, hr, DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr), errorMessage);

    writeLogA(finalMessage);
}

static __forceinline void handleErrorLogW(const wchar_t* file, int line, HRESULT hr, const wchar_t* message, ...) {
    wchar_t errorMessage[MAX_MESSAGE_LENGTH];
    va_list args;
    va_start(args, message);
    _vsnwprintf(errorMessage, sizeof(errorMessage) / sizeof(wchar_t), message, args);
    va_end(args);

    wchar_t finalMessage[MAX_FINAL_MESSAGE_LENGTH];
    _snwprintf(finalMessage, sizeof(finalMessage) / sizeof(wchar_t),
        L"File: %s\nLine: %d\nHRESULT: 0x%08X\nError System String: %s\nError System Description: %s\nMessage: %s\n",
        file, line, hr, DXGetErrorStringW(hr), DXGetErrorDescriptionW(hr), errorMessage);

    writeLogW(finalMessage);
}

static __forceinline void handleErrorLogAndShowA(const char* file, int line, HRESULT hr, const char* message, ...) {
    char errorMessage[MAX_MESSAGE_LENGTH];
    va_list args;
    va_start(args, message);
    vsnprintf(errorMessage, sizeof(errorMessage), message, args);
    va_end(args);

    char finalMessage[MAX_FINAL_MESSAGE_LENGTH];
    snprintf(finalMessage, sizeof(finalMessage),
        "File: %s\nLine: %d\nHRESULT: 0x%08X\nError System String: %s\nError System Description: %s\nMessage: %s\n",
        file, line, hr, DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr), errorMessage);

    writeLogA(finalMessage);
    fprintf(stderr, "%s", finalMessage);
    MessageBoxA(NULL, finalMessage, "Error", MB_OK | MB_ICONERROR);
    __debugbreak();
}

static __forceinline void handleErrorLogAndShowW(const wchar_t* file, int line, HRESULT hr, const wchar_t* message, ...) {
    wchar_t errorMessage[MAX_MESSAGE_LENGTH];
    va_list args;
    va_start(args, message);
    _vsnwprintf(errorMessage, sizeof(errorMessage) / sizeof(wchar_t), message, args);
    va_end(args);

    wchar_t finalMessage[MAX_FINAL_MESSAGE_LENGTH];
    _snwprintf(finalMessage, sizeof(finalMessage) / sizeof(wchar_t),
        L"File: %s\nLine: %d\nHRESULT: 0x%08X\nError System String: %s\nError System Description: %s\nMessage: %s\n",
        file, line, hr, DXGetErrorStringW(hr), DXGetErrorDescriptionW(hr), errorMessage);

    writeLogW(finalMessage);
    fwprintf(stderr, L"%s", finalMessage);
    MessageBoxW(NULL, finalMessage, L"Error", MB_OK | MB_ICONERROR);
    __debugbreak();
}

#pragma warning(pop)