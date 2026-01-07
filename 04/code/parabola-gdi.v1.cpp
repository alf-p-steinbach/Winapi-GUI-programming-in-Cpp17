#ifndef UNICODE
#   error "Please define UNICODE in the build (this is a wide function based program)."
#   include <terminate-compilation>     // Workaround for “must continue anyway!” g++.
#endif
#include <windows.h>

#include <iterator>
#include <vector>

#include <cassert>          // assert
#include <cstdlib>          // EXIT_FAILURE

namespace cppm {            // "C++ machinery"
    using   std::size;          // <iterator>

    using Nat = int;

    enum Process_exit_code: int { success = 0, failure = EXIT_FAILURE };
    
    template< class T > using in_ = const T&;       // Type of in-parameters.
    
    struct Sign{ enum Enum: int { negative = -1, zero = 0, positive = +1 }; };

    template< class T >
    constexpr auto sign_of( in_<T> v ) noexcept
        -> Sign::Enum
    { return Sign::Enum( (v > 0) - (v < 0) ); }

    template< class T >
    constexpr auto nsize( in_<T> o ) noexcept -> Nat { return Nat( size( o ) ); }
}  // cppm

using   std::wcslen;        // <cwchar>

namespace winapi {
    using   cppm::Nat, cppm::in_;

    auto client_rect_of( const HWND window )
        -> RECT
    {
        RECT r;
        GetClientRect( window, &r );
        return r;
    }

    auto width_of( in_<RECT> r )    -> Nat  { return r.right - r.left; }
    auto height_of( in_<RECT> r )   -> Nat  { return r.bottom - r.top; }
}  // winapi

namespace app {
    using   cppm::Nat, cppm::Process_exit_code, cppm::sign_of, cppm::nsize;

    using   std::vector;            // <vector>

    auto f( const double x ) -> double { return x*x/4; }

    void paint( const HWND window, const HDC dc )
    {
        static const auto& legend = L"Parabola (x²/4) — ASCII art graph by 日本国 кошка";

        const RECT  r   = winapi::client_rect_of( window );
        const Nat   h   = winapi::height_of( r );
        // const Nat   w   = winapi::width_of( r );

        const Nat   margin_left     = 20;
        const Nat   mid_row         = h/2;

        const double scaling = 10;      // So e.g. x = -15 maps to pixel row -150.
        vector<POINT> points;
        for( Nat i_row = 0; i_row < r.bottom; ++i_row ) {
            const Nat       row_number  = i_row - mid_row;
            const double    x           = 1.0*row_number/scaling;
            const double    y           = f( x );
            const Nat       i_col       = margin_left + Nat( y*scaling );
            points.push_back( POINT{ i_col, i_row } );          // (x, y) pixel coordinate.
        }
        Polyline( dc, points.data(), nsize( points ) );
    }

    void on_wm_paint( const HWND window )
    {
        PAINTSTRUCT     info = {};          // Primarily a dc and an update rectangle.

        const HDC dc = BeginPaint( window, &info );
        if( dc ) { paint( window, dc ); }
        EndPaint( window, &info );
    }

    auto CALLBACK window_proc(
        const HWND          window,
        const UINT          msg_id,         // Can be e.g. `WM_COMMAND`, `WM_SIZE`, ...
        const WPARAM        w_param,        // Meaning depends on the `msg_id`.
        const LPARAM        ell_param       // Meaning depends on the `msg_id`.
        ) -> LRESULT
    {
        switch( msg_id ) {
            case WM_DESTROY: {
                // The window is being destroyed. Terminate the message loop to avoid a hang:
                PostQuitMessage( Process_exit_code::success );
                return 0;
            }
            case WM_PAINT:  { on_wm_paint( window );  return 0; }
        }
        return DefWindowProc( window, msg_id, w_param, ell_param );     // Default handling.
    }

    const auto& window_class_name = L"Main window";

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
            L"Parabola",
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
}  // app

auto main() -> int { return app::run(); }
