#pragma once

#ifdef _WIN64
#define SERVLIBCALL /*__fastcall*/
#else
#define SERVLIBCALL __fastcall
#endif

#define SERVLIBFINLINE __forceinline

class servlib {

};

