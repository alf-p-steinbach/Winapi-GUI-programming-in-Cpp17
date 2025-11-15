#ifndef _MSC_VER
#   error "This file is for Visual C++ only."
#endif

#include <windows.h>        // GetStdHandle, GetFileType
#include <process.h>        // _set_app_type, _crt_...

const HANDLE    error_stream_handle = GetStdHandle( STD_ERROR_HANDLE );
const bool      has_error_stream    = (GetFileType( error_stream_handle ) != FILE_TYPE_UNKNOWN);

static auto fix_it() -> bool
{
    const auto  use_text_assertions = _crt_console_app;
    const auto  use_gui_assertions  = _crt_gui_app;
    _set_app_type( has_error_stream? use_text_assertions : use_gui_assertions );
    return true;
}

const bool dummy = fix_it();
