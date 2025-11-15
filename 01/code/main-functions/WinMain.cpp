#include <windows.h>

auto WINAPI WinMain(
    HINSTANCE           h_instance,               // Value from `GetModuleHandle( 0 )`
    HINSTANCE           _dummy,                   // 0
    char*               cmd_arguments_part,       // Value from `GetCommandLineA` + skip cmd verb.
    int                 initial_show_state        // Value from `GetStartupInfoW` + `.wShowWindow`
    ) -> int
{
    return 0;   // No special support for default return value, unlike standard `main`.
}
