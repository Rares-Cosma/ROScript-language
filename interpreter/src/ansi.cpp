#include "ansi.h"

bool codes=true;

#ifdef __EMSCRIPTEN__
// WASM: do nothing
void enableANSI(bool enable) {
    (void)enable; // avoid unused parameter warning
}

#elif _WIN32
#include <windows.h>

// Enable or disable ANSI escape codes on Windows console
void enableANSI(bool enable) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    if (enable) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    } else {
        dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }

    SetConsoleMode(hOut, dwMode);
}

#else
// Other OS (Linux, macOS): ANSI usually works by default, but we can ignore 'enable'
void enableANSI(bool enable) {
    (void)enable; // do nothing
}
#endif
