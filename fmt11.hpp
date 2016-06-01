// Tiny format/mustache templating library (C++11)
// - rlyeh, public domain

// Inspired by the better, safer and more featured library [fmtlib](https://github.com/fmtlib/fmt)
// PS: I ripped the examples and a few tests as well O:)

// ## Features
// [x] Basic python formatting: "{} {0} {1} {2:#x} {4:8.2} {:<20}"
// [x] Basic mustache templating "{{key}}"
// [x] Tiny, cross-platform,  macro-less, header-only.
// [x] Public domain.

// ## Cons
// [ ] Probably slow. No recursive code, but some allocations remain still.
// [ ] Probably unsafe. No exhaustive bound checking is performed at all.
// [ ] Probably not full featured enough.

// ## Alternatives
// - https://github.com/apfeltee/cpp11-sprintf
// - https://github.com/c42f/tinyformat
// - https://github.com/d-led/fakeformat
// - https://github.com/dbralir/print
// - https://github.com/fmtlib/fmt
// - https://github.com/kainjow/Mustache
// - https://github.com/no1msd/mstch/
// - https://github.com/rnlf/flossy
// - https://github.com/seanmiddleditch/formatxx

#ifndef FMT11_VERSION
#define FMT11_VERSION "1.0.2" /* (2016/06/01): Parse valid identifiers only
#define FMT11_VERSION "1.0.1" // (2016/05/31): Extra boundary checks
#define FMT11_VERSION "1.0.0" // (2016/05/29): Initial version */

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

template<unsigned trail_args, typename Map, typename... Args>
inline std::string fmt11hlp( const Map *ctx, const char *format, Args... args) {
    std::stringstream out;
    if( format ) {
         auto tpl = std::tuple_cat( std::tuple<Args...>{ args... }, std::make_tuple( 0,0,0,0,0,0,0,0,0,0) );
         char raw[64], tag[32], fmt[32];
         unsigned fix, dig, counter = 0;
         while( *format ) {
            if( *format++ != '{' ) {
                out << format[-1];
            } else { 
                auto parse = []( char raw[64], char tag[32], char fmt[32], unsigned &fix, unsigned &dig, const char *in ) -> int {
                    int lv = 0; // parses [{] { [tag][:][fmt] } [}] expressions; returns num of bytes parsed or 0 if error
                    char *o = raw, *m = tag, *g = 0;
                    while( *in && *in == '{' ) {
                        *o++ = *in++, ++lv;
                        if( (o - raw) >= 63 ) return 0;
                    }
                    while( *in && lv > 0 ) {
                        /**/ if( *in < 32 ) return 0;
                        else if( *in < '0' && !g ) return 0;
                        else if( *in == '}' ) --lv, *o++ = *in++;
                        else if( *in == ':' ) g = fmt, *o++ = *in++;
                        else *( g ? g : m)++ = *o++ = *in++;
                        if( ((o - raw) >= 63) || ((m - tag) >= 31) || ( g && (g - fmt) >= 31 )) return 0;
                    } 
                    *o = *m = *(g ? g : fmt) = 0;
                    if( 0 != lv ) {
                        return 0;
                    }
                    fix = dig = 0;
                    for( char *f = fmt; *f != 0; ++f ) {
                        char *input = f;
                        if( *input >= '0' && *input <= '9' ) {
                            double dbl = atof(input);
                            fix = int(dbl), dig = int(dbl * 1000 - fix * 1000);
                            while( dig && !(dig % 10) ) dig /= 10;
                            //printf("%s <> %d %d\n", input, fix, dig );
                            break;
                        }
                    }
                    return o - raw;
                };
                int read_bytes = parse( raw, tag, fmt, fix, dig, &format[-1] );
                if( !read_bytes ) {
                    out << format[-1];
                } else {
                    // style
                    format += read_bytes - 1;
                    for( char *f = fmt; *f; ++f ) switch( *f ) {
                        default: 
                            if( f[0] >= '0' && f[0] <= '9' ) {
                                while( (f[0] >= '0' && f[0] <= '9') || f[0] == '.' ) ++f; --f;
                                out << std::setw( fix );
                                out << std::fixed;
                                out << std::setprecision( dig );
                            } else {
                                out.fill( f[0] );
                            }
                        break; case '#': out << std::showbase;
                        break; case 'b': out << std::boolalpha;
                        break; case 'D': out << std::dec << std::uppercase;
                        break; case 'd': out << std::dec;
                        break; case 'O': out << std::oct << std::uppercase;
                        break; case 'o': out << std::oct;
                        break; case 'X': out << std::hex << std::uppercase;
                        break; case 'x': out << std::hex;
                        break; case 'f': out << std::fixed;
                        break; case '<': out << std::left;
                        break; case '>': out << std::right;
                    }
                    // value
                    char arg = tag[0];
                    if( !arg )  {
                        if( counter < (sizeof...(Args) - trail_args) ) {
                            arg = '0' + counter++;
                        } else {
                            arg = '\0';
                        }
                        //printf("arg %d/%d\n", int(counter), (sizeof...(Args) - trail_args));
                    }
                    switch( arg ) {
                        default: 
                            if( ctx ) {
                                auto find = ctx->find(tag);
                                if( find == ctx->end() ) out << raw;
                                else out << find->second;
                            } else {
                                out << raw;
                            }
                        break; case   0: out << raw;
                        break; case '0': out << std::get<0>(tpl);
                        break; case '1': out << std::get<1>(tpl);
                        break; case '2': out << std::get<2>(tpl);
                        break; case '3': out << std::get<3>(tpl);
                        break; case '4': out << std::get<4>(tpl);
                        break; case '5': out << std::get<5>(tpl);
                        break; case '6': out << std::get<6>(tpl);
                        break; case '7': out << std::get<7>(tpl);
                        break; case '8': out << std::get<8>(tpl);
                        break; case '9': out << std::get<9>(tpl);
                    }
                }
            }
        }
    }
    return out.str();
}

