#ifndef UNICODE
#   error "Please define UNICODE in the build (this is a wide function based program)."
#   include <terminate-compilation>     // Workaround for “must continue anyway!” g++.
#endif
#include <windows.h>

#include <initializer_list> // Formally required for initializer-list in range based `for`.
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
}  // cppm

namespace winapi {
    auto client_rect_of( const HWND window )
        -> RECT
    {
        RECT r;
        GetClientRect( window, &r );
        return r;
    }
}  // winapi

namespace app {
    using   cppm::Nat, cppm::Process_exit_code, cppm::sign_of;

    using   std::vector;            // <vector>

    const auto& window_class_name   = L"Main window";
    const auto& window_title        = L"Parabola (x²/4) — graph by 日本国 кошка, v3";

    auto f( const double x ) -> double { return x*x/4; }

    void paint( const HWND window, const HDC dc )
    {
        static constexpr COLORREF black = RGB( 0, 0, 0 );
        static const auto black_brush = static_cast<HBRUSH>( GetStockObject( BLACK_BRUSH ) );

        const RECT  r   = winapi::client_rect_of( window );
        const Nat   h   = r.bottom - r.top;     // r.top is always 0 for a client rect, but.

        const Nat   i_pixel_row_middle = h/2;

        const double    scaling     = 10;           // So e.g. math x = -15 maps to pixel row -150.

        // Plot the parabola.
        auto points = vector<POINT>( h );
        for( Nat i_pixel_row = 0; i_pixel_row < h; ++i_pixel_row ) {
            const int       relative_row_index  = i_pixel_row - i_pixel_row_middle;
            const double    x                   = 1.0*relative_row_index/scaling;
            const double    y                   = f( x );
            const int       i_pixel_col         = int( scaling*y );

            points[i_pixel_row] = POINT{ i_pixel_col, i_pixel_row };
        }
        Polyline( dc, points.data(), int( points.size() ) );

        // Add markers for every 5 math units of math x axis.
        const Nat       max_int_x_magnitude         = Nat( i_pixel_row_middle/scaling );
        const double    max_marker_x_magnitude      = 5*(max_int_x_magnitude/5);    // Symmetrical.
        for( double x = -max_marker_x_magnitude; x <= max_marker_x_magnitude; x += 5 ) {
            const double    y               = f( x );
            const int       i_pixel_row     = i_pixel_row_middle + int( scaling*x );
            const int       i_pixel_col     = int( scaling*y );

            const auto square_marker_rect = RECT{
                i_pixel_col - 2, i_pixel_row - 2, i_pixel_col + 3, i_pixel_row + 3
                };
            FillRect( dc, &square_marker_rect, black_brush );
        }
    }

    void on_wm_destroy( const HWND window )
    {
        // The window is being destroyed. Terminate the message loop to avoid a hang:
        (void) window;      // Unused.
        PostQuitMessage( Process_exit_code::success );
    }

    void on_wm_paint( const HWND window )
    {
        PAINTSTRUCT     info = {};          // Primarily a dc and an update rectangle.

        const HDC dc = BeginPaint( window, &info );
        if( dc ) { paint( window, dc ); }
        EndPaint( window, &info );
    }

    void on_wm_size( const HWND window )
    {
        InvalidateRect( window, nullptr, true );    // `true` ⇨ let `BeginPaint` erase background.
    }

    auto CALLBACK window_proc(
        const HWND          window,
        const UINT          msg_id,         // Can be e.g. `WM_COMMAND`, `WM_SIZE`, ...
        const WPARAM        w_param,        // Meaning depends on the `msg_id`.
        const LPARAM        ell_param       // Meaning depends on the `msg_id`.
        ) -> LRESULT
    {
        switch( msg_id ) {
            case WM_DESTROY:    { on_wm_destroy( window );  return 0; }
            case WM_PAINT:      { on_wm_paint( window );  return 0; }
            case WM_SIZE:       { on_wm_size( window );  return 0; }
        }
        return DefWindowProc( window, msg_id, w_param, ell_param );     // Default handling.
    }

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
            window_title,
            WS_OVERLAPPEDWINDOW,                        // Resizable and has a title bar.
            CW_USEDEFAULT, CW_USEDEFAULT, 640, 400,     // x y w h
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
