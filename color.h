#ifndef COLOR_H_INCLUDED
    #define COLOR_H_INCLUDED
#endif // COLOR_H_INCLUDED

#include <stdio.h>

#ifdef __MINGW32__
    #define NEED_COLOR_FIX
#endif

#ifdef NEED_COLOR_FIX
    #include <windows.h>
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define CON_RESET "\033[0m"
#define COLOR_BLUE_U "\033[4;36m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BELLO "\033[0;96m"

void Color();
void Color() {
#ifdef NEED_COLOR_FIX
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(handle, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(handle, mode);
        }
    }
#endif

    puts(COLOR_BLUE_U "I am blue underlined text" CON_RESET);
    puts(COLOR_GREEN "I am green text" CON_RESET);
    puts(COLOR_BELLO "I am \"colore bello\" text" CON_RESET);
}