inline std::string fmt11( const char *format ) {
    return fmt11hlp<1, std::map<std::string, std::string>>( nullptr, format, 0 );
}

template<typename... Args>
inline std::string fmt11( const char *format, Args... args ) {
    return fmt11hlp<0, std::map<std::string, std::string>>( nullptr, format, args... );
}

template<typename Map>
inline std::string fmt11map( const Map &ctx, const char *format ) {
    return fmt11hlp<1>( &ctx, format, 0 );
}

template<typename Map, typename... Args>
inline std::string fmt11map( const Map &ctx, const char *format, Args... args) {
    return fmt11hlp<0>( &ctx, format, args... );
}

#ifdef FMT11_BUILD_DEMO
#include <stdio.h>
int main() {
#   define TEST(a,b) printf("[%s] L%d: %s\n", !!((a) == (b)) ? " OK ":"FAIL", __LINE__, (a).c_str())

    // basics
    TEST( fmt11("Hello"), "Hello" );
    TEST( fmt11("Hello {} {}",   123, "456"), "Hello 123 456" ); // equivalent to {0} {1}
    TEST( fmt11("Hello {0} {1}", 123, "456"), "Hello 123 456" );
    TEST( fmt11("Hello {1} {0}", 123, "456"), "Hello 456 123" );

    // indices and reordering
    TEST( fmt11("Hello {1} {0}", 123, "world" ), "Hello world 123");
    TEST( fmt11("Hello {0} {1}", 123, "world" ), "Hello 123 world");
    TEST( fmt11("{0}{1}{0}", "abra", "cad" ), "abracadabra");

    // basic formatting: dec, hex, oct, prefixes, uppercase 
    TEST( fmt11("Hello {:d} {:x} {:o}", 42, 42, 42), "Hello 42 2a 52");
    TEST( fmt11("Hello {:#d} {:#x} {:#o}", 42, 42, 42), "Hello 42 0x2a 052");
    TEST( fmt11("Hello {0:d} {0:x} {0:o}", 42, 42, 42), "Hello 42 2a 52");
    TEST( fmt11("Hello {0:d} {0:#x} {0:#o}", 42, 42, 42), "Hello 42 0x2a 052");
    TEST( fmt11("Hello {:D} {:X} {:O}", 42, 42, 42), "Hello 42 2A 52");
    TEST( fmt11("Hello {:#D} {:#X} {:#O}", 42, 42, 42), "Hello 42 0X2A 052");
    TEST( fmt11("Hello {0:D} {0:X} {0:O}", 42, 42, 42), "Hello 42 2A 52");
    TEST( fmt11("Hello {0:D} {0:#X} {0:#O}", 42, 42, 42), "Hello 42 0X2A 052");
    TEST( fmt11("Hello {:d} {:x} {:o} {:#x} {:#o} {:#X} {:#O}", 42, 42, 42, 42, 42, 42, 42 ), "Hello 42 2a 52 0x2a 052 0X2A 052" );

    // extra formatting: width, decimals, custom prefixes and alignment
    TEST( fmt11("Hello {:8} {:12} {:16}", 3.14159, 3.14159, 3.14159 ), "Hello        3            3                3" );
    TEST( fmt11("Hello {:8.2} {:12.3} {:16.4}", 3.14159, 3.14159, 3.14159 ), "Hello     3.14        3.142           3.1416" );
    TEST( fmt11("Hello {:*8.2} {:|12.3} {:.16.4}", 3.14159, 3.14159, 3.14159 ), "Hello ****3.14 |||||||3.142 ..........3.1416" );
    TEST( fmt11("Hello {:<*8.2} {:<|12.3} {:<.16.4}", 3.14159, 3.14159, 3.14159 ), "Hello 3.14**** 3.142||||||| 3.1416.........." );
    TEST( fmt11("Hello {:.>10} Emmett {:!<10}", "Doc", "Brown" ), "Hello .......Doc Emmett Brown!!!!!" );

    // context and {{mustaches}}
    std::map< std::string, std::string > ctx { { "player1", "John" }, { "player2", "Doe" } };
    TEST( fmt11map( ctx, "Hello {{player1}} & {{player2}}!!"), "Hello John & Doe!!");
    TEST( fmt11map( ctx, "Hello {{player1}} & {{player2}}!! {0} {} {1}!!", 123, 456), "Hello John & Doe!! 123 123 456!!");
    TEST( fmt11map( ctx, "Hello {{player1}:*>10} & {{player2}:*<10}!!"), "Hello ******John & Doe*******!!");
    TEST( fmt11map( ctx,"{{player1}}{{player2}}"), "JohnDoe" );
    ctx.clear();

    // check for malformed input
    TEST( fmt11(0), "" );                // null ptr
    TEST( fmt11(""), "" );               // empty string
    TEST( fmt11("{}{}{}"), "{}{}{}" );   // no args
    TEST( fmt11("{}{}{}",1,2), "12{}" ); // not enough args
    TEST( fmt11("", 1,2,3), "");         // too many args
    TEST( fmt11("{}",1,2,3), "1" );      // too many args
    TEST( fmt11("{"), "{" );             // unbalanced
    TEST( fmt11("}"), "}" );             // unbalanced
    TEST( fmt11("{{"), "{{" );           // unbalanced
    TEST( fmt11("}}"), "}}" );           // unbalanced
    TEST( fmt11("{{{"), "{{{" );         // unbalanced
    TEST( fmt11("}}}"), "}}}" );         // unbalanced
    TEST( fmt11("}{"), "}{" );           // mismatch
    TEST( fmt11("}{}"), "}{}" );         // mismatch
    TEST( fmt11("}}{"), "}}{" );         // mismatch
    TEST( fmt11("{}{"), "{}{" );         // mismatch
    TEST( fmt11("{}}"), "{}}" );         // mismatch
    TEST( fmt11("{\t}", "hello"), "{\t}"); // invalid identifier
    TEST( fmt11("{\"{0}\"}", "hello"), "{\"hello\"}"); // false positive \" is not part of an identifier
    TEST( fmt11( std::string(128, '{').c_str() ), std::string(128, '{') );  // buffer overflow
    TEST( fmt11( std::string(128, '}').c_str() ), std::string(128, '}') );  // buffer overflow
    TEST( fmt11("{{player1}}{{player2}}"), "{{player1}}{{player2}}" );      // missing map
    TEST( fmt11map( ctx,"{{player1}}{{player2}}"), "{{player1}}{{player2}}" ); // empty map
}
#endif

#endif
