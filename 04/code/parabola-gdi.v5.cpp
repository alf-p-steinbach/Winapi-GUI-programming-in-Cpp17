#ifndef UNICODE
#   error "Please define UNICODE in the build (this is a wide function based program)."
#   include <terminate-compilation>     // Workaround for “must continue anyway!” g++.
#endif
#include <windows.h>

#include <initializer_list> // Formally required for initializer-list in range based `for`.
#include <iterator>
#include <vector>

#include <cassert>          // assert
#include <cmath>
#include <cstdlib>          // EXIT_FAILURE

namespace cppm {                // "C++ machinery"
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

namespace geometry{
    using   cppm::in_;

    struct Handedness{ enum Enum: int { like_math, opposite_math }; };

    // Template parameter `Point` should be like `struct Point{ int x; int y; }`.
    template< class Point, Handedness::Enum handedness = Handedness::like_math >
    class Point_vector_
    {
        Point   m_pt;

        friend auto math_rotl( in_<Point_vector_> vec )
            -> Point_vector_
        { return {-vec.y(), vec.x()}; }

        friend auto math_rotr( in_<Point_vector_> vec )
            -> Point_vector_
        { return {vec.y(), -vec.x()}; }

    public:
        Point_vector_(): m_pt() {}
        Point_vector_( in_<Point> pt ): m_pt( pt ) {}
        Point_vector_( const int x, const int y ): m_pt{ x, y } {}

        auto x() const -> int { return m_pt.x; }
        auto y() const -> int { return m_pt.y; }

        operator Point () const { return m_pt; }

        friend auto operator*( const int n, in_<Point_vector_> vec )
            -> Point_vector_
        { return {n*vec.x(), n*vec.y()}; }

        friend auto operator+( in_<Point> pt, in_<Point_vector_> vec )
            -> Point
        { return {pt.x + vec.x(), pt.y + vec.y()}; }

        friend auto operator-( in_<Point> pt, in_<Point_vector_> vec )
            -> Point
        { return {pt.x - vec.x(), pt.y - vec.y()}; }

        friend auto rotl( in_<Point_vector_> vec )
            -> Point_vector_
        { return (handedness == Handedness::like_math? math_rotl( vec ) : math_rotr( vec )); }

        friend auto rotr( in_<Point_vector_> vec )
            -> Point_vector_
        { return (handedness == Handedness::like_math? math_rotr( vec ) : math_rotl( vec )); }
    };
}  // geometry

namespace winapi {
    using   cppm::in_;

    auto client_rect_of( const HWND window )
        -> RECT
    {
        RECT r;
        GetClientRect( window, &r );
        return r;
    }

    auto extent_of( in_<RECT> r ) -> SIZE { return {r.right - r.left, r.bottom - r.top}; }

    void draw_line_sans_endpoint( const HDC dc, in_<POINT> from, in_<POINT> to )
    {
        const POINT points[] = {from, to};
        Polyline( dc, points, 2 );
    }

    void set_px( const HDC dc, in_<POINT> where )
    {
        // With a fancy pen this can conceivably be imperfect.
        draw_line_sans_endpoint( dc, where, {where.x + 1, where.y} );
    }

    void draw_line( const HDC dc, in_<POINT> from, in_<POINT> to )
    {
        draw_line_sans_endpoint( dc, from, to );
        set_px( dc, to );
    }
}  // winapi

namespace app {
    using   cppm::Nat, cppm::in_, cppm::Process_exit_code, cppm::sign_of;

    using   geometry::Handedness, geometry::Point_vector_;

    using   std::vector;            // <vector>

    using   std::trunc;             // <cmath>

    const auto& window_class_name   = L"Main window";
    const auto& window_title        = L"Parabola (x²/4) — graph by 日本国 кошка, v5";

    auto f( const double x ) -> double { return x*x/4; }

    using Math_point        = struct{ double x; double y; };
    using Px_point          = POINT;                // Pixel location.
    using Px_point_vector   = Point_vector_<Px_point, Handedness::opposite_math>;

    enum class Px_index: int {};                    // Pixel indexing for a math axis.
    void operator++( Px_index& v ) { v = Px_index( int( v ) + 1 ); }
    auto value_before( Px_index v ) -> Px_index { return Px_index( int( v ) - 1 ); }
    auto operator<=( Px_index a, Px_index b ) -> bool { return int( a ) <= int( b ); }

