#ifndef UNICODE
#   error "Please define UNICODE in the build (this is a wide function based program)."
#   include <terminate-compilation>     // Workaround for “must continue anyway!” g++.
#endif
#include <windows.h>

#include <cassert>          // assert
#include <cstdlib>          // EXIT_FAILURE

enum Process_exit_code: int { success = 0, failure = EXIT_FAILURE };

template< class T >
constexpr auto sign_of( const T& v ) noexcept -> int { return (v > 0) - (v < 0); }

auto CALLBACK window_proc(
    const HWND          window,
    const UINT          msg_id,         // Can be e.g. `WM_COMMAND`, `WM_SIZE`, ...
    const WPARAM        w_param,        // Meaning depends on the `msg_id`.
    const LPARAM        ell_param       // Meaning depends on the `msg_id`.
    ) -> LRESULT
{
    if( msg_id == WM_DESTROY ) {
        // The window is being destroyed. Terminate the message loop to avoid a hang:
        PostQuitMessage( Process_exit_code::success );
        return 0;
    }
    return DefWindowProc( window, msg_id, w_param, ell_param );     // Default handling.
}

const auto& window_class_name = L"A basic empty main window";

auto make_window_class_params()
    -> WNDCLASS
{
    WNDCLASS params = {};
    params.lpfnWndProc      = &window_proc;
    params.hInstance        = GetModuleHandle( 0 );         // Not very useful in modern code.
    params.hIcon            = LoadIcon( 0, IDI_APPLICATION );
    params.hCursor          = LoadCursor( 0, IDC_ARROW );
    params.hbrBackground    = CreateSolidBrush( RGB( 0xFF, 0x80, 0x00 ) );          // Orange.
    params.lpszClassName    = window_class_name;
    return params;
};

auto run()
    -> Process_exit_code
{
    const WNDCLASS window_class_params = make_window_class_params();
    RegisterClass( &window_class_params );

    const HWND window = CreateWindow(
        window_class_name,
        L"My first general Windows API window!",
        WS_OVERLAPPEDWINDOW,                        // Resizable and has a title bar.
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 280,     // x y w h
        HWND(),                                     // Owner window; none.
        HMENU(),                                    // Menu handle or child window id.
        GetModuleHandle( 0 ),                       // Not very useful in modern code.
        nullptr                                     // Custom parameter for app’s use.
        );
    if( not window ) {
        return Process_exit_code::failure;          // Avoid hanging in the event loop.
    }

    ShowWindow( window, SW_SHOWDEFAULT );           // Displays the window.

    // Event loop a.k.a. message loop:
    for( ;; ) {
        MSG msg;
        switch( sign_of( GetMessage( &msg, 0, 0, 0 ) ) ) {
            case +1: {
                TranslateMessage( &msg );           // Provides e.g. Alt+Space sysmenu shortcut.
                DispatchMessage( &msg );            // Calls the window proc of relevant window.
                continue;
            }
            case 0: {
                assert( msg.message == WM_QUIT );
                return Process_exit_code( msg.wParam );
            }
            case -1: {
                return Process_exit_code::failure;
            }
        }
    }
}

auto main() -> int { return run(); }
