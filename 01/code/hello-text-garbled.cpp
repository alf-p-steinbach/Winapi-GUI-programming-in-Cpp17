// An example of usually garbled text with `MessageBoxA`.
// However, this will present the text correctly if you have configured Windows to use UTF-8
// globally (beta feature). Ditto if you have configured UTF-8 as the /OEM/ codepage (!).

#include <windows.h>
#include <string>
using   std::string, std::to_string;

using Byte = unsigned char;
using C_str = const char*;

auto main() -> int
{
    const C_str     title   = "Hello, world!";
    const C_str     i10n    = "Every 日本国 кошка likes Norwegian blåbærsyltetøy!";
    const UINT      options = MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND;

    string text;
    text += string() + "ANSI CP = " + to_string( GetACP() ) + '\n';
    text += '\n';
    text += i10n;

    MessageBoxA( 0, text.c_str(), title, options );
}
