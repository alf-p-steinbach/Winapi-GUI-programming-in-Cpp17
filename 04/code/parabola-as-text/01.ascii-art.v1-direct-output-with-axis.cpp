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
        for( int line = -15; line <= +15; ++line ) {
            const double x = line;
            const double y = f( x );

            const int       column          = left_margin + static_cast<int>( y*horizontal_scaling );
            const bool      is_marked       = (line % 5 == 0);
            const C_str     x_axis_char     = (is_marked? "╂" : "┃");
            const C_str     plot_char       = (is_marked? "■" : "○");

            if( column < 0 or column >= 120 ) {
                cout    << spaces( left_margin ) << x_axis_char << '\n';
            } else if( column < left_margin ) {
                cout    << spaces( column ) << plot_char
                        << spaces( left_margin - (column + 1) ) << x_axis_char
                        << '\n';
            } else if( column == left_margin ) {
                cout    << spaces( column ) << plot_char << '\n';
            } else {
                cout    << spaces( left_margin ) << x_axis_char
                        << spaces( column - (left_margin + 1) ) << plot_char
                        << '\n';
            }
        }
    }
}  // app

auto main() -> int { system( "chcp 65001 >nul" );  app::run(); }
