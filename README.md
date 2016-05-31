# fmt11 <a href="https://travis-ci.org/r-lyeh/fmt11"><img src="https://api.travis-ci.org/r-lyeh/fmt11.svg?branch=master" align="right" /></a>
Tiny format/mustache templating library (C++11)
Inspired by the better, safer and more featured library [fmtlib](https://github.com/fmtlib/fmt)

## Features:
- [x] Basic python formatting: "{} {0} {1} {2:#x} {4:8.2} {:<20}"
- [x] Basic mustache templating "{{key}}"
- [x] Tiny, cross-platform,  macro-less, header-only.
- [x] Public domain.

## Cons:
- [ ] Probably slow. No recursive code, but some heap allocations left.
- [ ] Probably unsafe. Only minimal bound checking is performed.
- [ ] Probably not featured enough.

## Alternatives
- https://github.com/c42f/tinyformat
- https://github.com/fmtlib/fmt
- https://github.com/seanmiddleditch/formatxx
- https://github.com/d-led/fakeformat
- https://github.com/no1msd/mstch/
- https://github.com/rnlf/flossy
- https://github.com/dbralir/print
- https://github.com/kainjow/Mustache
- https://github.com/apfeltee/cpp11-sprintf

## Showcase
```c++
    fmt11( "Hello {} {}", "world", 123);   // variadic arguments
    fmt11( "Hello {0} {1}", "world", 123); // indices
    fmt11( "Hello {1} {0}", "world", 123); // indices: reordering
    fmt11( "{0}{1}{0}", "abra", "cad");    // indices: repeating
    fmt11( "{0:x} {0:d} {0:o}", 42, 42, 42 );                 // formatting
    fmt11( "{0:.8.2} {1:;10.2} {0:!8.5}", 3.14159, 3.14159 ); // formatting
    fmt11( "{:<20} {:>20}", "hello", "world");                // alignment

    // symbols and mustaches
    std::map< std::string, std::string > map { {"player1", "John"} };
    fmt11map( map, "Welcome {{player1}:!<20}. It's {0} {2} in {1}!", 12, "town", "o'clock" );
```

## Changelog
- v1.0.1 (2016/05/31): Extra boundary checks
- v1.0.0 (2016/05/29): Initial version
