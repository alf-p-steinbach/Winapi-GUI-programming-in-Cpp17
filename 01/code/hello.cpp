// A near minimal, basic Windows GUI “Hello, world!” program.
#include <windows.h>

using Wide_c_str = const wchar_t*;

auto main() -> int
{
    const Wide_c_str    title   = L"Hello, world!";
    const Wide_c_str    text    = L"Every 日本国 кошка likes Norwegian blåbærsyltetøy!";
    const UINT          options = MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND;

    MessageBoxW( 0, text, title, options );
}
