#include <windows.h>

auto WINAPI wWinMain(
    HINSTANCE           h_instance,               // Value from `GetModuleHandle( 0 )`
    HINSTANCE           _dummy,                   // 0
    wchar_t*            cmd_arguments_part,       // Value from `GetCommandLineW` + skip cmd verb.
    int                 initial_show_state        // Value from `GetStartupInfoW` + `.wShowWindow`
    ) -> int
{
    return 0;   // No special support for default return value, unlike standard `main`.
}
