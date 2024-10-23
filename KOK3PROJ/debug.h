#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <stdlib.h>

static void message_handle(const char* file, const char* function, int line, const char* message, ...) {
    va_list args;
    va_start(args, message);

    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(NULL, 0, message, args_copy);
    va_end(args_copy);

    if (length > 0) {
        char* buffer = (char*)malloc((static_cast<size_t>(length) + 1) * sizeof(char));
        if (buffer) {
            vsnprintf(buffer, static_cast<size_t>(length) + 1, message, args);
            char full_message[1024];
            snprintf(full_message, sizeof(full_message), "File: %s\nFunction: %s\nLine: %d\n\n%s", file, function, line, buffer);

            MessageBox(NULL, full_message, "Error", MB_OK | MB_ICONERROR);

#ifdef _DEBUG
            OutputDebugString(buffer);
            __debugbreak();
#endif
            free(buffer);
        }
    }

    va_end(args);
}

#define ERROR_MSG(message, ...) message_handle(__FILE__, __FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define CHECK_ASSERT(predicate, message, ...) if (!(predicate)) ERROR_MSG(message, ##__VA_ARGS__)