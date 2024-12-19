#pragma once
#include "framework.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DXUT_ERR(str, hr, ...)          handleErrorLog(__FILE__, __LINE__, hr, str, __VA_ARGS__)
#define DXUT_ERR_MSGBOX(str, hr, ...)   handleErrorLogAndShow(__FILE__, __LINE__, hr, str, __VA_ARGS__)
#else
#define DXUT_ERR(str, hr, ...)          (hr)
#define DXUT_ERR_MSGBOX(str, hr, ...)   (hr)
#endif

#if defined(_WIN64)
#define CONTEXT_TYPE CONTEXT
#define RtlCaptureContext RtlCaptureContext
#define SymFromAddr SymFromAddr
#define CaptureStackBackTrace RtlCaptureStackBackTrace
#else
#define CONTEXT_TYPE CONTEXT
#define RtlCaptureContext RtlCaptureContext
#define SymFromAddr SymFromAddr
#define CaptureStackBackTrace CaptureStackBackTrace
#endif

#pragma warning(push)
#pragma warning(disable: 6328)
__forceinline void WriteRegistersToLog(FILE* logFile, CONTEXT_TYPE* context) {
    fprintf(logFile, "Registers:\n");
#if defined(_WIN64)
    fprintf(logFile, "    RAX: 0x%016llX, RBX: 0x%016llX, RCX: 0x%016llX, RDX: 0x%016llX\n",
        context->Rax, context->Rbx, context->Rcx, context->Rdx);
    fprintf(logFile, "    RSI: 0x%016llX, RDI: 0x%016llX, RBP: 0x%016llX, RSP: 0x%016llX\n",
        context->Rsi, context->Rdi, context->Rbp, context->Rsp);
    fprintf(logFile, "    RIP: 0x%016llX\n", context->Rip);
#else
    fprintf(logFile, "    EAX: 0x%08X, EBX: 0x%08X, ECX: 0x%08X, EDX: 0x%08X\n",
        context->Eax, context->Ebx, context->Ecx, context->Edx);
    fprintf(logFile, "    ESI: 0x%08X, EDI: 0x%08X, EBP: 0x%08X, ESP: 0x%08X\n",
        context->Esi, context->Edi, context->Ebp, context->Esp);
    fprintf(logFile, "    EIP: 0x%08X\n", context->Eip);
#endif
    fprintf(logFile, "\n");
}

__forceinline void WriteFlagsToLog(FILE* logFile, CONTEXT_TYPE* context) {
    fprintf(logFile, "Flags:\n");
#if defined(_WIN64)
    fprintf(logFile, "    RFLAGS: 0x%016llX\n", context->EFlags);
#else
    fprintf(logFile, "    EFLAGS: 0x%08X\n", context->EFlags);
#endif
    fprintf(logFile, "\n");
}

__forceinline void WriteThreadAndProcessInfoToLog(FILE* logFile) {
    HANDLE hThread = GetCurrentThread();
    HANDLE hProcess = GetCurrentProcess();

    fprintf(logFile, "Thread and Process Information:\n");
    fprintf(logFile, "    Current Thread ID: %lu\n", GetThreadId(hThread));
    fprintf(logFile, "    Current Process ID: %lu\n", GetProcessId(hProcess));
    fprintf(logFile, "\n");
}

__forceinline void writeLog(const char* errorMessage) {
    FILE* logFile = fopen("log.txt", "a");
    if (logFile) {
        fputs(errorMessage, logFile);
        fprintf(logFile, "\n");

        CONTEXT_TYPE context;
        RtlCaptureContext(&context);
        WriteRegistersToLog(logFile, &context);
        WriteFlagsToLog(logFile, &context);
        WriteThreadAndProcessInfoToLog(logFile);

        HANDLE hProcess = GetCurrentProcess();
        SymInitialize(hProcess, NULL, TRUE);

        void* stack[64];
        unsigned short frames = CaptureStackBackTrace(0, 64, stack, NULL);
        const DWORD NAME_BUFFER_SIZE = 256;
        DWORD nameBufferSize = NAME_BUFFER_SIZE * sizeof(char);
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(1, sizeof(SYMBOL_INFO) + nameBufferSize);
        if (symbol) {
            symbol->MaxNameLen = NAME_BUFFER_SIZE - 1;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

            for (unsigned int i = 0; i < frames; i++) {
                DWORD64 address = (DWORD64)stack[i];
                if (!SymFromAddr(hProcess, address, NULL, symbol)) {
                    DWORD error = GetLastError();
                    fprintf(logFile, "%u: Unable to retrieve symbol for address 0x%016llX (Error: 0x%08X)\n", frames - i - 1, address, error);
                }
                else
                    fprintf(logFile, "%u: %s - 0x%016llX\n", frames - i - 1, symbol->Name, symbol->Address);
            }
            free(symbol);
        }

        SymCleanup(hProcess);
        fclose(logFile);
    }
}

__forceinline void handleErrorLog(const char* file, int line, HRESULT hr, const char* message, ...) {
    char errorMessage[1024];
    va_list args;
    va_start(args, message);
    vsnprintf(errorMessage, sizeof(errorMessage), message, args);
    va_end(args);

    char finalMessage[2048];
    const char* errorString = DXGetErrorString(hr);
    const char* errorDescription = DXGetErrorDescription(hr);
    snprintf(finalMessage, sizeof(finalMessage), "File: %s\nLine: %d\nHRESULT: 0x%08X\nError System String: %s\nError System Description: %s\nMessage: %s\n",
        file, line, hr, errorString, errorDescription, errorMessage);
    writeLog(finalMessage);
}

__forceinline void handleErrorLogAndShow(const char* file, int line, HRESULT hr, const char* message, ...) {
    char errorMessage[1024];
    va_list args;
    va_start(args, message);
    vsnprintf(errorMessage, sizeof(errorMessage), message, args);
    va_end(args);

    char finalMessage[2048];
    const char* errorString = DXGetErrorString(hr);
    const char* errorDescription = DXGetErrorDescription(hr);
    snprintf(finalMessage, sizeof(finalMessage), "File: %s\nLine: %d\nHRESULT: 0x%08X\nError System String: %s\nError System Description: %s\nMessage: %s\n",
        file, line, hr, errorString, errorDescription, errorMessage);

    writeLog(finalMessage);

    fprintf(stderr, "%s", finalMessage);
    MessageBoxA(NULL, finalMessage, "Error", MB_OK | MB_ICONERROR);
    __debugbreak();
}
#pragma warning(pop)