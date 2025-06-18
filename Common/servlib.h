#pragma once

#ifdef _WIN64
#define SERVLIBCALL /*__fastcall*/
#else
#define SERVLIBCALL __fastcall
#endif

class servlib {

};

