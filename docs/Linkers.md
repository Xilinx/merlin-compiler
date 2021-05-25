# A small tutorial to initiate learning the build toolchain

## Preliminaries

`clang++` and `g++` take individual .cpp files and turn them into .o files. All code in the .cpp
must have _definitions_, but the _implementation_ is not necessary for a successful compile. When
you include a .hpp, the C++ pre-processor simply replaces the #include directive with the content of
that .hpp -- header files are a useful index of definitions, and .cpp files contain implementations.
To produce the final executable, the compiler calls either `ld` or `lld` to link the object files
(.o) together along with any static libraries (.a) and dynamic libraries (.so). To produce .a files,
the compiler calls `ar` to compress .o files together and produce an index for ease of linking. To
produce .so files, the compiler calls `ld` or `lld` with the `-dy` flag. Code is copied out from
static libraries when they are linked into other artifacts, and the produced artifact has no
dependency on the static library; dynamic libraries are loaded at runtime by the dynamic loader, and
the paths to libraries are in `/etc/ld.so.conf.d` (typically `/usr/lib` and `/usr/local/lib` among
others).

## Link Errors, libstdc++, and libc++

Link errors arise when a directory specified in the `-L` flag does not contain the library specified
using the `-l` flag.

    $ ld -lc --verbose
    attempt to open /usr/x86_64-redhat-linux/lib64/libc.so failed
    attempt to open /usr/x86_64-redhat-linux/lib64/libc.a failed
    attempt to open /usr/local/lib64/libc.so failed
    attempt to open /usr/local/lib64/libc.a failed
    attempt to open /lib64/libc.so succeeded
    opened script file /lib64/libc.so
    opened script file /lib64/libc.so
    attempt to open /lib64/libc.so.6 succeeded
    /lib64/libc.so.6
    attempt to open /usr/lib64/libc_nonshared.a succeeded
    attempt to open /lib64/ld-linux-x86-64.so.2 succeeded
    /lib64/ld-linux-x86-64.so.2
    ld-linux-x86-64.so.2 needed by /lib64/libc.so.6
    found ld-linux-x86-64.so.2 at /lib64/ld-linux-x86-64.so.2

So ld found libc, and it was under one of the paths specified in the conf. Notice that libc is
classically a dynamic library, and every single program on the entire system must be compiled
against the same version. The compiler mangles the symbols of the various functions during
compilation, and during link-time, these symbols are looked up in the other artifacts.

    $ nm -C str.o
                     U __cxa_begin_catch
                     U __cxa_end_catch
                     U __gxx_personality_v0
    0000000000000000 T main
                     U _Unwind_Resume
                     U std::__1::locale::use_facet(std::__1::locale::id&) const
                     U std::__1::ios_base::getloc() const
    0000000000000000 W std::__1::char_traits<char>::eq_int_type(int, int)
    0000000000000000 W std::__1::char_traits<char>::eof()
                     U std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(unsigned long, char)
                     U std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(char const*)
                     U std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::~basic_string()
                     U std::__1::basic_ostream<char, std::__1::char_traits<char> >::put(char)
                     U std::__1::basic_ostream<char, std::__1::char_traits<char> >::flush()
                     U std::__1::basic_ostream<char, std::__1::char_traits<char> >::sentry::sentry(std::__1::basic_ostream<char, std::__1::char_traits<char> >&)
                     U std::__1::basic_ostream<char, std::__1::char_traits<char> >::sentry::~sentry()
    0000000000000000 W std::__1::ostreambuf_iterator<char, std::__1::char_traits<char> > std::__1::__pad_and_output<char, std::__1::char_traits<char> >(std::__1::ostreambuf_iterator<char, std::__1::char_traits<char> >, char const*, char const*, char const*, std::__1::ios_base&, char)
    0000000000000000 W std::__1::basic_ostream<char, std::__1::char_traits<char> >& std::__1::__put_character_sequence<char, std::__1::char_traits<char> >(std::__1::basic_ostream<char, std::__1::char_traits<char> >&, char const*, unsigned long)
                     U std::__1::cout
    0000000000000000 W std::__1::basic_ostream<char, std::__1::char_traits<char> >& std::__1::endl<char, std::__1::char_traits<char> >(std::__1::basic_ostream<char, std::__1::char_traits<char> >&)
                     U std::__1::ctype<char>::id
                     U std::__1::locale::~locale()
                     U std::__1::ios_base::__set_badbit_and_consider_rethrow()
                     U std::__1::ios_base::clear(unsigned int)
    0000000000000000 W std::__1::iterator<std::__1::output_iterator_tag, void, void, void, void>::iterator()
    0000000000000000 W std::__1::iterator<std::__1::output_iterator_tag, void, void, void, void>::iterator()
    0000000000000000 n std::__1::iterator<std::__1::output_iterator_tag, void, void, void, void>::iterator()
    0000000000000000 W std::__1::basic_ostream<char, std::__1::char_traits<char> >& std::__1::operator<< <char, std::__1::char_traits<char>, std::__1::allocator<char> >(std::__1::basic_ostream<char, std::__1::char_traits<char> >&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&)

