#include <windows.h>

#include <cassert>          // assert
#include <cstdlib>          // EXIT_FAILURE

enum Process_exit_code: int { success = 0, failure = EXIT_FAILURE };

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
    return DefWindowProcW( window, msg_id, w_param, ell_param );    // Default handling.
}

const auto& window_class_name = L"A basic empty main window";

auto make_window_class_params()
    -> WNDCLASSW
{
    WNDCLASSW params = {};
    params.lpfnWndProc      = &window_proc;
    params.hInstance        = GetModuleHandleW( 0 );        // Not very useful in modern code.
    params.hIcon            = LoadIcon( 0, IDI_APPLICATION );   // ANSI or Wide, either is OK.
    params.hCursor          = LoadCursor( 0, IDC_ARROW );       // ANSI or Wide, either is OK.
    params.hbrBackground    = CreateSolidBrush( RGB( 0xFF, 0x80, 0x00 ) );          // Orange.
    params.lpszClassName    = window_class_name;
    return params;
};

auto run()
    -> Process_exit_code
{
    const WNDCLASSW window_class_params = make_window_class_params();
    RegisterClassW( &window_class_params );

    const HWND window = CreateWindowW(
        window_class_name,
        L"My first general Windows API window!",
        WS_OVERLAPPEDWINDOW,                        // Resizable and has a title bar.
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 280,     // x y w h
        HWND(),                                     // Owner window; none.
        HMENU(),                                    // Menu handle or child window id.
        GetModuleHandleW( 0 ),                      // Not very useful in modern code.
        nullptr                                     // Custom parameter for app’s use.
        );
    if( not window ) {
        return Process_exit_code::failure;          // Avoid hanging in the event loop.
    }

    ShowWindow( window, SW_SHOWDEFAULT );           // Displays the window.

    // Event loop a.k.a. message loop:
    MSG msg;
    while( GetMessageW( &msg, 0, 0, 0 ) ) {         // We’re ignoring that it can fail.
        TranslateMessage( &msg );           // Provides e.g. Alt+Space sysmenu shortcut.
        DispatchMessageW( &msg );           // Calls the window proc of relevant window.
    }
    assert( msg.message == WM_QUIT );
    return Process_exit_code( msg.wParam );
}

auto main() -> int { return run(); }