    class Coordinate_transformation
    {
        // Scaling so that e.g. math x = -15 maps to px row -150.
        static constexpr double     scaling         = 10;
        static constexpr double     minimum_y       = -2.0; // In display’s left edge.
        static constexpr Nat        i_px_col_y_zero = int( scaling*( 0.0 - minimum_y ) );

        const Nat   m_w;
        const Nat   m_h;
        const Nat   m_i_px_row_middle;

    public:
        Coordinate_transformation( const Nat w, const Nat h ):
            m_w( w ),
            m_h( h ),
            m_i_px_row_middle( h/2 )
        {
            assert( m_w >= 0 );
            assert( m_h >= 0 );
        }

        explicit Coordinate_transformation( in_<SIZE> size ):
            Coordinate_transformation( size.cx, size.cy )
        {}

        auto px_w() const -> Nat { return m_w; }
        auto px_h() const -> Nat { return m_h; }

        auto px_unit_for_math_x() const -> Px_point_vector { return {0, 1}; }         // ↓
        auto px_unit_for_math_y() const -> Px_point_vector { return {1, 0}; }         // →

        auto px_index_from_math_x( const double x ) const
            -> Px_index
        { return Px_index( m_i_px_row_middle + int( scaling*x ) ); }

        auto px_index_from_math_y( const double y ) const
            -> Px_index
        { return Px_index( i_px_col_y_zero + int( scaling*y ) ); }

        auto math_x_from( const Px_index i_px ) const
            -> double
        {
            const int relative_row_index = int( i_px ) - m_i_px_row_middle;
            return 1.0*relative_row_index/scaling;
        }

        auto px_pt_from_indices( const Px_index i_px_for_x, const Px_index i_px_for_y ) const
            -> Px_point
        { return {int( i_px_for_y ), int( i_px_for_x )}; }

        auto px_pt_from( in_<Math_point> math ) const
            -> Px_point
        {
            const auto px_x = int( px_index_from_math_y( math.y ) );
            const auto px_y = int( px_index_from_math_x( math.x ) );
            return {px_x, px_y};
        }

        auto math_minimum_x() const -> double { return -m_i_px_row_middle/scaling; }
        auto math_maximum_x() const -> double { return math_minimum_x() + m_h/scaling; }

        auto math_minimum_y() const -> double { return minimum_y; }
        auto math_maximum_y() const -> double { return math_minimum_y() + m_w/scaling; }
    };

    class Coordinate_transformation_with_parameterization_on_axes:
        public Coordinate_transformation
    {
        using Base = Coordinate_transformation;

    public:
        struct Math_axis{ enum Enum: int { x, y }; };
        static constexpr Math_axis::Enum math_axes[] = { Math_axis::x, Math_axis::y };

        using Base::Coordinate_transformation;      // Constructors.

        auto px_unit_vector_for( const Math_axis::Enum axis ) const
            -> Px_point_vector
        { return (axis == Math_axis::x? px_unit_for_math_x() : px_unit_for_math_y()); }

        using Base::px_pt_from;

        auto px_pt_from( const Math_axis::Enum axis, const double v ) const
            -> Px_point
        { return (axis == Math_axis::x? px_pt_from( {v, 0} ) : px_pt_from( {0, v} ) ); }

        auto math_minimum( const Math_axis::Enum axis ) const
            -> double
        { return (axis == Math_axis::x? math_minimum_x() : math_minimum_y()); }

        auto math_maximum( const Math_axis::Enum axis ) const
            -> double
        { return (axis == Math_axis::x? math_maximum_x() : math_maximum_y()); }

        auto px_i_first( const Math_axis::Enum ) const
            -> Px_index
        { return Px_index( 0 ); }

        auto px_i_beyond( const Math_axis::Enum axis ) const
            -> Px_index
        { return Px_index( axis == Math_axis::x? px_h() : px_w() ); }
    };

    class Painter
    {
        using Ct = Coordinate_transformation_with_parameterization_on_axes;

        const HDC   m_dc;
        const Ct    m_xform;

