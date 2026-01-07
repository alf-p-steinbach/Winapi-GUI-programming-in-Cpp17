#include <iostream>
#include <string>

#include <cstdlib>

using Nat = int;
using C_str = const char*;

namespace app {
    using   std::cout,              // <iostream>
            std::string;            // <string>
    using   std::system;            // <cstdlib>

    auto f( const double x ) -> double { return x*x/4; }

    auto spaces( const Nat n ) -> string { return string( n, ' ' ); }

    void run()
    {
        const Nat       left_margin         = 2;
        const double    horizontal_scaling  = 2;    // A char is ~half as wide as high.

        cout << "Parabola (x²/4) — ASCII art graph by 日本国 кошка, version 1.\n";
        for( int i_line = -15; i_line <= +15; ++i_line ) {
            const double x = i_line;
            const double y = f( x );

            const int       i_column        = left_margin + static_cast<int>( y*horizontal_scaling );
            const bool      is_marked       = (i_line % 5 == 0);
            const C_str     x_axis_char     = (is_marked? "╂" : "┃");
            const C_str     plot_char       = (is_marked? "■" : "○");

            if( i_column < 0 or i_column >= 120 ) {
                cout    << spaces( left_margin ) << x_axis_char << '\n';
            } else if( i_column < left_margin ) {
                cout    << spaces( i_column ) << plot_char
                        << spaces( left_margin - (i_column + 1) ) << x_axis_char
                        << '\n';
            } else if( i_column == left_margin ) {
                cout    << spaces( i_column ) << plot_char << '\n';
            } else {
                cout    << spaces( left_margin ) << x_axis_char
                        << spaces( i_column - (left_margin + 1) ) << plot_char
                        << '\n';
            }
        }
    }
}  // app

auto main() -> int { system( "chcp 65001 >nul" );  app::run(); }
