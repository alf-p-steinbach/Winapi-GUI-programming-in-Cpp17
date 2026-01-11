#include <algorithm>
#include <iostream>
#include <iterator>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <cstddef>
#include <cstdlib>

namespace cppm {        // "C++ machinery"
    using   std::size;          // <iterator>

    using Nat = int;
    using Byte = unsigned char;
    using C_str = const char*;

    struct Unchecked {};

    template< class T > using const_    = const T;
    template< class T > using in_       = const T&;

    template< class T >
    constexpr auto nsize( in_<T> o ) noexcept -> Nat { return Nat( size( o ) ); }
}  // cppm

namespace u8 {
    using   cppm::Nat, cppm::Byte, cppm::C_str, cppm::Unchecked, cppm::const_, cppm::in_;
    using   std::function,          // <functional>
            std::string,            // <string>
            std::string_view;       // <string_view>

    constexpr auto is_tailbyte( const_<const char*> p_byte )
        -> bool
    { return ((Byte( *p_byte ) >> 6) == 0b10); }

    constexpr auto next_after( const_<const char*> p_first )
        -> const char*
    {
        for( const char* p = p_first + 1; ; ++p ) { if( not is_tailbyte( p ) ) {
            return p;
        } }
    }

    class Code_point
    {
        string  m_encoding;

    public:
        Code_point() {}
        Code_point( const char ch ): m_encoding{ ch } {}

        Code_point( Unchecked, const C_str p_first_byte, const C_str p_beyond ):
            m_encoding( p_first_byte, p_beyond )
        {}      // assert( p_beyond == next_after( p_first_byte ) )

        explicit Code_point( const C_str p_first_byte ):
            Code_point( Unchecked{}, p_first_byte, next_after( p_first_byte ) )
        {}

        auto sv() const -> string_view { return m_encoding; }
    };

    inline auto operator==( in_<Code_point> a, in_<Code_point> b )
        -> bool
    { return (a.sv() == b.sv()); }

    inline auto operator!=( in_<Code_point> a, in_<Code_point> b )
        -> bool
    { return (a.sv() != b.sv()); }

    using Cp_callback = void( in_<Code_point> );

    inline auto for_each_cp_in( in_<string_view> s, in_<function<Cp_callback>> callback )
    {
        const_<const char*> p_beyond = s.data() + s.size();
        for( const char* p = s.data(); p != p_beyond; ) {
            const_<const char*> p_next = next_after( p );
            callback( Code_point( Unchecked{}, p, p_next ) );   // Some premature optimization.
            p = p_next;
        }
    }

    inline auto n_cp_in( in_<string_view> s )
        -> Nat
    {
        Nat count = 0;
        for_each_cp_in( s, [&count]( in_<Code_point> ){ ++count; } );
        return count;
    }
}  // u8

namespace app {
    using   cppm::Nat, cppm::C_str, cppm::in_, cppm::nsize;

    using   std::max,               // <algorithm>
            std::cout,              // <iostream>
            std::string,            // <string>
            std::string_view,       // <string_view>
            std::vector;            // <vector>

    using   std::size_t,            // <cstddef>
            std::system;            // <cstdlib>

    auto f( const double x ) -> double { return x*x/4; }

    class Display_buffer
    {
        using Line = vector<u8::Code_point>;
        vector<Line>    m_lines;

    public:
        struct Col{ Nat value; };  struct Row{ Nat value; };

        explicit Display_buffer( const Nat n_lines ): m_lines( n_lines ) {}

        void put_at( const Row i_row, const Col i_col, in_<string_view> line )
        {
            Line& stored = m_lines.at( i_row.value );
            const Nat new_size = max( nsize( stored ), i_col.value + u8::n_cp_in( line ) );
            stored.resize( new_size, ' ' );
            u8::for_each_cp_in( line,
                [i = i_col.value, &stored]( in_<u8::Code_point> cp ) mutable {
                    stored[i++] = cp;
                }
            );
        }

        void put_at( const Row i_row, in_<string_view> line ) { put_at( i_row, Col{0}, line ); }

        auto string_at( const Row i_row ) const
            -> string
        {
            string result;
            for( const u8::Code_point& cp: m_lines.at( i_row.value ) ) {
                result.append( cp.sv() );
            }
            while( result != "" and result.back() == ' ' ) { result.pop_back(); }   // Trim right.
            return result;
        }
    };

    auto spaces( const Nat n ) -> string { return string( n, ' ' ); }

    auto repeat_times( const Nat n, in_<string_view> s )
        -> string
    {
        string result;
        for( Nat i = 0; i < n; ++i ) { result.append( s ); }
        return result;
    }

    void run()
    {
        const Nat       left_margin         = 2;
        const Nat       horizontal_scaling  = 2;    // A char is ~half as wide as high.

        const int   i_first_line    = -15;
        const int   i_last_line     = +15;
        const Nat   n_columns       = 120;
        const Nat   n_lines         = (i_last_line + 1) - i_first_line;

        using Row = Display_buffer::Row;  using Col = Display_buffer::Col;
        auto display_buffer = Display_buffer( n_lines );

        const auto generate_y_axis = [&]
        {
            display_buffer.put_at( Row{ 0 - i_first_line }, repeat_times( n_columns, "━" ) );
            for( Nat i_col = left_margin; i_col < n_columns; i_col += 5*horizontal_scaling ) {
                if( i_col > left_margin ) { display_buffer.put_at( Row{ 0 - i_first_line }, Col{ i_col }, "┿" ); }
            }
        };

        const auto generate_x_axis_and_graph = [&]
        {
            for( Nat i = 0; i < n_lines; ++i ) {
                const int i_line = i + i_first_line;
                const double x = i_line;
                const double y = f( x );
                
                const int       i_column        = left_margin + static_cast<int>( y*horizontal_scaling );
                const bool      is_marked       = (i_line % 5 == 0);
                const C_str     x_axis_char     = (i_line == 0? "╋" : is_marked? "╂" : "┃");
                const C_str     plot_char       = (is_marked? "■" : "○");

                display_buffer.put_at( Row{ i }, Col{ left_margin }, x_axis_char );
                if( 0 <= i_column and i_column < 120 ) {
                    display_buffer.put_at( Row{ i }, Col{ i_column }, plot_char );
                }
            }
        };

        generate_y_axis();
        generate_x_axis_and_graph();
        display_buffer.put_at(
            Row{ 2 - i_first_line }, Col{ left_margin + 40 },
            "Parabola (x²/4) — ASCII art graph by 日本国 кошка, version 2."
            );
        for( Nat i = 0; i < n_lines; ++i ) { cout << display_buffer.string_at( Row{ i } ) << '\n'; }
    }
}  // app

auto main() -> int { system( "chcp 65001 >nul" );  app::run(); }