        inline void draw_math_axis( const Ct::Math_axis::Enum axis ) const;

        inline void add_math_axis_ticks( const Ct::Math_axis::Enum axis, const Nat tick_distance ) const;

        inline void plot_the_parabola() const;
        
        inline void add_markers_on_the_graph() const;
        
        void draw_axes_with_ticks() const
        {
            for( const auto axis: Ct::math_axes ) { draw_math_axis( axis ); }
            for( const auto axis: Ct::math_axes ) { add_math_axis_ticks( axis, 5 ); }
        }

    public:
        Painter( const HDC dc, in_<SIZE> client_area_size ):
            m_dc( dc ),
            m_xform( client_area_size )
        {}

        void paint() const
        {
            // Display the math x and y axes first to make the graph appear to be “above”.
            draw_axes_with_ticks();
            plot_the_parabola();
            add_markers_on_the_graph();
        }
    };

    void Painter::draw_math_axis( const Ct::Math_axis::Enum axis ) const
    {
        const auto& _ = m_xform;
        const double    first_v     = _.math_minimum( axis );
        const double    last_v      = _.math_maximum( axis );
        winapi::draw_line( m_dc, _.px_pt_from( axis, first_v ), _.px_pt_from( axis, last_v ) );
    }

    void Painter::add_math_axis_ticks( const Ct::Math_axis::Enum axis, const Nat tick_distance ) const
    {
        const auto& _ = m_xform;
        const Px_point_vector   tick_extent     = 2*rotl( _.px_unit_vector_for( axis ) );
        const Nat               td              = tick_distance;

        const double    min_marker_value    = td*trunc( _.math_minimum( axis )/td );
        const double    max_marker_value    = td*trunc( _.math_maximum( axis )/td );

        // Add ticks on the math y-axis for every td math units. Note: looping over integer values.
        for( double value = min_marker_value; value <= max_marker_value; value += td ) {
            const Px_point pt = _.px_pt_from( axis, value );
            winapi::draw_line( m_dc, pt - tick_extent, pt + tick_extent );
        }
    }

    void Painter::plot_the_parabola() const
    {
        // The graph is plotted to just outside the client area.
        const auto& _ = m_xform;
        constexpr auto x_axis = Ct::Math_axis::x;

        const Px_index      i_px_first      = _.px_i_first( x_axis );
        const Px_index      i_px_beyond     = _.px_i_beyond( x_axis );
        const auto          n_px_indices    = int( i_px_beyond );

        assert( int( i_px_first ) == 0 );
        assert( int( i_px_beyond ) > 0 );

        auto points = vector<POINT>( n_px_indices + 2 );    // 2 extra indices for plotting to outside.
        for(    Px_index    i_px_for_x      = value_before( i_px_first );
                i_px_for_x <= i_px_beyond;
                ++i_px_for_x
                ) {
            const double        x           = _.math_x_from( i_px_for_x );
            const double        y           = f( x );
            const Px_index      i_px_for_y  = _.px_index_from_math_y( y );

            points[int( i_px_for_x ) + 1] = _.px_pt_from_indices( i_px_for_x, i_px_for_y );
        }
        Polyline( m_dc, points.data(), int( points.size() ) );
    }

    void Painter::add_markers_on_the_graph() const
    {
        // Add markers on the graph for every 5 math units of math x axis.
        const auto& _ = m_xform;
        const Nat td = 5;
        static const auto black_brush = static_cast<HBRUSH>( GetStockObject( BLACK_BRUSH ) );
        
        const double    min_marker_x    = td*trunc( _.math_minimum_x()/td );
        const double    max_marker_x    = td*trunc( _.math_maximum_x()/td );

        // Note: looping over integer values.
        for( double x = min_marker_x; x <= max_marker_x; x += td ) {
            const double y = f( x );
            const Px_point pt = _.px_pt_from( {x, y} );
            const auto square_marker_rect = RECT{ pt.x - 2, pt.y - 2, pt.x + 3, pt.y + 3 };
            FillRect( m_dc, &square_marker_rect, black_brush );
        }
    }

    void paint( const HWND window, const HDC dc )
    {
        const SIZE client_area_size = winapi::extent_of( winapi::client_rect_of( window ) );
        Painter( dc, client_area_size ).paint();
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