The symbols in this object file fall into three categories:

1.  `__cxa`, `__gxx`, `__Unwind` symbols that can be resolved by linking to glibc and gcc libraries.

2.  main is a function in the program that's fully defined. The 'T' next to it indicates "text",
    which means that it is _defined_ in str.o (because it was defined in str.c), and therefore does
    not need any further resolution.

3.  `std::__1` symbols, which can be resolved by linking to libc++ from LLVM (not libstdc++ that's
    supplied with gcc). libc++ uses the C++11 "inline namespaces" feature to mangle the standard
    library so that mixing libstdc++ with libc++ would lead to link-time errors instead of runtime
    crashes; libc++ is _not_ binary-compatible with libstdc++.

Okay, so how do we compile str.c using gcc and libc++? First, we produce an object file with the
mangled symbols from including the libc++ headers. Notice how `std::__::placeholders` symbols are
additionally present when we use `-std=c++11`.

    $ g++ -std=c++11 -nostdinc++ -I<prefix>/include/c++/v1 -c str.c
    $ nm -C str.o

Next, we need to invoke `ld` to link str.o with the dynamic library libc++.so:

    $ ld str.o -L<prefix>/lib -dy -lc++

Lots of link errors, but let's look at the prerequisites (undefined symbols) in libc++ first:

    $ nm -uC libc++.so.1.0
          U fprintf@@GLIBC_2.2.5
          ...

Any C++ library has to be built on top of a glibc. Okay, so where can we find these symbols?

    $ ld str.o -L<prefix>/lib -L/lib64 -dy -lc++ -lc

Note that we absolutely need to link libc.so dynamically; static linking will only work with the
special musl library. We still get link errors, but we find out which libraries the symbols are in
are try again:

    $ ld str.o -L/curr/software/llvm/lib -L/lib64 \
        -L/lib/gcc/x86_64-redhat-linux/4.8.2 -dy -lc++ -lc -lpthread -lgcc_s
    ld: warning: cannot find entry symbol _start; defaulting to 0000000000400950
    str.o: In function `main':
    str.cpp:(.text+0x16): undefined reference to `std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(char const*)'
    str.o: In function `std::__1::ostreambuf_iterator<char, std::__1::char_traits<char> > std::__1::__pad_and_output<char, std::__1::char_traits<char> >(std::__1::ostreambuf_iterator<char, std::__1::char_traits<char> >, char const*, char const*, char const*, std::__1::ios_base&, char)':
    str.cpp:(.text._ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_[_ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_]+0x190): undefined reference to `std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(unsigned long, char)'
    ld: defaultlibs: hidden symbol `_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1EPKc' isn't defined
    ld: final link failed: Bad value

Shouldn't these symbols be present in the text section of libc++? What happened?

    $ nm --defined-only -C libc++.so.1.0 | fgrep 'std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(char const*)'
    000000000008f130 t std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(char const*)
    000000000008f130 t std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(char const*)

It is indeed present, but the lowercase 't' indicates that it is not visible (exported). Let's see
what `clang` does with `str.cpp`:

   $ clang++ -std=c++11 -c str.cpp -o str.clang.o -stdlib=libc++
   $ nm -C str.clang.o

In the diff, we see:

    - std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string(char const*)
    + std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__init(char const*, unsigned long)

Of course it doesn't use the non-exported symbol! We could choose to fork libcxx and make the
symbols visible.

## `_start`, C runtime objects (crt)

Now that we understand visibility, it's time to invoke `ld`; note that the objects in the C runtime
must be prefixed and suffixed as follows for `_start` and a bunch of other ELF ABI symbols to be
resolved:

    crt1.o
    crti.o
    crtbegin.o
    <main object files>
    crtend.o
    crtn.o
