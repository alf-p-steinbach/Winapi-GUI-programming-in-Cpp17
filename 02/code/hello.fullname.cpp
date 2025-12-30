// A full name greeting GUI program.
#include <string>
using   std::wstring;           // <string>

#include <windows.h>

#define SECURITY_WIN32          // This must be defined before the include; no default.
#include <security.h>           // GetUserNameExW

auto full_username() -> wstring
{
    auto result = wstring( 123, L'\0' );    // Probably sufficient size.
    auto n = ULONG( result.size() );        // Buffer size on call, but n characters on return.

    using Format_enum = EXTENDED_NAME_FORMAT;
    const bool success = GetUserNameExW( Format_enum::NameDisplay, result.data(), &n );

    if( success ) { result.resize( n ); } else { result = L"<unknown>"; }
    return result;
}

auto main() -> int
{
    using Wide_c_str = const wchar_t*;

    const wstring       title   = wstring() + L"Hello " + full_username() + L"…";
    const Wide_c_str    text    = L"Greetings from some API level C++ code!";
    const UINT          options = MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND;

    MessageBoxW( 0, text, title.c_str(), options );
}
