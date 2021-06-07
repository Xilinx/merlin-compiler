/* JJW (12/8/2008): Do not include these definitions for EDG 3.10 */
/* DQ (11/1/2011): I think we do need this for at least C++ code (e.g. __CHAR_BIT__ must
   be defined along with lots of other Common Predefined Macros.
*/
// DQ (12/18/2018): Adding support to ignore output of declarations in this file when using
// EDG frontend for CPP processing with the "-edg:E" option.  Note that "-E" can be specified
// but that causes the backend compiler to be called directly and this file of preinclude 
// declarations is not used in that case.
#ifndef USE_ROSE_CPP_PROCESSING

/* DQ (6/12/2005): Permit this to be optionally commented out because it makes it hard to
    look at the AST merging graphs. The ROSE-IR graphs will now filter nodes associated
    with declarations from this file (marked as front-end specific).
*/
#if 0
#define SKIP_ROSE_BUILTIN_DECLARATIONS
#endif

/* DQ (9/28/2012): define away __restrict as a test for C support (temporary nginx application handling). */
#define __restrict

/* This macro can be defined using "-DSKIP_ROSE_BUILTIN_DECLARATIONS" to avoid ROSE
   builtin functions required for compatability with the user selected backend compiler 
*/
#ifndef SKIP_ROSE_BUILTIN_DECLARATIONS

/* Must use C style comments so that "--edg:old_c" options will work! */
/* DQ (7/13/2006): Undefine these before defining them to avoid warnings. */
/* DQ (12/23/2006): Let EDG define this if possible, but we reset it for 64 bit systems
   where either EDG does not get it correct or we don't setup EDG correctly!
#undef __SIZE_TYPE__ 
*/
#undef __VERSION__

/* TV (3/8/13): undef 6 additionals macros because they are define while using EDG 4.0 with CUDA support */
#undef __unix__
#undef __x86_64
#undef __linux__
#undef unix
#undef __x86_64__
#undef __WCHAR_TYPE__


// $REPLACE_ME_WITH_MACRO_DEFINITIONS
#define __DBL_MIN_EXP__ (-1021)
#define __UINT_LEAST16_MAX__ 65535
#define __ATOMIC_ACQUIRE 2
#define __FLT_MIN__ 1.17549435082228750797e-38F
#define __GCC_IEC_559_COMPLEX 2
#define __UINT_LEAST8_TYPE__ unsigned char
#define __INTMAX_C(c) c ## L
#define __CHAR_BIT__ 8
#define __UINT8_MAX__ 255
#define __WINT_MAX__ 4294967295U
#define __ORDER_LITTLE_ENDIAN__ 1234
#define __SIZE_MAX__ 18446744073709551615UL
#define __WCHAR_MAX__ 2147483647
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 1
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 1
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 1
#define __DBL_DENORM_MIN__ ((double)4.94065645841246544177e-324L)
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8 1
#define __GCC_ATOMIC_CHAR_LOCK_FREE 2
#define __GCC_IEC_559 2
#define __FLT_EVAL_METHOD__ 0
#define __unix__ 1
#define __GCC_ATOMIC_CHAR32_T_LOCK_FREE 2
#define __x86_64 1
#define __UINT_FAST64_MAX__ 18446744073709551615UL
#define __SIG_ATOMIC_TYPE__ int
#define __DBL_MIN_10_EXP__ (-307)
#define __FINITE_MATH_ONLY__ 0
#define __UINT_FAST8_MAX__ 255

#define __DEC64_MAX_EXP__ 385
#define __INT8_C(c) c
#define __UINT_LEAST64_MAX__ 18446744073709551615UL
#define __SHRT_MAX__ 32767
#define __LDBL_MAX__ 1.18973149535723176502e+4932L
#define __UINT_LEAST8_MAX__ 255
#define __GCC_ATOMIC_BOOL_LOCK_FREE 2
#define __UINTMAX_TYPE__ long unsigned int
#define __linux 1
#define __DEC32_EPSILON__ 1E-6DF
#define __unix 1
#define __UINT32_MAX__ 4294967295U
#define __LDBL_MAX_EXP__ 16384
#define __WINT_MIN__ 0U
#define __linux__ 1
#define __SCHAR_MAX__ 127
#define __WCHAR_MIN__ (-__WCHAR_MAX__ - 1)
#define __INT64_C(c) c ## L
#define __DBL_DIG__ 15
#define __GCC_ATOMIC_POINTER_LOCK_FREE 2
#define __SIZEOF_INT__ 4
#define __SIZEOF_POINTER__ 8
#define __USER_LABEL_PREFIX__ 
#define __LDBL_HAS_INFINITY__ 1
#define __FLT_EPSILON__ 1.19209289550781250000e-7F
#define __LDBL_MIN__ 3.36210314311209350626e-4932L
#define __DEC32_MAX__ 9.999999E96DF
#define __INT32_MAX__ 2147483647
#define __SIZEOF_LONG__ 8
#define __STDC_IEC_559__ 1
#define __STDC_ISO_10646__ 201103L
#define __UINT16_C(c) c
#define __DECIMAL_DIG__ 21
#define __gnu_linux__ 1
#define __has_include_next(STR) __has_include_next__(STR)
#define __LDBL_HAS_QUIET_NAN__ 1
#define __MMX__ 1
#define __FLT_HAS_DENORM__ 1
#define __SIZEOF_LONG_DOUBLE__ 16
#define __BIGGEST_ALIGNMENT__ 16
#define __DBL_MAX__ ((double)1.79769313486231570815e+308L)
#define __INT_FAST32_MAX__ 9223372036854775807L
#define __DBL_HAS_INFINITY__ 1
#define __DEC32_MIN_EXP__ (-94)
#define __INT_FAST16_TYPE__ long int
#define __LDBL_HAS_DENORM__ 1
#define __DEC128_MAX__ 9.999999999999999999999999999999999E6144DL
#define __INT_LEAST32_MAX__ 2147483647
#define __DEC32_MIN__ 1E-95DF
#define __DBL_MAX_EXP__ 1024
#define __DEC128_EPSILON__ 1E-33DL
#define __SSE2_MATH__ 1
#define __ATOMIC_HLE_RELEASE 131072
#define __PTRDIFF_MAX__ 9223372036854775807L
#define __amd64 1
#define __STDC_NO_THREADS__ 1
#define __ATOMIC_HLE_ACQUIRE 65536
#define __LONG_LONG_MAX__ 9223372036854775807LL
#define __SIZEOF_SIZE_T__ 8
#define __SIZEOF_WINT_T__ 4
#define __GCC_HAVE_DWARF2_CFI_ASM 1
#define __GXX_ABI_VERSION 1002
#define __FLT_MIN_EXP__ (-125)
#define __INT_FAST64_TYPE__ long int
#define __DBL_MIN__ ((double)2.22507385850720138309e-308L)
#define __LP64__ 1
#define __DECIMAL_BID_FORMAT__ 1
#define __DEC128_MIN__ 1E-6143DL
#define __REGISTER_PREFIX__ 
#define __UINT16_MAX__ 65535
#define __DBL_HAS_DENORM__ 1
#define __UINT8_TYPE__ unsigned char
#define __NO_INLINE__ 1
#define __FLT_MANT_DIG__ 24
#define __VERSION__ "4.9.4"
#define __UINT64_C(c) c ## UL
#define _STDC_PREDEF_H 1
#define __GCC_ATOMIC_INT_LOCK_FREE 2
#define __FLOAT_WORD_ORDER__ __ORDER_LITTLE_ENDIAN__
#define __STDC_IEC_559_COMPLEX__ 1
#define __INT32_C(c) c
#define __DEC64_EPSILON__ 1E-15DD
#define __ORDER_PDP_ENDIAN__ 3412
#define __DEC128_MIN_EXP__ (-6142)
#define __INT_FAST32_TYPE__ long int
#define __UINT_LEAST16_TYPE__ short unsigned int
#define unix 1
#define __INT16_MAX__ 32767
#define __SIZE_TYPE__ long unsigned int
#define __UINT64_MAX__ 18446744073709551615UL
#define __INT8_TYPE__ signed char
#define __ELF__ 1
#define __FLT_RADIX__ 2
#define __INT_LEAST16_TYPE__ short int
#define __LDBL_EPSILON__ 1.08420217248550443401e-19L
#define __UINTMAX_C(c) c ## UL
#define __SSE_MATH__ 1
#define __k8 1
#define __SIG_ATOMIC_MAX__ 2147483647
#define __GCC_ATOMIC_WCHAR_T_LOCK_FREE 2
#define __SIZEOF_PTRDIFF_T__ 8
#define __x86_64__ 1
#define __DEC32_SUBNORMAL_MIN__ 0.000001E-95DF
#define __INT_FAST16_MAX__ 9223372036854775807L
#define __UINT_FAST32_MAX__ 18446744073709551615UL
#define __UINT_LEAST64_TYPE__ long unsigned int
#define __FLT_HAS_QUIET_NAN__ 1
#define __FLT_MAX_10_EXP__ 38
#define __LONG_MAX__ 9223372036854775807L
#define __DEC128_SUBNORMAL_MIN__ 0.000000000000000000000000000000001E-6143DL
#define __FLT_HAS_INFINITY__ 1
#define __UINT_FAST16_TYPE__ long unsigned int
#define __DEC64_MAX__ 9.999999999999999E384DD
#define __CHAR16_TYPE__ short unsigned int
#define __PRAGMA_REDEFINE_EXTNAME 1
#define __INT_LEAST16_MAX__ 32767
#define __DEC64_MANT_DIG__ 16
#define __INT64_MAX__ 9223372036854775807L
#define __UINT_LEAST32_MAX__ 4294967295U
#define __GCC_ATOMIC_LONG_LOCK_FREE 2
#define __INT_LEAST64_TYPE__ long int
#define __INT16_TYPE__ short int
#define __INT_LEAST8_TYPE__ signed char
#define __DEC32_MAX_EXP__ 97
#define __INT_FAST8_MAX__ 127
#define __INTPTR_MAX__ 9223372036854775807L
#define linux 1
#define __SSE2__ 1
#define __LDBL_MANT_DIG__ 64
#define __DBL_HAS_QUIET_NAN__ 1
#define __SIG_ATOMIC_MIN__ (-__SIG_ATOMIC_MAX__ - 1)
#define __code_model_small__ 1
#define __k8__ 1
#define __INTPTR_TYPE__ long int
#define __UINT16_TYPE__ short unsigned int
#define __WCHAR_TYPE__ int
#define __SIZEOF_FLOAT__ 4
#define __UINTPTR_MAX__ 18446744073709551615UL
#define __DEC64_MIN_EXP__ (-382)
#define __INT_FAST64_MAX__ 9223372036854775807L
#define __GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1
#define __FLT_DIG__ 6
#define __UINT_FAST64_TYPE__ long unsigned int
#define __INT_MAX__ 2147483647
#define __amd64__ 1
#define __INT64_TYPE__ long int
#define __FLT_MAX_EXP__ 128
#define __ORDER_BIG_ENDIAN__ 4321
#define __DBL_MANT_DIG__ 53
#define __INT_LEAST64_MAX__ 9223372036854775807L
#define __GCC_ATOMIC_CHAR16_T_LOCK_FREE 2
#define __DEC64_MIN__ 1E-383DD
#define __WINT_TYPE__ unsigned int
#define __UINT_LEAST32_TYPE__ unsigned int
#define __SIZEOF_SHORT__ 2
#define __SSE__ 1
#define __LDBL_MIN_EXP__ (-16381)
#define __INT_LEAST8_MAX__ 127
#define __SIZEOF_INT128__ 16
#define __LDBL_MAX_10_EXP__ 4932
#define __ATOMIC_RELAXED 0
#define __DBL_EPSILON__ ((double)2.22044604925031308085e-16L)
#define _LP64 1
#define __UINT8_C(c) c
#define __INT_LEAST32_TYPE__ int
#define __SIZEOF_WCHAR_T__ 4
#define __UINT64_TYPE__ long unsigned int
#define __INT_FAST8_TYPE__ signed char
#define __DBL_DECIMAL_DIG__ 17
#define __FXSR__ 1
#define __DEC_EVAL_METHOD__ 2
#define __UINT32_C(c) c ## U
#define __INTMAX_MAX__ 9223372036854775807L
#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#define __FLT_DENORM_MIN__ 1.40129846432481707092e-45F
#define __INT8_MAX__ 127
#define __UINT_FAST32_TYPE__ long unsigned int
#define __CHAR32_TYPE__ unsigned int
#define __FLT_MAX__ 3.40282346638528859812e+38F
#define __INT32_TYPE__ int
#define __SIZEOF_DOUBLE__ 8
#define __FLT_MIN_10_EXP__ (-37)
#define __INTMAX_TYPE__ long int
#define __DEC128_MAX_EXP__ 6145
#define __ATOMIC_CONSUME 1
#define __UINTMAX_MAX__ 18446744073709551615UL
#define __DEC32_MANT_DIG__ 7
#define __DBL_MAX_10_EXP__ 308
#define __LDBL_DENORM_MIN__ 3.64519953188247460253e-4951L
#define __INT16_C(c) c
#define __STDC__ 1
#define __PTRDIFF_TYPE__ long int
#define __ATOMIC_SEQ_CST 5
#define __UINT32_TYPE__ unsigned int
#define __UINTPTR_TYPE__ long unsigned int
#define __DEC64_SUBNORMAL_MIN__ 0.000000000000001E-383DD
#define __DEC128_MANT_DIG__ 34
#define __LDBL_MIN_10_EXP__ (-4931)
#define __SIZEOF_LONG_LONG__ 8
#define __GCC_ATOMIC_LLONG_LOCK_FREE 2
#define __LDBL_DIG__ 18
#define __FLT_DECIMAL_DIG__ 9
#define __UINT_FAST16_MAX__ 18446744073709551615UL
#define __GNUC_GNU_INLINE__ 1
#define __GCC_ATOMIC_SHORT_LOCK_FREE 2
#define __UINT_FAST8_TYPE__ unsigned char
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_RELEASE 3


/* DQ (9/1/2014): Define this for the Intel compiler (to avoid Intel specific code not handled by EDG) */
#ifdef __INTEL_COMPILER
  #define _OVERRIDE_COMPLEX_SPECIALIZATION_

/* DQ (4/3/2016): This appears to work fine as a definition for the missing macro support for the Intel V16 compiler. 
   Note that undefining the macro does not work well, and results in references to undefined functions.  
   This appears to be a clang specific macro function (compiler extension) that is supported in Intel V16 C++ cpmpiler.
 */
/* DQ (7/23/2020): Later versions of EDG support this properly (EDG 6.0 and maybe EDG 5.0).
   However this is confirmed to still be required for the Intel compiler.
*/
  #define __has_include_next(file) 1

/* DQ (5/10/2016): Ignoring the SSE support (turn it off). */
  #define _PMMINTRIN_H_INCLUDED
  #define _TMMINTRIN_H_INCLUDED
  #define _NMMINTRIN_H_INCLUDED
  #define _SMMINTRIN_H_INCLUDED
  #define _ZMMINTRIN_H_INCLUDED
  #define _INCLUDED_IMM
  #define _BITS_OPT_RANDOM_H

// DQ (12/17/2016): This is a Microsoft language extension which we need to define away in non-windows mode.
  #define __declspec(X)
#endif

/* DQ (8/28/2015): Adding specialized support for Intel compiler. */
#ifdef __INTEL_COMPILER
  #define __inline inline
#endif


// DQ (4/26/2019): Only use the ROSE_GNUATTR() featur for non-ansi code use. Might also 
// need to combine this with turning off the optimization flag that triggers use of builtins.
// Define away the GNU specific attribute mechanism (not supported by Microsoft compilers).
#if defined(ROSE_MICROSOFT_EXTENSIONS) || defined(__STRICT_ANSI__)
#  define ROSE_GNUATTR(X) 
#  define __attribute__(X) 
#else
#  define ROSE_GNUATTR(X) __attribute__(X)
#endif



/* DQ (4/23/2017): This is now added on the EDG command line to support handling of Microsoft options.
   This supports analysis of Microsofte specific code on Linuc platforms.  It is not clear now specific
   our support can be since we can't interogate the Microsoft compiler for some specifics.
 */
#ifdef ROSE_USE_MICROSOFT_EXTENSIONS

// Define away _Complex (until we figure out what this is equivalent to in MSVC).
#define _Complex
#define __int128_t long long int
#define __builtin_va_list int

// DQ (4/23/2017): Define/undef this so that WINE header files will avoid a bad path.
// This is an issue for the processing of the WINE 2.0.1 winnt.h header file (line 2345).
// Either of these will partially fix the problem, but I think that undefining _MSC_VER
// is the best solution, because I think we don't want 32-bit support and we don't want 
// to turn off 64-bit support.
// #define __i386__
// #undef __x86_64__

// DQ (9/4/2020): This is required for WINE support (need to rethink how to support MS Windows header files and WINE).
// DQ (9/2/2020): Heather reported this as a bug when using MS headers.
#undef _MSC_VER
// Heather reports additional macros that may be required are: 
// _MX64 = 100 
// _MAMD = 100
// DQ (9/2/2020): She is sending us more details about this.

// DQ (4/23/2017): This is required to avoid calling wine-2.0.1/include/winscard.h which calls wtypes.h (which does not exist).
// Also prevents call to headef file "unknown.h"
#define NOCRYPT 1

#if 0
#if defined(__i386__) && defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 2)))
    #error "__GNUC__ is defined"
#elif defined(__i386__) && defined(_MSC_VER)
    #error "__i386__ && _MSC_VER are defined"
#elif defined(__x86_64__) && defined(__GNUC__)
    #error "__x86_64__ && __GNUC__ are defined"
#elif defined(__x86_64__) && defined(_MSC_VER)
    #error "__x86_64__ && _MSC_VER are defined"
#else
    #error "nothing defined"
#endif
#endif

#endif

#if __sun
// PP (16/05/19): This is most likely WRONG, however some __int128_t is required on Solaris
typedef long long int __int128_t;
#endif /* __sun */

#if __cplusplus >= 201103L
/* DQ (1/27/2017): This is needed for at least the Intel compilers and GNU 4.8 and later compilers in C++11 mode. */
/* DQ (10/8/2015): Added support for Intel compiler to handle fenv.h header file. 
   This is reported as a bug in Intel v14 support for Boost, but this is a fix to
   permit the code in test2015_87.C to compile.
*/
/* DQ (4/9/2017): This is also a problem for the Intel v16 compiler (for C++11 test2017_04.C). */
/* DQ (4/4/2017): This is a problem for the Intel v15 compiler (for C++11 test2017_04.C). */

// DQ (4/10/2017): This is what is required to make Intel v15 and v16 work on my desktop.
#if (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 1500))
#define _GLIBCXX_FENV_H
#endif

#if !(defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 1500))

// DQ (4/10/2017): And this is what is required to make Intel V16 work on hudson-rose-23 (at least they both work).
#if !(defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 1600))

  #define __ECL
  typedef unsigned    long fenv_t;
  typedef unsigned    long fexcept_t;
  #define FE_INVALID      1
  #define FE_DIVBYZERO    4
  #define FE_OVERFLOW     8
  #define FE_UNDERFLOW    16
  #define FE_INEXACT      32
  #define _FENV_PUBAPI   /* do not change this line! */

/* DQ (9/2/2020): Added to support ROSE-2511 (Cxx11_tests/test2020_79.C) */
  #define FE_ALL_EXCEPT (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

  extern int  _FENV_PUBAPI feclearexcept (int __excepts) ;
  extern int  _FENV_PUBAPI fegetexceptflag (fexcept_t *__flagp, int __excepts) ;
  extern int  _FENV_PUBAPI fesetexceptflag (const fexcept_t *__flagp, int __excepts) ;
/* DQ (1/20/2017): Added remaining entries required for work around in Intel v16 compiler. */
  extern int  _FENV_PUBAPI feraiseexcept (int __excepts) throw ();
  extern int  _FENV_PUBAPI fetestexcept (int __excepts) throw ();
  extern int  _FENV_PUBAPI fegetround (void) throw ();
  extern int  _FENV_PUBAPI fesetround (int __rounding_direction) throw ();
  extern int  _FENV_PUBAPI fegetenv (fenv_t *__envp) throw ();
  extern int  _FENV_PUBAPI feholdexcept (fenv_t *__envp) throw ();
  extern int  _FENV_PUBAPI fesetenv (__const fenv_t *__envp) throw ();
  extern int  _FENV_PUBAPI feupdateenv (__const fenv_t *__envp) throw ();
#endif
#endif
#else
/* Non C++11 mode */
/* DQ (4/16/2017): Debugging test2017_33.C (part of mysql support, make this availalbe for C++ non C++11 mode for now) */

/* Note: I am not clear why the fenv.h header file is the problem that it is for our support in ROSE, but this does 
   appear to fix it for all cases I have encountered so far. But it would be better if we could just read the header 
file directly */

#ifdef __cplusplus
enum
  {
    FE_INVALID =
#define FE_INVALID	0x01
      FE_INVALID,
    __FE_DENORM = 0x02,
    FE_DIVBYZERO =
#define FE_DIVBYZERO	0x04
      FE_DIVBYZERO,
    FE_OVERFLOW =
#define FE_OVERFLOW	0x08
      FE_OVERFLOW,
    FE_UNDERFLOW =
#define FE_UNDERFLOW	0x10
      FE_UNDERFLOW,
    FE_INEXACT =
#define FE_INEXACT	0x20
      FE_INEXACT
  };

#define FE_ALL_EXCEPT (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

enum
  {
    FE_TONEAREST =
#define FE_TONEAREST	0
      FE_TONEAREST,
    FE_DOWNWARD =
#define FE_DOWNWARD	0x400
      FE_DOWNWARD,
    FE_UPWARD =
#define FE_UPWARD	0x800
      FE_UPWARD,
    FE_TOWARDZERO =
#define FE_TOWARDZERO	0xc00
      FE_TOWARDZERO
  };

/* Type representing exception flags.  */
typedef unsigned short int fexcept_t;

/* Type representing floating-point environment.  This structure
   corresponds to the layout of the block written by the `fstenv'
   instruction and has additional fields for the contents of the MXCSR
   register as written by the `stmxcsr' instruction.  */
#if 0
typedef struct
  {
    unsigned short int __control_word;
    unsigned short int __unused1;
    unsigned short int __status_word;
    unsigned short int __unused2;
    unsigned short int __tags;
    unsigned short int __unused3;
    unsigned int __eip;
    unsigned short int __cs_selector;
    unsigned int __opcode:11;
    unsigned int __unused4:5;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __unused5;
#ifdef __x86_64__
    unsigned int __mxcsr;
#endif
  }
fenv_t;
#else
// DQ (4/16/2017): This appears to be failing in the name qualification, so rewrite it as need to debug this.
#if 0
struct rose_fenv_tmp_struct
  {
    unsigned short int __control_word;
    unsigned short int __unused1;
    unsigned short int __status_word;
    unsigned short int __unused2;
    unsigned short int __tags;
    unsigned short int __unused3;
    unsigned int __eip;
    unsigned short int __cs_selector;
 // unsigned int __opcode:11;
 // unsigned int __unused4:5;
    unsigned int __opcode;
    unsigned int __unused4;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __unused5;
#ifdef __x86_64__
    unsigned int __mxcsr;
#endif
};

typedef rose_fenv_tmp_struct fenv_t;
#else
typedef unsigned    long fenv_t;
#endif
#endif

extern int feclearexcept (int __excepts) throw();
extern int fegetexceptflag (fexcept_t *__flagp, int __excepts) throw();
extern int feraiseexcept (int __excepts) throw();
extern int fesetexceptflag (const fexcept_t *__flagp, int __excepts) throw();
extern int fetestexcept (int __excepts) throw();
extern int fegetround (void) throw();
extern int fesetround (int __rounding_direction) throw();
extern int fegetenv (fenv_t *__envp) throw();
extern int feholdexcept (fenv_t *__envp) throw();
extern int fesetenv (const fenv_t *__envp) throw();
extern int feupdateenv (const fenv_t *__envp) throw();
extern int feenableexcept (int __excepts) throw();
extern int fedisableexcept (int __excepts) throw();
extern int fegetexcept (void) throw();
#endif

#endif

/* DQ (4/16/2016): Added Clang support.  Turn off the use of the type generic 
   math header file.  It is not yet clear what we should do with these.
 */
#ifdef __clang__
   #define __TGMATH_H
#endif

/* DQ (2/27/2017): Fixed to support GNU 4.6 and later compilers instead of just GNU 4.7 and later compilers. */
/* DQ (3/15/2015): Adding support for uint128 (required for Xen) */
/* This shuld be a test of the version of EDG, but it is not clear how to do that. */
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ < 6))
#ifndef __INTEL_COMPILER
#ifndef __clang__
   /* DQ (3/15/2015): Adding support for uint128 (required for Xen) */
      typedef long long int __int128_t;
      typedef unsigned long long int __uint128_t;
#endif
#endif
#endif

// TV (12/29/2018): EDG provides support for __float128 (ROSE-1604)
/* DQ (3/30/2017): I am unclear how general this is (this will fail for GNU compilers, so we have to only let ROSE see it). */
/* GNU: error: conflicting declaration 'typedef long double __float128' */
#ifndef ROSE_USE_EDG_QUAD_FLOAT
typedef long double __float128;
#else
// TV (01/06/2019): case of _Float128: fix from https://software.intel.com/en-us/forums/intel-c-compiler/topic/742701
#ifndef __cplusplus
#define _Float32 float
#define _Float64 double
#define _Float128 long double
#define _Float32x float
#define _Float64x double
#endif
#endif

/* DQ (10/22/2015): Change point at which this is applied from 4.8.3 to 4.8.0. */
/* DQ (9/23/2015): Adding support for avoid use of non-GNU language features in EDG frontend */
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8) && (__GNUC_PATCHLEVEL__ >= 0))
   #define _GTHREAD_USE_MUTEX_INIT_FUNC
   #define _GTHREAD_USE_RECURSIVE_MUTEX_INIT_FUNC
   #define _GTHREAD_USE_COND_INIT_FUNC
#endif

/* DQ (5/10/2016): Added support for GNU 5.x compilers (at least 5.1) */
/* DQ (7/23/2020): Later versions of EDG support this properly (EDG 6.0 and maybe EDG 5.0).
#if ((__GNUC__ > 4) && (__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 0))
   #undef __has_include(STR)
#endif
*/

/* DQ (10/22/2010): Isolate some of the OSX specific details. */
#ifdef __APPLE__
/* DQ (10/13/2010): This is a specific fix suggested by Cong Hou for an OSX problem. 
 * The use of __BLOCKS__ (being defined as a macro) controls the code that ROSE will 
 * see in the Mac OSX header files.  If it is defined then ROSE will try to parse 
 * some nonstandard code that represents a Max OSX (or Apple) language extension.
 * So in this fix we detect if we are on an OSX OS (thus using OSX header files)
 * and we undefine the __BLOCKS__ macro.
 */
#undef __BLOCKS__

/* DQ (1/16/2017): define this for the mac since it does not have a builtin definition. */
#define __builtin_va_arg(v,l) (l)(sizeof(l))

/* Required builtins for Clang 3.8 support on Mac 
    __c11_atomic_init
    __c11_atomic_thread_fence
    __c11_atomic_signal_fence
    __c11_atomic_is_lock_free (The argument is the size of the _Atomic(...) object, instead of its address)
    __c11_atomic_store
    __c11_atomic_load
    __c11_atomic_exchange
    __c11_atomic_compare_exchange_strong
    __c11_atomic_compare_exchange_weak
    __c11_atomic_fetch_add
    __c11_atomic_fetch_sub
    __c11_atomic_fetch_and
    __c11_atomic_fetch_or
    __c11_atomic_fetch_xor
*/

// DQ (1/16/2017): Note that we cannot seem to reset the value of the _GNUC_VER macro value.
// So the best solution is to make a copy of the Mac specifi hdrs1/atomic header file.

// #define _GNUC_VER 408
// #if !__has_feature(cxx_atomic) && _GNUC_VER < 407
// #if _GNUC_VER >= 407
//    #warning _GNUC_VER greater then 407
// #endif

#if __cplusplus
bool __c11_atomic_is_lock_free(__SIZE_TYPE__ x);
#endif

void __atomic_thread_fence (int memorder);
void __atomic_signal_fence (int memorder);
#define __atomic_exchange(ptr,val,ret,memorder) *(ptr)
#define __atomic_store(ptr,val,memorder)


/* Clang specific builtins for ARM instructions (these would ahve to be template functions, and specific to C++)
T __builtin_arm_ldrex(const volatile T *addr);
T __builtin_arm_ldaex(const volatile T *addr);
int __builtin_arm_strex(T val, volatile T *addr);
int __builtin_arm_stlex(T val, volatile T *addr);
void __builtin_arm_clrex(void);
*/

#endif //#ifdef __APPLE__


#if defined(__clang__)

/* DQ (2/27/2017): These are required to support Clang 3.5.0 using Boost 1.55. */
unsigned long long __builtin_ia32_rdpmc(int x);
unsigned long long __builtin_ia32_rdtsc(void);
unsigned long long __builtin_ia32_rdtscp(unsigned int *__A);

#endif


/* DQ (1/28/2017): Adding support for builtin va args required on at least on GNU 4.8 and 4.9 compilers. 
   A better solution is to exclude tests using this builtin for EDG version 4.12.
__builtin_va_list & __builtin_next_arg(char const *format); 
 */

/* This does not work since the include path may not be set to include this file. 
   Also including this is problematic since it would be used in both the compilation 
   of ROSE source code and source code using ROSE (which is awkward).
 */
/* Include rose_config.h (so that we can see and use the macro definitions) */
/* #include "rose_config.h" */

/* DQ (7/11/2009): fix emacs coloring... with a '$' */

/* Turn on use of restrict in EDG front-end using --edg:restrict */
#ifdef __GNUC__
/* for GNU g++ */
#define restrict __restrict__
#endif

#if 0
/* DQ (1/20/2014): This is better handled by generated the expressions for size dependent 
   expression instead of the constant folded values. This is now better implemented where
   it was an issue for bit field widths. 
 */
/* DQ (1/20/2014): Added support for -m32 mode and the associated associated size of size_t. */
#ifdef ROSE_M32BIT
/* Build a 32-bit version of size_t using "int32_t". */
 #include<stdint.h>
  typedef int32_t size_t;
#else
/* The 64-bit version of "size_t" is not built by default (stddef.h must be include). */
#endif
#endif

/* Defined language modes in ROSE (permits control over what builting functions are defined) */
#define ROSE_C_LANGUAGE_MODE      0
#define ROSE_CXX_LANGUAGE_MODE    1
#define ROSE_CUDA_LANGUAGE_MODE   2
#define ROSE_OPENCL_LANGUAGE_MODE 3

#ifndef ROSE_LANGUAGE_MODE
  #error "Macro ROSE_LANGUAGE_MODE should have been defined ROSE_LANGUAGE_MODE == 0 for C and C99 and ROSE_LANGUAGE_MODE == 1 for C++ (CUDA == 2, OpenCL == 3)"
#endif

/* DQ (9/1/2016): Adding support for ROSE generated builtin functions */
// #include "rose_generated_builtin_functions.h"

/* DQ (9/6/2016): We need size_t to be defined, but we only want it defined so that the builtin functions 
   will make sense, then we will undefine it and allow the header files to define it properly.
 */
/* typedef long unsigned int size_t; */
/* typedef __SIZE_TYPE__ size_t; */
#define size_t __SIZE_TYPE__

/*Pei-Hung (02/07/2020) added builtin for Clang*/
#if defined(__clang__)
#if (ROSE_LANGUAGE_MODE == ROSE_C_LANGUAGE_MODE)
  typedef __WCHAR_TYPE__ wchar_t;
#endif
  const char * __builtin_char_memchr ( const void * ptr, int value, size_t num );
  int __builtin_wmemcmp ( const void * ptr1, const void * ptr2, size_t num );
  size_t __builtin_wcslen ( const wchar_t * str );
  const wchar_t * __builtin_wmemchr ( const void * ptr, int value, size_t num );
#endif

/* DQ (9/2/2016): This is part of what is required using the new automated support for builtin functions. */
#if (ROSE_LANGUAGE_MODE == ROSE_CXX_LANGUAGE_MODE)
/* C++ case is different because the header files will define a complex type (see hdrs1/complex) */

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ < 4))
/* DQ (5/02/2016): These need to be defined away for the case of GNU 4.2 (and I think anything less than GNU 4.4). */
/* Note that Clang will pertend to be GNU 4.2.1. */
#ifndef __clang__
  #define __complex__
  #define __real__ 
  #define __imag__ 
  #define _Complex
#endif

/* DQ (9/13/2016): The GNU 4.2 compiler requires this to be turned off (fails for test2011_133.C). */
#undef __SSE2__

#endif

/* DQ (10/13/2012): Added to support complex with EDG 4.x work (defines away _Imaginary, I
   don't like this fix).  This is required when constant folding is turned off.
 */
/* #define _Imaginary __imag__ */
#else
/* This works for both C and C99 modes */
  #define __complex__ _Complex
  #define __real__
  #define __imag__

/* DQ (10/13/2012): Added to support complex with EDG 4.x work (defines away _Imaginary, I
   don't like this fix).  This is required when constant folding is turned off.
 */
  #define _Imaginary __imag__
#endif

/* gcc uses the C99 name _Complex_I in <complex.h>, but our EDG doesn't handle
   the GCC extension that they define it to. */
#ifdef __INTEL_COMPILER
/* #define _Complex_I __I__ */
#else
#define _Complex_I __I__
#endif

/* Disable inclusion of complex.h on Linux */
/* #define _COMPLEX_H */
/* Disable inclusion of complex.h on Mac OS X */
/* #define __COMPLEX__ */

/* Defined this to avoid warnings (e.g. test2001_11.C) from 3.4.6 systems header files. */
/* DQ (9/6/2016): This is also required to support tests/nonsmoke/functional/CompileTests/mergeAST_tests/mergeTest_134.C */
#define __weakref__(NAME)

/* For debugging use the separate generated header file of builtin functions. */
#if __cplusplus >= 201103L
#if defined(__EDG_VERSION__) && __EDG_VERSION__ == 409
// DQ (12/16/2016): Added constexpr for EDG 4.9 using c++11.
// #define ROSE_CONSTEXPR
   #define ROSE_CONSTEXPR constexpr
#else
   #define ROSE_CONSTEXPR constexpr
#endif
// #warning "Set ROSE_CONSTEXPR to constexpr"
#else
#  define ROSE_CONSTEXPR
// #warning "NOT setting ROSE_CONSTEXPR to constexpr"
#endif


// DQ (3/12/2017): Re-comment these out for EDG 4.9 (required for Clang C++ but not C tests).
// DQ (12/16/2016): Comment these out for EDG 4.9.
// #if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 409
// #if defined(__EDG_VERSION__) && (__EDG_VERSION__ > 409 || (__EDG_VERSION__ == 409 && defined (__clang__) && defined(__cplusplus)))
#if defined(__EDG_VERSION__) && (__EDG_VERSION__ > 409 || (__EDG_VERSION__ == 409 && defined(__cplusplus)))

/* Note that might not be using our builtin function mechanism for EDG 4.12 and later (at least while we sort it out) */
// $REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS

// DQ (12/16/2016): Comment these out for EDG 4.9.
#endif


/* DQ (9/12/2016): Added builtin functions (not yet supported in config/Builtins.def).
   These are required to compile: tests/nonsmoke/functional/CompileTests/Cxx11_tests/test2015_08.C
   Note that these are not in our ROSE specific table of builtin functions at present.
 */
// #if 0
// DQ (3/10/2017): Include these for EDG 4.9 and later.
// DQ (12/16/2016): Comment these out for EDG 4.9.
// #if defined(__EDG_VERSION__) && __EDG_VERSION__ == 409
#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 409
  // long long          __builtin_bswap64(long long __X);
void               __builtin_ia32_fxsave(void *__P);
void               __builtin_ia32_fxrstor(void *__P);
void               __builtin_ia32_fxsave64(void *__P);
void               __builtin_ia32_fxrstor64(void *__P);

// DQ (3/10/2017): These conflict with these declarations repeated further below.
// int __builtin_signbitf(float) ROSE_GNUATTR((nothrow, const));
// int __builtin_signbitl(long double) ROSE_GNUATTR((nothrow, const));
#endif


// DQ (12/16/2016): This is incorrect for EDG 4.9, but required for later versions of EDG.
#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 411
unsigned long long int __builtin_bswap64(unsigned long long int) ROSE_GNUATTR((nothrow, const));
#endif

// DQ (12/16/2016): Skip these for EDG 4.9.
#if defined(__EDG_VERSION__) && __EDG_VERSION__ > 409
ROSE_CONSTEXPR double __builtin_fabs(double) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR float __builtin_fabsf(float) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR long double __builtin_fabsl(long double) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR float __builtin_powf(float, float) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR long double __builtin_powl(long double, long double) ROSE_GNUATTR((nothrow, const));

#ifdef __cplusplus
ROSE_CONSTEXPR int __builtin_isfinite(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isinf(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isnan(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isnormal(...) ROSE_GNUATTR((nothrow, const));
#endif


#ifdef __cplusplus
// DQ (2/21/2017): These functions are not defined for C language support.
ROSE_CONSTEXPR int __builtin_isgreater(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isgreaterequal(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isless(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_islessequal(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_islessgreater(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isunordered(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_fpclassify(int, int, int, int, ...) ROSE_GNUATTR((nothrow, const));
int __builtin_isinf_sign(...) ROSE_GNUATTR((nothrow, const));
#endif

#ifdef __cplusplus
// DQ (2/21/2017): These functions are not defined for C language support.
int __builtin_constant_p(...) ROSE_GNUATTR((nothrow, const));
int __builtin_classify_type(...) ROSE_GNUATTR((nothrow, const));
void __builtin_va_start(__builtin_va_list&, ...) ROSE_GNUATTR((nothrow));
void __builtin_va_end(__builtin_va_list&) ROSE_GNUATTR((nothrow));

// DQ (4/16/2017): This appears to fix the issue with "__builtin_va_list &" (not const-qualified) 
// cannot be initialized with a value of type "__va_list_tag *" __builtin_va_copy(args_copy,args);
// void __builtin_va_copy(__builtin_va_list&, __builtin_va_list&) ROSE_GNUATTR((nothrow));
void __builtin_va_copy(__builtin_va_list&, __builtin_va_list) ROSE_GNUATTR((nothrow));

void __builtin_stdarg_start(__builtin_va_list&, ...) ROSE_GNUATTR((nothrow));
#endif


#ifdef __cplusplus
// DQ (2/21/2017): These functions are not defined for C language support.
void __builtin_prefetch(void const*, ...) ROSE_GNUATTR((nothrow, const));
#endif

#ifdef __cplusplus
// DQ (2/21/2017): These functions are not defined for C language support.
void __sync_fetch_and_add(...);
char __sync_fetch_and_add_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_fetch_and_add_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_fetch_and_add_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_fetch_and_add_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_fetch_and_add_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_fetch_and_sub(...);
char __sync_fetch_and_sub_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_fetch_and_sub_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_fetch_and_sub_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_fetch_and_sub_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_fetch_and_sub_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_fetch_and_or(...);
char __sync_fetch_and_or_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_fetch_and_or_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_fetch_and_or_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_fetch_and_or_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_fetch_and_or_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_fetch_and_and(...);
char __sync_fetch_and_and_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_fetch_and_and_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_fetch_and_and_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_fetch_and_and_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_fetch_and_and_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_fetch_and_xor(...);
char __sync_fetch_and_xor_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_fetch_and_xor_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_fetch_and_xor_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_fetch_and_xor_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_fetch_and_xor_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));

// DQ (3/9/2017): Another builtin function from mysql.
// void __sync_add_and_fetch(...);
long int __sync_add_and_fetch(volatile long int * word, unsigned long int ammount);
unsigned int __sync_add_and_fetch (volatile unsigned int * word, unsigned long int ammount);

// DQ (3/15/2017): Adding support for mysql required builtins.
unsigned long int __sync_add_and_fetch(volatile unsigned long int * word, unsigned long int ammount);


char __sync_add_and_fetch_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_add_and_fetch_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_add_and_fetch_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_add_and_fetch_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_add_and_fetch_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));

// DQ (3/9/2017): Another builtin function from mysql.
// void __sync_sub_and_fetch(...);
long int __sync_sub_and_fetch(volatile long int * word, unsigned long int ammount);
unsigned int __sync_sub_and_fetch (volatile unsigned int * word, unsigned long int ammount);

// DQ (3/15/2017): Adding support for mysql required builtins.
unsigned long int __sync_sub_and_fetch (volatile unsigned long int * word, unsigned long int ammount);

char __sync_sub_and_fetch_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_sub_and_fetch_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_sub_and_fetch_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_sub_and_fetch_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_sub_and_fetch_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_or_and_fetch(...);
char __sync_or_and_fetch_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_or_and_fetch_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_or_and_fetch_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_or_and_fetch_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_or_and_fetch_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_and_and_fetch(...);
char __sync_and_and_fetch_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_and_and_fetch_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_and_and_fetch_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_and_and_fetch_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_and_and_fetch_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_xor_and_fetch(...);
char __sync_xor_and_fetch_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_xor_and_fetch_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_xor_and_fetch_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_xor_and_fetch_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_xor_and_fetch_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));

// DQ (2/26/2017): The generated builtin is required to have a bool type.
// void __sync_bool_compare_and_swap(...);
bool __sync_bool_compare_and_swap(...);

void __sync_val_compare_and_swap(...);

// DQ (3/9/2017): Define these are macros and then undefine them below to avoid conflicts.
// typedef __INT32_TYPE__ int32;
// typedef __INT64_TYPE__ int64;
// #define int32 __INT32_TYPE__
// #define int64 __INT64_TYPE__
#define local_int32 int
#define local_int64 long long int

// DQ (3/9/2017): Added builtins for mysql use of gcc_syn.h header file.
local_int32 __sync_val_compare_and_swap(local_int32 volatile *a, local_int32 cmp,local_int32 set);
local_int64 __sync_val_compare_and_swap(local_int64 volatile *a, local_int64 cmp, local_int64 set);
void* __sync_val_compare_and_swap(void volatile *a, void* cmp, void* set);
local_int32 __sync_fetch_and_add( local_int32 volatile *__mem, local_int32 __val);
local_int64 __sync_fetch_and_add( local_int64 volatile *__mem, local_int64 __val);
local_int32 __sync_lock_test_and_set( local_int32 volatile *__mem, local_int32 __val);
local_int64 __sync_lock_test_and_set( local_int64 volatile *__mem, local_int64 __val);
void* __sync_lock_test_and_set( void volatile *__mem, void* __val);
local_int32 __sync_fetch_and_or( local_int32 volatile *__mem, local_int32 __val);
local_int64 __sync_fetch_and_or( local_int64 volatile *__mem, local_int64 __val);
void* __sync_fetch_and_or( void volatile *__mem, local_int64 __val);

#undef local_int32
#undef local_int64

char __sync_val_compare_and_swap_1(char volatile*, char, char, ...) ROSE_GNUATTR((nothrow));
short __sync_val_compare_and_swap_2(short volatile*, short, short, ...) ROSE_GNUATTR((nothrow));
int __sync_val_compare_and_swap_4(int volatile*, int, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_val_compare_and_swap_8(long long int volatile*, long long int, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_val_compare_and_swap_16(__int128_t volatile*, __int128_t, __int128_t, ...) ROSE_GNUATTR((nothrow));
char __sync_lock_test_and_set_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_lock_test_and_set_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_lock_test_and_set_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_lock_test_and_set_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_lock_test_and_set_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_lock_release(...);
void __sync_lock_release_1(char volatile*, ...) ROSE_GNUATTR((nothrow));
void __sync_lock_release_2(short volatile*, ...) ROSE_GNUATTR((nothrow));
void __sync_lock_release_4(int volatile*, ...) ROSE_GNUATTR((nothrow));
void __sync_lock_release_8(long long int volatile*, ...) ROSE_GNUATTR((nothrow));
void __sync_lock_release_16(__int128_t volatile*, ...) ROSE_GNUATTR((nothrow));
void __sync_swap(...);
char __sync_swap_1(char volatile*, char, ...) ROSE_GNUATTR((nothrow));
short __sync_swap_2(short volatile*, short, ...) ROSE_GNUATTR((nothrow));
int __sync_swap_4(int volatile*, int, ...) ROSE_GNUATTR((nothrow));
long long int __sync_swap_8(long long int volatile*, long long int, ...) ROSE_GNUATTR((nothrow));
__int128_t __sync_swap_16(__int128_t volatile*, __int128_t, ...) ROSE_GNUATTR((nothrow));
void __sync_synchronize(...) ROSE_GNUATTR((nothrow));
#endif

/* DQ (4/23/2017): This is now added on the EDG command line to support handling of Microsoft options.
   This supports analysis of Microsofte specific code on Linuc platforms.  It is not clear now specific
   our support can be since we can't interogate the Microsoft compiler for some specifics.
 */
#ifndef ROSE_USE_MICROSOFT_EXTENSIONS
#ifdef __cplusplus
// DQ (2/21/2017): These functions are not defined for C language support.
void __noop(...) ROSE_GNUATTR((nothrow));
#endif
#endif

//Builtin Function Call support
void* __builtin_apply_args();
void* __builtin_apply(void (*function)(), void *arguments, size_t size);
void  __builtin_return(void* result);

#if defined(__INTEL_COMPILER) && (__cplusplus >= 201400L)
// DQ (2/21/2017): Debugging C++14 support specific to the Intel compiler.
extern "C" char* gets (char* __s) ROSE_GNUATTR((deprecated));
// char* gets (char* __s) ROSE_GNUATTR((deprecated));
#endif


// This need to be const expr in EDG 4.12
ROSE_CONSTEXPR double __builtin_huge_val() ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR float __builtin_huge_valf() ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR long double __builtin_huge_vall() ROSE_GNUATTR((nothrow, const));

ROSE_CONSTEXPR double __builtin_nan(char const*) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR float __builtin_nanf(char const*) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR long double __builtin_nanl(char const*) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR double __builtin_nans(char const*) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR float __builtin_nansf(char const*) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR long double __builtin_nansl(char const*) ROSE_GNUATTR((nothrow, const));

// DQ (12/16/2016): evening: allow these to be constexpr.
ROSE_CONSTEXPR int __builtin_clz(unsigned int) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_clzl(unsigned long int) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_clzll(unsigned long long int) ROSE_GNUATTR((nothrow, const));

ROSE_CONSTEXPR int __builtin_signbitf(float) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_signbitl(long double) ROSE_GNUATTR((nothrow, const));

ROSE_CONSTEXPR unsigned short  __builtin_bswap16(unsigned short x) ROSE_GNUATTR((nothrow, const));

#if __cplusplus
ROSE_CONSTEXPR bool            __builtin_is_constant_evaluated  ()                 ROSE_GNUATTR((nothrow, const));
#endif

#endif

/* DQ (7/24/2020): This code is no longer required now that the EDG/ROSE connection is fixed for at least EDG 6.0 (maybe EDG 5.0). */
/* DQ (7/23/2020): This fixes a bug demonstrated with GNU 10 using make check-core and/or make check-smoke. */
/* #if __GNUC__ >= 10 && __cplusplus */
#if 0
template <typename T, typename U>
struct __rose_is_same_as {
  static constexpr bool value = false;
};

template <typename T>
struct __rose_is_same_as<T,T> {
  static constexpr bool value = true;
};

#define __is_same_as(T,U) __rose_is_same_as<T,U>::value
#endif


/* DQ (7/26/2020): Adding GNU 10 C++20 specific builtin functions (part of what 
is needed to compile C++20 specific headers, the rest are some modified headers). */
#if __GNUC__ >= 10 && __cplusplus
bool __builtin_coro_done(void* _M_fr_ptr);
void __builtin_coro_resume(void* _M_fr_ptr);
void __builtin_coro_destroy(void* _M_fr_ptr);
// std::compare_three_way()(__a.address(), __b.address());
void* __builtin_coro_promise(void* _M_fr_ptr, unsigned long, bool);
ROSE_CONSTEXPR bool __atomic_always_lock_free(unsigned long, unsigned long);
// extern unsigned long ATOMIC_CHAR8_T_LOCK_FREE;
const unsigned long __GCC_ATOMIC_CHAR8_T_LOCK_FREE = 2;
#endif


/* DQ (9/2/2016): Comment out to use new automated generation of builtin functions. */
#if 0

// DQ (12/16/2016): Comment these out for EDG 4.9.
// #if defined(__EDG_VERSION__) && __EDG_VERSION__ == 409

/* DQ (4/12/2015): Removing this mapping for __builtin_va_start to __builtin_stdarg_start
   since EDG now supports __builtin_va_start directly.
 */
/* PC (9/25/2006): Define __builtin_va_start to the EDG expected symbol
   __builtin_stdarg_start */
/* #define __builtin_va_start __builtin_stdarg_start */

/* DQ (8/28/2012): This is an EDG limitation (stricter interpretation of C and C++, I gather).
   EDG 4.4 no longer supports the GNU extension ``__thread'' so we have to disable this
   by setting it with a macro to defin it away (to nothing).  See the details in the 
   EDG/src/Changes file. This is tested in: C_tests/test2009_19.c.
*/
/* DQ (5/24/2015): We need to support this how as part of improved C level support in ROSE. */
/* #define __thread */

/* Outside strict ISO C mode (-ansi, -std=c89 or -std=c99), the functions _exit, alloca,
    bcmp, bzero, dcgettext, dgettext, dremf, dreml, drem, exp10f, exp10l, exp10, ffsll,
    ffsl, ffs, fprintf_unlocked, fputs_unlocked, gammaf, gammal, gamma, gettext, index,
    isascii, j0f, j0l, j0, j1f, j1l, j1, jnf, jnl, jn, mempcpy, pow10f, pow10l, pow10,
    printf_unlocked, rindex, scalbf, scalbl, scalb, signbit, signbitf, signbitl,
    significandf, significandl, significand, sincosf, sincosl, sincos, stpcpy, strdup,
    strfmon, toascii, y0f, y0l, y0, y1f, y1l, y1, ynf, ynl and yn may be handled as
    built-in functions. All these functions have corresponding versions prefixed with
    __builtin_, which may be used even in strict C89 mode.
*/
/* extern void _exit ( int Status); */

/* The ISO C99 functions _Exit, acoshf, acoshl, acosh, asinhf, asinhl, asinh, atanhf,
    atanhl, atanh, cabsf, cabsl, cabs, cacosf, cacoshf, cacoshl, cacosh, cacosl, cacos,
    cargf, cargl, carg, casinf, casinhf, casinhl, casinh, casinl, casin, catanf, catanhf,
    catanhl, catanh, catanl, catan, cbrtf, cbrtl, cbrt, ccosf, ccoshf, ccoshl, ccosh,
    ccosl, ccos, cexpf, cexpl, cexp, cimagf, cimagl, cimag, conjf, conjl, conj, copysignf,
    copysignl, copysign, cpowf, cpowl, cpow, cprojf, cprojl, cproj, crealf, creall, creal,
    csinf, csinhf, csinhl, csinh, csinl, csin, csqrtf, csqrtl, csqrt, ctanf, ctanhf,
    ctanhl, ctanh, ctanl, ctan, erfcf, erfcl, erfc, erff, erfl, erf, exp2f, exp2l, exp2,
    expm1f, expm1l, expm1, fdimf, fdiml, fdim, fmaf, fmal, fmaxf, fmaxl, fmax, fma, fminf,
    fminl, fmin, hypotf, hypotl, hypot, ilogbf, ilogbl, ilogb, imaxabs, isblank, iswblank,
    lgammaf, lgammal, lgamma, llabs, llrintf, llrintl, llrint, llroundf, llroundl,
    llround, log1pf, log1pl, log1p, log2f, log2l, log2, logbf, logbl, logb, lrintf,
    lrintl, lrint, lroundf, lroundl, lround, nearbyintf, nearbyintl, nearbyint,
    nextafterf, nextafterl, nextafter, nexttowardf, nexttowardl, nexttoward, remainderf,
    remainderl, remainder, remquof, remquol, remquo, rintf, rintl, rint, roundf, roundl,
    round, scalblnf, scalblnl, scalbln, scalbnf, scalbnl, scalbn, snprintf, tgammaf,
    tgammal, tgamma, truncf, truncl, trunc, vfscanf, vscanf, vsnprintf and vsscanf are
    handled as built-in functions except in strict ISO C90 mode (-ansi or -std=c89). 
*/
/* extern void _Exit ( int Status); */
double      __builtin_copysign  (double __builtin__x, double __builtin__y);
float       __builtin_copysignf (float __builtin__x, float __builtin__y);
long double __builtin_copysignl (long double __builtin__x, long double __builtin__y);

/* There are also built-in versions of the ISO C99 functions acosf, acosl, asinf, asinl,
    atan2f, atan2l, atanf, atanl, ceilf, ceill, cosf, coshf, coshl, cosl, expf, expl,
    fabsf, fabsl, floorf, floorl, fmodf, fmodl, frexpf, frexpl, ldexpf, ldexpl, log10f,
    log10l, logf, logl, modfl, modf, powf, powl, sinf, sinhf, sinhl, sinl, sqrtf, sqrtl,
    tanf, tanhf, tanhl and tanl that are recognized in any mode since ISO C90 reserves
    these names for the purpose to which ISO C99 puts them. All these functions have
    corresponding versions prefixed with __builtin_. 
*/

/* DQ (4/12/2005): Required to compile the gnu version 3.4.3 cmath header file. Defined
//                 as functions instead of macros to avoid constant propagation issues.
*/
float       __builtin_acosf  (float __builtin__x);
long double __builtin_acosl  (long double __builtin__x);
float       __builtin_asinf  (float __builtin__x);
long double __builtin_asinl  (long double __builtin__x);
float       __builtin_atanf  (float __builtin__x);
long double __builtin_atanl  (long double __builtin__x);
float       __builtin_atan2f (float __builtin__x,float __builtin__y);
long double __builtin_atan2l (long double __builtin__x,long double __builtin__y);
float       __builtin_ceilf  (float __builtin__x);
long double __builtin_ceill  (long double __builtin__x);
float       __builtin_coshf  (float __builtin__x);
long double __builtin_coshl  (long double __builtin__x);
float       __builtin_floorf (float __builtin__x);
long double __builtin_floorl (long double __builtin__x);
float       __builtin_fmodf  (float __builtin__x,float __builtin__y);
long double __builtin_fmodl  (long double __builtin__x,long double __builtin__y);
float       __builtin_frexpf (float __builtin__x,int *__builtin__y);
long double __builtin_frexpl (long double __builtin__x,int *__builtin__y);
float       __builtin_ldexpf (float __builtin__x,float __builtin__y);
long double __builtin_ldexpl (long double __builtin__x,long double __builtin__y);
float       __builtin_log10f (float __builtin__x);
long double __builtin_log10l (long double __builtin__x);
float       __builtin_modff  (float __builtin__x,float *__builtin__y);
long double __builtin_modfl  (long double __builtin__x,long double *__builtin__y);
float       __builtin_powf   (float __builtin__x,float __builtin__y);
long double __builtin_powl   (long double __builtin__x,long double __builtin__y);
float       __builtin_sinhf  (float __builtin__x);
long double __builtin_sinhl  (long double __builtin__x);
float       __builtin_tanf   (float __builtin__x);
long double __builtin_tanl   (long double __builtin__x);
float       __builtin_tanhf  (float __builtin__x);
long double __builtin_tanhl  (long double __builtin__x);

/* DQ (5/15/2006): Suggested by Christian Biesinger (working with Markus Schordan) */
long double __builtin_powil  (long double __builtin__x, int __builtin__i);
double      __builtin_powi   (double __builtin__x, int __builtin__i);
float       __builtin_powif  (float __builtin__x, int __builtin__i);


/* The ISO C94 functions iswalnum, iswalpha, iswcntrl, iswdigit, iswgraph, iswlower,
    iswprint, iswpunct, iswspace, iswupper, iswxdigit, towlower and towupper are handled
    as built-in functions except in strict ISO C90 mode (-ansi or -std=c89). 
*/

/* The ISO C90 functions abort, abs, acos, asin, atan2, atan, calloc, ceil, cosh, cos,
    exit, exp, fabs, floor, fmod, fprintf, fputs, frexp, fscanf, isalnum, isalpha,
    iscntrl, isdigit, isgraph, islower, isprint, ispunct, isspace, isupper, isxdigit,
    tolower, toupper, labs, ldexp, log10, log, malloc, memcmp, memcpy, memset, modf, pow,
    printf, putchar, puts, scanf, sinh, sin, snprintf, sprintf, sqrt, sscanf, strcat,
    strchr, strcmp, strcpy, strcspn, strlen, strncat, strncmp, strncpy, strpbrk, strrchr,
    strspn, strstr, tanh, tan, vfprintf, vprintf and vsprintf are all recognized as
    built-in functions unless -fno-builtin is specified (or -fno-builtin-function is
    specified for an individual function). All of these functions have corresponding
    versions prefixed with __builtin_.
*/


/* DQ (7/29/2005): declarations for builtin functions used by GNU, but 
// already defined in EDG (it seems that we can provide declarations for 
// them explicitly).  These should be marked as compiler generated in 
// the AST.
*/
char *      __builtin_strchr (const char *__builtin__s, int __builtin__c);
char *      __builtin_strrchr(const char *__builtin__s, int __builtin__c);
char *      __builtin_strpbrk(const char *__builtin__s, const char *__builtin__accept);
char *      __builtin_strstr (const char *__builtin__haystack, const char *__builtin__needle);
float       __builtin_nansf  (const char *__builtin__x);
double      __builtin_nans   (const char *__builtin__x);
long double __builtin_nansl  (const char *__builtin__x);
double      __builtin_fabs   (double      __builtin__x);
float       __builtin_fabsf  (float       __builtin__x);
long double __builtin_fabsl  (long double __builtin__x);
float       __builtin_cosf   (float       __builtin__x);
long double __builtin_cosl   (long double __builtin__x);
float       __builtin_sinf   (float       __builtin__x);
long double __builtin_sinl   (long double __builtin__x);
float       __builtin_sqrtf  (float       __builtin__x);
long double __builtin_sqrtl  (long double __builtin__x);

/* DQ (9/2/2016): Comment out to use new automated generation of builtin functions. */
#endif


/* DQ (11/20/2016): This should be implemented only for EDG 4.11. */
#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 412
/* DQ (11/17/2016): Added support for more builtin functions (not in EDG, required by 6.1 of GNU. */
// #if (__GNUC__ >= 6)

// DQ (1/19/2017): Make the use of at least __builtin_va_arg available to the GNU 4.x compilers when using EDG 4.12.
// DQ (12/17/2016): I think this has to be used for GNU version 5.x as well.
// #if (__GNUC__ >= 6) || defined(__INTEL_COMPILER)
// #if (__GNUC__ >= 5) || defined(__INTEL_COMPILER)
#if (__GNUC__ >= 4) || defined(__INTEL_COMPILER)

// DQ (3/11/2017): Added Clang specific support to skip AVX header files.
#if defined(__clang__)

// Note that clang macro names for include guards are different!
 
/* Test how many files of builtins we have to add */
   #define __SMMINTRIN_H
   #define __AMMINTRIN_H
   #define __TMMINTRIN_H
   #define __PMMINTRIN_H

   #define __AVX2INTRIN_H
// #define __AVX512FINTRIN_H
   #define __AVX512FINTRIN_H

   #define __AVX512CDINTRIN_H
   #define __AVX512VLINTRIN_H
   #define __AVX512BWINTRIN_H
   #define __AVX512DQINTRIN_H
   #define __AVX512VLBWINTRIN_H
   #define __AVX512VLDQINTRIN_H
   #define __AVX512IFMAINTRIN_H
   #define __AVX512IFMAVLINTRIN_H
   #define __AVX512VBMIINTRIN_H
   #define __AVX512VBMIVLINTRIN_H
   #define __SHAINTRIN_H
   #define __LZCNTINTRIN_H
   #define __BMIINTRIN_H
   #define __BMI2INTRIN_H
   #define __FMAINTRIN_H
   #define __F16CINTRIN_H
   #define __RTMINTRIN_H
   #define __XTESTINTRIN_H

/* This just includes a lot of other include files specific to AVX (partially addressed above) */
/* #define _IMMINTRIN_H_INCLUDED */

   #define __MM3DNOW_H
   #define __FMA4INTRIN_H
   #define __XOPMMINTRIN_H
   #define __LWPINTRIN_H
   #define __RDSEEDINTRIN_H
   #define __XSAVEINTRIN_H
   #define __XSAVEOPTINTRIN_H
   #define __ADXINTRIN_H
   #define __CLWBINTRIN_H
   #define __PCOMMITINTRIN_H
   #define __CLFLUSHOPTINTRIN_H
   #define __XSAVESINTRIN_H
   #define __XSAVECINTRIN_H
   #define __MWAITXINTRIN_H
   #define __CLZEROINTRIN_H
   #define __PKUINTRIN_H

// Need one for than for non-clang compilers.
   #define __XOPINTRIN_H
#endif


/* Test how many files of builtins we have to add */
   #define _SMMINTRIN_H_INCLUDED
   #define _AMMINTRIN_H_INCLUDED
   #define _TMMINTRIN_H_INCLUDED
   #define _PMMINTRIN_H_INCLUDED

   #define _AVX2INTRIN_H_INCLUDED
   #define _AVX512FINTRIN_H_INCLUDED
   #define _AVX512CDINTRIN_H_INCLUDED
   #define _AVX512VLINTRIN_H_INCLUDED
   #define _AVX512BWINTRIN_H_INCLUDED
   #define _AVX512DQINTRIN_H_INCLUDED
   #define _AVX512VLBWINTRIN_H_INCLUDED
   #define _AVX512VLDQINTRIN_H_INCLUDED
   #define _AVX512IFMAINTRIN_H_INCLUDED
   #define _AVX512IFMAVLINTRIN_H_INCLUDED
   #define _AVX512VBMIINTRIN_H_INCLUDED
   #define _AVX512VBMIVLINTRIN_H_INCLUDED
   #define _SHAINTRIN_H_INCLUDED
   #define _LZCNTINTRIN_H_INCLUDED
   #define _BMIINTRIN_H_INCLUDED
   #define _BMI2INTRIN_H_INCLUDED
   #define _FMAINTRIN_H_INCLUDED
   #define _F16CINTRIN_H_INCLUDED
   #define _RTMINTRIN_H_INCLUDED
   #define _XTESTINTRIN_H_INCLUDED

/* This just includes a lot of other include files specific to AVX (partially addressed above) */
/* #define _IMMINTRIN_H_INCLUDED */

   #define _MM3DNOW_H_INCLUDED
   #define _FMA4INTRIN_H_INCLUDED
   #define _XOPMMINTRIN_H_INCLUDED
   #define _LWPINTRIN_H_INCLUDED
   #define _RDSEEDINTRIN_H_INCLUDED
   #define _XSAVEINTRIN_H_INCLUDED
   #define _XSAVEOPTINTRIN_H_INCLUDED
   #define _ADXINTRIN_H_INCLUDED
   #define _CLWBINTRIN_H_INCLUDED
   #define _PCOMMITINTRIN_H_INCLUDED
   #define _CLFLUSHOPTINTRIN_H_INCLUDED
   #define _XSAVESINTRIN_H_INCLUDED
   #define _XSAVECINTRIN_H_INCLUDED
   #define _MWAITXINTRIN_H_INCLUDED
   #define _CLZEROINTRIN_H_INCLUDED
   #define _PKUINTRIN_H_INCLUDED

/* DQ (12/14/2016): Adding more evaluations of required builtin support. */
// #define _GLIBCXX_ATOMIC_BASE_H
// #define _GLIBCXX_ATOMIC 1
   # define _GLIBCXX_VISIBILITY(V) 
   # define _GLIBCXX_BEGIN_NAMESPACE_VERSION
   # define _GLIBCXX_END_NAMESPACE_VERSION
   void __atomic_thread_fence (int memorder);
   void __atomic_signal_fence (int memorder);
// bool __atomic_test_and_set (void *ptr, int memorder);
   #define __atomic_test_and_set(ptr,memorder) false
// void __atomic_clear (bool *ptr, int memorder);
   #define __atomic_clear(ptr,memorder)

// DQ (2/9/2017): Last parameter should be const volatile uint8_t* for processing boost files.
// See tests/CompileTests/RoseExample_tests/testRoseHeaders_01.C on GNU 4.9.3 in non-C++11 mode.
// bool __atomic_is_lock_free (size_t size, void *ptr);
// int __atomic_is_lock_free (size_t size, void *ptr);
// int __atomic_is_lock_free (size_t size, const volatile uint8_t*);
   int __atomic_is_lock_free (size_t size, void *ptr);
#ifdef __cplusplus
   int __atomic_is_lock_free (size_t size, const volatile unsigned char*);
   int __atomic_is_lock_free (size_t size, const volatile unsigned long*);

// DQ (3/11/2017): This is required for Boost 1.55 with tests/nonsmoke/functional/CompileTests/RoseExample_tests/testRoseHeaders_01.C.
// int __atomic_is_lock_free (unsigned long, const volatile uint16_t *);
   int __atomic_is_lock_free (size_t size, const volatile unsigned short*);
   int __atomic_is_lock_free (size_t size, const volatile unsigned int*);
   int __atomic_is_lock_free (size_t size, void *const volatile *);

// DQ (2/12/2017): Added to support C++11 test2014_04.C.
#if __cplusplus >= 201103L
// DQ (2/21/2017): We don't want to inject this type into the global namespace.
// typedef decltype(nullptr) nullptr_t;
// int __atomic_is_lock_free (size_t size, nullptr_t);
   int __atomic_is_lock_free (size_t size, decltype(nullptr));
#endif
#endif

// DQ (3/11/2017): This is required for Boost 1.55 with tests/nonsmoke/functional/CompileTests/RoseExample_tests/testRoseHeaders_01.C.
#define __atomic_store(ptr,val,memorder)

// void __atomic_store_n (type *ptr, type val, int memorder);
   #define __atomic_store_n(ptr,val,memorder)
   #define __atomic_load(ptr,ret,memorder)
   #define __atomic_load_n(ptr,memorder) *(ptr)
   #define __atomic_exchange_n(ptr,val,memorder) *(ptr)
   #define __atomic_exchange(ptr,val,ret,memorder) *(ptr)
   #define __atomic_compare_exchange_n(ptr,expected,desired,weak,success_memorder,failure_memorder) false
   long __builtin_labs(long i);
   long long __builtin_llabs(long long i);

// DQ (3/9/2017): Yet another builtin function requirement.
   #define __atomic_compare_exchange(ptr,expected,desired,weak,success_memorder,failure_memorder) false

// int __sync_lock_test_and_set( int & v, int n );
   int __sync_lock_test_and_set( int *v, int n );
// type __sync_lock_test_and_set (type *ptr, type value, ...)

// DQ (2/10/2017): Change __Cplusplus to the correct __cplusplus.
// DQ (12/15/2016): Need to address the use of C, not just C++.
// int __sync_lock_release( int & v );
#ifdef __cplusplus
   int __sync_lock_release( int & v );
#endif

/* 
DQ (1/31/2018): The original version is the only one that appears to work (except on new testcode: test2018_23.C).
DQ (1/27/2018): It appears that this is a better implementation to permit test2018_22.C to pass.
                However, we need to still explicitly support the builtin functions that take 
                types are argument.
DQ (12/15/2016): Adding more evaluations of required builtin support. 
*/
// #define __builtin_va_arg(v,l) (l)(sizeof(l))
// #define __builtin_va_arg(v,__type) ({__type __typeValue; __typeValue;})
#define __builtin_va_arg(v,l) (l)(sizeof(l))

// DQ (12/16/2016): Another iteration of adding builtin functions.
   #define __atomic_fetch_add(ptr,ret,memorder)  *(ptr)
   #define __atomic_fetch_sub(ptr,ret,memorder)  *(ptr)
   #define __atomic_fetch_and(ptr,ret,memorder)  *(ptr)
   #define __atomic_fetch_xor(ptr,ret,memorder)  *(ptr)
   #define __atomic_fetch_or(ptr,ret,memorder)   *(ptr)
   #define __atomic_fetch_nand(ptr,ret,memorder) *(ptr)

// DQ (12/17/2016): Another iteration of adding builtin functions.
   #define __atomic_add_fetch(ptr,ret,memorder)  *(ptr)
   #define __atomic_sub_fetch(ptr,ret,memorder)  *(ptr)
   #define __atomic_and_fetch(ptr,ret,memorder)  *(ptr)
   #define __atomic_xor_fetch(ptr,ret,memorder)  *(ptr)
   #define __atomic_or_fetch(ptr,ret,memorder)   *(ptr)
   #define __atomic_nand_fetch(ptr,ret,memorder) *(ptr)

#endif
#endif

/* DQ (11/20/2016): This should be implemented only for EDG 4.11. */
#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 411
/* DQ (9/12/2016): Comment out to use new automated generation of builtin functions. 
   However, these are required for EDG 4.11 to handle C++11 test codes.
 */

// DQ (3/10/2017): Make this work with Clang to fix a few missing builtin functions.
/* DQ (11/6/2014): Use a different mechanism to handle these for the GNU 4.7 compiler and above (required form C++11) */
// #if (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4))
#if (defined(__clang__) || (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)))

#if 0
/* DQ (11/10/2014): This was added to support the use of math.h header files for C++11, 
   but might not be required now. It is a problem when not using the C++11 mode.
 */
constexpr bool __builtin_isgreater(double x, double y);
constexpr bool __builtin_isgreaterequal(double x, double y);
constexpr bool __builtin_isless(double x, double y);
#else
ROSE_CONSTEXPR int __builtin_isgreater(double x, double y);
ROSE_CONSTEXPR int __builtin_isgreaterequal(double x, double y);
ROSE_CONSTEXPR int __builtin_isless(double x, double y);
#endif

int __builtin_ia32_bsrsi(int x);
unsigned long long __builtin_ia32_rdpmc(int x);
unsigned long long __builtin_ia32_rdtsc(void);
unsigned long long __builtin_ia32_rdtscp(unsigned int *__A);
unsigned char      __builtin_ia32_rolqi(unsigned char __X, int __C);
unsigned short     __builtin_ia32_rolhi(unsigned short __X, int __C);
unsigned char      __builtin_ia32_rorqi(unsigned char __X, int __C);
unsigned short     __builtin_ia32_rorhi(unsigned short __X, int __C);
void               __builtin_ia32_pause();
int                __builtin_ia32_bsrdi(long long __X);

// DQ (12/14/2016): Comment this out.
// long long          __builtin_bswap64(long long __X);

void               __builtin_ia32_fxsave(void *__P);
void               __builtin_ia32_fxrstor(void *__P);
void               __builtin_ia32_fxsave64(void *__P);
void               __builtin_ia32_fxrstor64(void *__P);
unsigned char      __builtin_ia32_addcarryx_u32(unsigned char __CF, unsigned int __X, unsigned int __Y, unsigned int *__P);
unsigned char      __builtin_ia32_addcarryx_u64(unsigned char __CF, unsigned long __X, unsigned long __Y, unsigned long long *__P);

// DQ (3/10/2017): Yet more adx builtins required.
unsigned char      __builtin_ia32_addcarry_u32(unsigned char __CF, unsigned int __X, unsigned int __Y, unsigned int *__P);
unsigned char      __builtin_ia32_addcarry_u64(unsigned char __CF, unsigned long long __X, unsigned long long __Y, unsigned long long *__P);

// DQ (3/10/2017): Yet more adx builtins required.
unsigned char __builtin_ia32_subborrow_u32(unsigned char __cf, unsigned int __x, unsigned int __y, unsigned int *__p);
unsigned char __builtin_ia32_subborrow_u64(unsigned char __cf, unsigned long long __x, unsigned long long __y, unsigned long long *__p);

/* DQ (11/17/2016): Added support for more builtin functions (not in EDG, required by 6.1 of GNU. */
unsigned int __builtin_ia32_crc32qi (unsigned int__C,unsigned char __V);
unsigned int __builtin_ia32_crc32hi (unsigned int __C, unsigned short __V);
unsigned int __builtin_ia32_crc32si (unsigned int __C, unsigned int __V);
unsigned long long __builtin_ia32_crc32di (unsigned long long __C, unsigned long long __V);
unsigned long long __builtin_ia32_readeflags_u64 (void);
void __builtin_ia32_writeeflags_u64 (unsigned long long X);

// DQ (12/17/2016): Comment this out since they are defined in Intel header 
// files and are redundant when processing files using CPP.
#if 0

/* emmintrin.h */

/* SSE3 builtin functions required for GNU 6.1 compiler */
/* SSE2 */
typedef double __v2df ROSE_GNUATTR((__vector_size__ (16)));
typedef long long __v2di ROSE_GNUATTR((__vector_size__ (16)));
typedef int __v4si ROSE_GNUATTR((__vector_size__ (16)));
typedef short __v8hi ROSE_GNUATTR((__vector_size__ (16)));
typedef char __v16qi ROSE_GNUATTR((__vector_size__ (16)));

/* Internal data types for implementing the intrinsics.  */
typedef int __v2si ROSE_GNUATTR((__vector_size__ (8)));
typedef char __v8qi ROSE_GNUATTR((__vector_size__ (8)));

/* The Intel API is flexible enough that we must allow aliasing with other
   vector types, and their scalar components.  */
typedef float __m128 ROSE_GNUATTR((__vector_size__ (32), __may_alias__));
typedef long long __m128i ROSE_GNUATTR((__vector_size__ (16), __may_alias__));
typedef double __m128d ROSE_GNUATTR((__vector_size__ (16), __may_alias__));

/* Not clear what these should be  */
// DQ (3/12/2017): Added refinements to support Clang.
#if defined(__clang__)
  #define __m64 long long
#else
// typedef int __m64 ROSE_GNUATTR((__vector_size__ (8), __may_alias__));
  typedef long long __m64 ROSE_GNUATTR((__vector_size__(8)));
#endif
typedef short __v4hi ROSE_GNUATTR((__vector_size__ (8)));
// typedef long long __m64 ROSE_GNUATTR((__vector_size__ (16)));
// typedef int __v4hi ROSE_GNUATTR((__vector_size__ (16)));
typedef float __v2sf ROSE_GNUATTR((__vector_size__ (8)));
typedef float __v4sf ROSE_GNUATTR((__vector_size__ (16)));

/* pmmintrin.h */
__m128 __builtin_ia32_addsubps (__m128 __X, __m128 __Y);
__m128 __builtin_ia32_haddps (__m128 __X, __m128 __Y);
__m128 __builtin_ia32_hsubps (__m128 __X, __m128 __Y);
__m128 __builtin_ia32_movshdup (__m128 __X);
__m128 __builtin_ia32_movsldup (__m128 __X);
__m128d __builtin_ia32_addsubpd (__m128d __X, __m128d __Y);
__m128d __builtin_ia32_haddpd (__m128d __X, __m128d __Y);
__m128d __builtin_ia32_hsubpd (__m128d __X, __m128d __Y);

/* __m128i __builtin_ia32_lddqu (__m128i const *__P); */
__m128i __builtin_ia32_lddqu (char const *__P);

/* tmmintrin.h */
__m128i __builtin_ia32_phaddw128 (__v8hi __X, __v8hi __Y);
__m128i __builtin_ia32_phaddd128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_phaddsw128 (__v8hi __X, __v8hi __Y);
__m64   __builtin_ia32_phaddw (__v4hi __X, __v4hi __Y);

__m64 __builtin_ia32_phaddd (__v2si __X, __v2si __Y);
__m64 __builtin_ia32_phaddsw (__v4hi __X, __v4hi __Y);
__m128i __builtin_ia32_phsubw128 (__v8hi __X, __v8hi __Y);
__m128i __builtin_ia32_phsubd128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_phsubsw128 (__v8hi __X, __v8hi __Y);
__m64 __builtin_ia32_phsubw (__v4hi __X, __v4hi __Y);
__m64 __builtin_ia32_phsubd (__v2si __X, __v2si __Y);
__m64 __builtin_ia32_phsubsw (__v4hi __X, __v4hi __Y);
__m128i __builtin_ia32_pmaddubsw128 (__v16qi __X, __v16qi __Y);
__m64 __builtin_ia32_pmaddubsw (__v8qi __X, __v8qi __Y);
__m128i __builtin_ia32_pmulhrsw128 (__v8hi __X, __v8hi __Y);
__m64 __builtin_ia32_pmulhrsw (__v4hi __X, __v4hi __Y);
__m128i __builtin_ia32_pshufb128 (__v16qi __X, __v16qi __Y);
__m64 __builtin_ia32_pshufb (__v8qi __X, __v8qi __Y);
__m128i __builtin_ia32_psignb128 (__v16qi __X, __v16qi __Y);
__m128i __builtin_ia32_psignw128 (__v8hi __X, __v8hi __Y);
__m128i __builtin_ia32_psignd128 (__v4si __X, __v4si __Y);
__m64 __builtin_ia32_psignb (__v8qi __X, __v8qi __Y);
__m64 __builtin_ia32_psignw (__v4hi __X, __v4hi __Y);
__m64 __builtin_ia32_psignd (__v2si __X, __v2si __Y);

__m128i __builtin_ia32_pabsb128 (__v16qi __X);
__m128i __builtin_ia32_pabsw128 (__v8hi __X);
__m128i __builtin_ia32_pabsd128 (__v4si __X);
__m64 __builtin_ia32_pabsb (__v8qi __X);
__m64 __builtin_ia32_pabsw (__v4hi __X);
__m64 __builtin_ia32_pabsd (__v2si __X);

void __builtin_ia32_movntsd (double *__P, __v2df __Y);
void __builtin_ia32_movntss (float *__P, __v4sf __Y);
__m128i __builtin_ia32_extrq (__v2di __X, __v16qi __Y);
__m128i __builtin_ia32_insertq (__v2di __X, __v2di __Y);

/* smmintrin.h */
int __builtin_ia32_ptestz128 (__v2di __M, __v2di __V);
int __builtin_ia32_ptestc128 (__v2di __M, __v2di __V);
int __builtin_ia32_ptestnzc128 (__v2di __M, __v2di __V);
__m128i __builtin_ia32_pblendvb128 (__v16qi __X, __v16qi __Y, __v16qi __M);
__m128 __builtin_ia32_blendvps (__v4sf __X, __v4sf __Y, __v4sf __M);
__m128d __builtin_ia32_blendvpd (__v2df __X, __v2df __Y, __v2df __M);
__m128i __builtin_ia32_pcmpeqq (__v2di __X, __v2di __Y);
__m128i __builtin_ia32_pminsb128 (__v16qi __X, __v16qi __Y);
__m128i __builtin_ia32_pmaxsb128 (__v16qi __X, __v16qi __Y);
__m128i __builtin_ia32_pminuw128 (__v8hi __X, __v8hi __Y);
__m128i __builtin_ia32_pmaxuw128 (__v8hi __X, __v8hi __Y);
__m128i __builtin_ia32_pminsd128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_pmaxsd128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_pminud128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_pmaxud128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_pmulld128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_pmuldq128 (__v4si __X, __v4si __Y);

__m128i __builtin_ia32_phminposuw128 (__v8hi __X);
__m128i __builtin_ia32_pmovsxbd128 (__v16qi __X);
__m128i __builtin_ia32_pmovsxwd128 (__v8hi __X);
__m128i __builtin_ia32_pmovsxbq128 (__v16qi __X);
__m128i __builtin_ia32_pmovsxdq128 (__v4si __X);
__m128i __builtin_ia32_pmovsxwq128 (__v8hi __X);
__m128i __builtin_ia32_pmovsxbw128 (__v16qi __X);
__m128i __builtin_ia32_pmovzxbd128 (__v16qi __X);
__m128i __builtin_ia32_pmovzxwd128 (__v8hi __X);


__m128i __builtin_ia32_pmovzxbq128 (__v16qi __X);
__m128i __builtin_ia32_pmovzxdq128 (__v4si __X);
__m128i __builtin_ia32_pmovzxwq128 (__v8hi __X);
__m128i __builtin_ia32_pmovzxbw128 (__v16qi __X);
__m128i __builtin_ia32_packusdw128 (__v4si __X, __v4si __Y);
__m128i __builtin_ia32_movntdqa (__v2di* __X);

/* wmmintrin.h */
__m128i __builtin_ia32_aesdec128 (__v2di __X, __v2di __Y);
__m128i __builtin_ia32_aesdeclast128 (__v2di __X, __v2di __Y);
__m128i __builtin_ia32_aesenc128 (__v2di __X, __v2di __Y);
__m128i __builtin_ia32_aesenclast128 (__v2di __X, __v2di __Y);
__m128i __builtin_ia32_aesimc128 (__v2di __X);

/* avxintrin.h */
/* Internal data types for implementing the intrinsics.  */
typedef double __v4df ROSE_GNUATTR((__vector_size__ (32)));
typedef float __v8sf ROSE_GNUATTR((__vector_size__ (32)));
typedef long long __v4di ROSE_GNUATTR((__vector_size__ (32)));
typedef unsigned long long __v4du ROSE_GNUATTR((__vector_size__ (32)));
typedef int __v8si ROSE_GNUATTR((__vector_size__ (32)));
typedef unsigned int __v8su ROSE_GNUATTR((__vector_size__ (32)));
typedef short __v16hi ROSE_GNUATTR((__vector_size__ (32)));
typedef unsigned short __v16hu ROSE_GNUATTR((__vector_size__ (32)));
typedef char __v32qi ROSE_GNUATTR((__vector_size__ (32)));
typedef unsigned char __v32qu ROSE_GNUATTR((__vector_size__ (32)));

/* The Intel API is flexible enough that we must allow aliasing with other
   vector types, and their scalar components.  */
typedef float __m256 ROSE_GNUATTR((__vector_size__ (32), __may_alias__));
typedef long long __m256i ROSE_GNUATTR((__vector_size__ (32), __may_alias__));
typedef double __m256d ROSE_GNUATTR((__vector_size__ (32), __may_alias__));

__m256d __builtin_ia32_addsubpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_addsubps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_andpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_andps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_andnpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_andnps256 (__v8sf __A, __v8sf __B);

__m256d __builtin_ia32_blendvpd256 (__v4df __X, __v4df __Y, __v4df __M);
__m256 __builtin_ia32_blendvps256 (__v8sf __X, __v8sf __Y, __v8sf __M);
__m256d __builtin_ia32_haddpd256 (__v4df __X, __v4df __Y);
__m256 __builtin_ia32_haddps256 (__v8sf __X, __v8sf __Y);
__m256d __builtin_ia32_hsubpd256 (__v4df __X, __v4df __Y);
__m256 __builtin_ia32_hsubps256 (__v8sf __X, __v8sf __Y);
__m256d __builtin_ia32_maxpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_maxps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_minpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_minps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_orpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_orps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_xorpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_xorps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_cvtdq2pd256 (__v4si __A);
__m256 __builtin_ia32_cvtdq2ps256 (__v8si __A);
__m128 __builtin_ia32_cvtpd2ps256 (__v4df __A);
__m256i __builtin_ia32_cvtps2dq256 (__v8sf __A);
__m256d __builtin_ia32_cvtps2pd256 (__v4sf __A);
__m128i __builtin_ia32_cvttpd2dq256 (__v4df __A);
__m128i __builtin_ia32_cvtpd2dq256 (__v4df __A);
__m256i __builtin_ia32_cvttps2dq256 (__v8sf __A);

void __builtin_ia32_vzeroall();
void __builtin_ia32_vzeroupper();

__m128d __builtin_ia32_vpermilvarpd (__v2df __A, __v2di __C);
__m256d __builtin_ia32_vpermilvarpd256 (__v4df __A, __v4di __C);
__m128 __builtin_ia32_vpermilvarps (__v4sf __A, __v4si __C);
__m256 __builtin_ia32_vpermilvarps256 (__v8sf __A, __v8si __C);
__m128 __builtin_ia32_vbroadcastss (float const * __X);
__m256d __builtin_ia32_vbroadcastsd256 (double const *__X);
__m256 __builtin_ia32_vbroadcastss256 (float const *__X);
__m256d __builtin_ia32_vbroadcastf128_pd256 (__m128d const *__X);
__m256 __builtin_ia32_vbroadcastf128_ps256 (__m128 const *__X);

__m256d __builtin_ia32_loadupd256 (double const *__P);
void __builtin_ia32_storeupd256 (double *__P, __v4df __A);
__m256 __builtin_ia32_loadups256 (float const *__P);
void __builtin_ia32_storeups256 (float *__P, __v8sf __A);
__m256i __builtin_ia32_loaddqu256 (char const * __P);
void __builtin_ia32_storedqu256 (char *__P, __v32qi __A);
__m128d __builtin_ia32_maskloadpd (const __v2df *__P, __v2di __M);
void __builtin_ia32_maskstorepd (__v2df *__P, __v2di __M, __v2df __A);
__m256d __builtin_ia32_maskloadpd256 (const __v4df *__P, __v4di __M);
void __builtin_ia32_maskstorepd256 (__v4df *__P, __v4di __M, __v4df __A);
__m128 __builtin_ia32_maskloadps (const __v4sf *__P, __v4si __M);
void __builtin_ia32_maskstoreps (__v4sf * __P, __v4si __M, __v4sf __A);
__m256 __builtin_ia32_maskloadps256 (const __v8sf *__P, __v8si __M);
void __builtin_ia32_maskstoreps256 (__v8sf *__P, __v8si __M, __v8sf __A);
__m256 __builtin_ia32_movshdup256 (__v8sf __X);
__m256 __builtin_ia32_movsldup256 (__v8sf __X);
__m256d __builtin_ia32_movddup256 (__v4df __X);
__m256i __builtin_ia32_lddqu256 (char const *__P);
void __builtin_ia32_movntdq256 (__v4di * __A, __v4di __B);
void __builtin_ia32_movntpd256 (double *__A, __v4df __B);
void __builtin_ia32_movntps256 (float *__P, __v8sf __A);
__m256 __builtin_ia32_rcpps256 (__v8sf __A);
__m256 __builtin_ia32_rsqrtps256 (__v8sf __A);
__m256d __builtin_ia32_sqrtpd256 (__v4df __A);
__m256 __builtin_ia32_sqrtps256 (__v8sf __A);

// #define _mm256_ceil_pd(V)	_mm256_round_pd ((V), _MM_FROUND_CEIL)
// #define _mm256_floor_pd(V)	_mm256_round_pd ((V), _MM_FROUND_FLOOR)
// #define _mm256_ceil_ps(V)	_mm256_round_ps ((V), _MM_FROUND_CEIL)
// #define _mm256_floor_ps(V)	_mm256_round_ps ((V), _MM_FROUND_FLOOR)

__m256d __builtin_ia32_unpckhpd256 (__v4df __A, __v4df __B);
__m256d __builtin_ia32_unpcklpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_unpckhps256 (__v8sf __A, __v8sf __B);
__m256 _builtin_ia32_unpcklps256 (__v8sf __A, __v8sf __B);
int __builtin_ia32_vtestzpd (__v2df __M, __v2df __V);
int __builtin_ia32_vtestcpd (__v2df __M, __v2df __V);
int __builtin_ia32_vtestnzcpd (__v2df __M, __v2df __V);
int __builtin_ia32_vtestzps (__v4sf __M, __v4sf __V);
int __builtin_ia32_vtestcps (__v4sf __M, __v4sf __V);
int __builtin_ia32_vtestnzcps (__v4sf __M, __v4sf __V);
int __builtin_ia32_vtestzpd256 (__v4df __M, __v4df __V);
int __builtin_ia32_vtestcpd256 (__v4df __M, __v4df __V);
int __builtin_ia32_vtestnzcpd256 (__v4df __M, __v4df __V);
int __builtin_ia32_vtestzps256 (__v8sf __M, __v8sf __V);
int __builtin_ia32_vtestcps256 (__v8sf __M, __v8sf __V);
int __builtin_ia32_vtestnzcps256 (__v8sf __M, __v8sf __V);
int __builtin_ia32_ptestz256 (__v4di __M, __v4di __V);
int __builtin_ia32_ptestc256 (__v4di __M, __v4di __V);
int __builtin_ia32_ptestnzc256 (__v4di __M, __v4di __V);
int __builtin_ia32_movmskpd256 (__v4df __A);
int __builtin_ia32_movmskps256 (__v8sf __A);

__m128d __builtin_ia32_pd_pd256 (__v4df __A);
__m128 __builtin_ia32_ps_ps256 (__v8sf __A);
__m128i __builtin_ia32_si_si256 (__v8si __A);
__m256d __builtin_ia32_pd256_pd (__v2df __A);
__m256 __builtin_ia32_ps256_ps (__v4sf __A);
__m256i __builtin_ia32_si256_si (__v4si __A);

__m256 __builtin_ia32_unpcklps256 (__v8sf __A, __v8sf __B);

void __builtin_ia32_monitor (void const * __P, unsigned int __E, unsigned int __H);
void __builtin_ia32_mwait (unsigned int __E, unsigned int __H);


/* New builtins (11/19/2016) */

__m256d __builtin_ia32_addpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_addps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_divpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_divps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_mulpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_mulps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_subpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_subps256 (__v8sf __A, __v8sf __B);


/* Test how many files of builtins we have to add */
   #define _AVX2INTRIN_H_INCLUDED
   #define _AVX512FINTRIN_H_INCLUDED
   #define _AVX512CDINTRIN_H_INCLUDED
   #define _AVX512VLINTRIN_H_INCLUDED
   #define _AVX512BWINTRIN_H_INCLUDED
   #define _AVX512DQINTRIN_H_INCLUDED
   #define _AVX512VLBWINTRIN_H_INCLUDED
   #define _AVX512VLDQINTRIN_H_INCLUDED
   #define _AVX512IFMAINTRIN_H_INCLUDED
   #define _AVX512IFMAVLINTRIN_H_INCLUDED
   #define _AVX512VBMIINTRIN_H_INCLUDED
   #define _AVX512VBMIVLINTRIN_H_INCLUDED
   #define _SHAINTRIN_H_INCLUDED
   #define _LZCNTINTRIN_H_INCLUDED
   #define _BMIINTRIN_H_INCLUDED
   #define _BMI2INTRIN_H_INCLUDED
   #define _FMAINTRIN_H_INCLUDED
   #define _F16CINTRIN_H_INCLUDED
   #define _RTMINTRIN_H_INCLUDED
   #define _XTESTINTRIN_H_INCLUDED

/* DQ (12/14/2016): Add to the list. */
   #define _MMINTRIN_H_INCLUDED
   #define _XMMINTRIN_H_INCLUDED
   #define _EMMINTRIN_H_INCLUDED
// #define _EMMINTRIN_H_INCLUDED

/* DQ (12/14/2016): This is the header file that includes all of the other header files (above). */
   #define _X86INTRIN_H_INCLUDED


/* This just includes a lot of other include files specific to AVX (partially addressed above) */
/* #define _IMMINTRIN_H_INCLUDED */

/* immintrin.h */
int __builtin_ia32_rdrand16_step (unsigned short *__P);
int __builtin_ia32_rdrand32_step (unsigned int *__P);
unsigned int __builtin_ia32_rdfsbase32();
unsigned long long __builtin_ia32_rdfsbase64();
unsigned int __builtin_ia32_rdgsbase32();
unsigned long long __builtin_ia32_rdgsbase64();
void __builtin_ia32_wrfsbase32 (unsigned int __B);
void __builtin_ia32_wrfsbase64 (unsigned long long __B);
void __builtin_ia32_wrgsbase32 (unsigned int __B);
void __builtin_ia32_wrgsbase64 (unsigned long long __B);
int __builtin_ia32_rdrand64_step (unsigned long long *__P);

/* emmintrin.h */

__m128i _mm_set1_epi64x (long long __A);
__m128d _mm_set1_pd (double __F);
__m128i _mm_set_epi64x (long long __q1, long long __q0);
__m128i _mm_and_si128 (__m128i __A, __m128i __B);
__m128i _mm_or_si128 (__m128i __A, __m128i __B);
__m128d _mm_sub_pd (__m128d __A, __m128d __B);
__m128d _mm_mul_pd (__m128d __A, __m128d __B);
double _mm_cvtsd_f64 (__m128d __A);
__m128d _mm_hadd_pd (__m128d __X, __m128d __Y);
__m128d _mm_add_pd (__m128d __A, __m128d __B);
__m128 _mm_hadd_ps (__m128 a, __m128 b);
void _mm_storeu_pd (double *__P, __m128d __A);

__m128i _mm_cvtepi8_epi32(__m128i a);
__m128i _mm_cvtepi8_epi64 (__m128i a);
__m128i _mm_cvtepi8_epi16(__m128i a);
__m128i _mm_cvtepi32_epi64(__m128i a);
__m128i _mm_cvtepi16_epi32(__m128i a);
__m128i _mm_cvtepi16_epi64(__m128i a);
__m128i _mm_cvtepu8_epi32(__m128i a);
__m128i _mm_cvtepu8_epi64(__m128i a);
__m128i _mm_cvtepu8_epi16(__m128i a);
__m128i _mm_cvtepu32_epi64(__m128i a);
__m128i _mm_cvtepu16_epi32(__m128i a);
__m128i _mm_cvtepu16_epi64(__m128i a);
__m128  _mm_hadd_ps(__m128 a, __m128 b);
__m128i _mm_cmpeq_epi64 (__m128i a, __m128i b);
__m128  _mm_blendv_ps(__m128 v1, __m128 v2, __m128 v3);
__m128d _mm_blendv_pd(__m128d v1, __m128d v2, __m128d v3);
__m128i _mm_blendv_epi8 (__m128i a, __m128i b, __m128i mask);
__m128i _mm_blend_epi16 (__m128i a, __m128i b, const int imm8);
__m128i _mm_max_epi32 (__m128i a, __m128i b);
__m128i _mm_max_epi8 (__m128i a, __m128i b);
__m128i _mm_max_epu16 (__m128i a, __m128i b);
__m128i _mm_max_epu32 (__m128i a, __m128i b);
__m128i _mm_min_epi32 (__m128i a, __m128i b);
__m128i _mm_min_epi8 (__m128i a, __m128i b);
__m128i _mm_min_epu16 (__m128i a, __m128i b);
__m128i _mm_min_epu32 (__m128i a, __m128i b);
__m128i _mm_mul_epi32 (__m128i a, __m128i b);
__m128i _mm_mullo_epi32 (__m128i a, __m128i b);
__m128i _mm_packus_epi32 (__m128i a, __m128i b);
__m128i _mm_shuffle_epi8 (__m128i a, __m128i b);


   #define _MM3DNOW_H_INCLUDED
   #define _FMA4INTRIN_H_INCLUDED
   #define _XOPMMINTRIN_H_INCLUDED
   #define _LWPINTRIN_H_INCLUDED
   #define _RDSEEDINTRIN_H_INCLUDED
   #define _XSAVEINTRIN_H_INCLUDED
   #define _XSAVEOPTINTRIN_H_INCLUDED
   #define _ADXINTRIN_H_INCLUDED
   #define _CLWBINTRIN_H_INCLUDED
   #define _PCOMMITINTRIN_H_INCLUDED
   #define _CLFLUSHOPTINTRIN_H_INCLUDED
   #define _XSAVESINTRIN_H_INCLUDED
   #define _XSAVECINTRIN_H_INCLUDED
   #define _MWAITXINTRIN_H_INCLUDED
   #define _CLZEROINTRIN_H_INCLUDED
   #define _PKUINTRIN_H_INCLUDED


/* DQ (11/17/2016): Added support for more builtin functions (not in EDG, required by 6.1 of GNU. */
#if (__GNUC__ >= 6)
   #define __SSE3__
   #define __SSE2__
   #define __SSE4A__
   #define __SSE4_1__
#endif

// DQ (12/17/2016): Comment this out since they are defined in Intel header 
// files and are redundant when processing files using CPP.
#else

#if 0
// DQ (7/31/2018): We don't want to include these since they will cause the ROSE generated xmmintrin.h file to be ignored (see Cxx11_tests/test2018_135.C).

   #define _X86INTRIN_H_INCLUDED

// DQ (1/8/2017): I think this is required for GNU 6.1 compiler (see Cxx_tests/test2011_133.C).
   #define _MMINTRIN_H_INCLUDED
   #define _XMMINTRIN_H_INCLUDED
   #define _EMMINTRIN_H_INCLUDED
#endif

// DQ (12/17/2016): Added what are likely Intel (v16) specific types.  These may have 
// to be defined macros since "typedef unsigned __int64 m64_u64;" is a problem.
// typedef __INT16_TYPE__ __int16;
// typedef __INT32_TYPE__ __int32;
// typedef __INT64_TYPE__ __int64;
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long

// DQ (12/17/2016): This is sufficent now that we have modified EDG to eliminate the error on underflow for floating point values.
#define __INTEL_CLANG_COMPILER

// #define _mm_set1_epi64x(X) ((__m128i)(0LL))
// #define _mm_set1_epi64x(X)
// typedef double  __m128d ROSE_GNUATTR((__vector_size__(16)));
// typedef __int64 __m128i ROSE_GNUATTR((__vector_size__(16)));
// __m128i _mm_set1_epi64x (long long __A);
// extern __m128i  _mm_set1_epi64x(__int64);

// DQ (12/17/2016): Intel sets this by default to a value too large for EDG to accept (so we just reset it).
// This is only used in the limits header file and as the return value for a function that hides this macro.
// For use in ROSE this is a simple fix that does not effect the users code.  As a result is we generate
// a file via CPP to process by ROSE we MUST use ROSE to generate the CPP file (so that these macros and
// the __INTEL_CLANG_COMPILER macro will be seen by CPP and code generated that can be read by ROSE.
// Else the code generated will be such that only the Intel compiler can read it (Intel specific types
// and builtin function that return types that are only defined in the Intel header files).

// These values can not be processed as litrals by EDG.
// #define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// #define __FLT_DENORM_MIN__ 1.40129846e-45F
// Note that the ROSE configuration fixes the header files to be:
// #define __DBL_DENORM_MIN__ 4.9406564584124654e-324L
// #define __FLT_DENORM_MIN__ 1.40129846e-45L
// But this is not enough to permit the processing of the header 
// files where they are expanded to a single file via CPP. So
// we need to define them to be smaller values.
// #undef __FLT_DENORM_MIN__ 
// #undef __DBL_DENORM_MIN__ 
// #define __FLT_DENORM_MIN__ 42.0
// #define __DBL_DENORM_MIN__ 42.0
// Note that this should also likely effect:
// #define __LDBL_DENORM_MIN__ 3.64519953188247460253e-4951L
// But this value has not been a problem for EDG (so far).

#endif

#else

/* DQ (2/21/2017): This is a problem for the Clang compiler. */
#if 0

/* DQ (5/4/2010): Reported problem under Ubuntu 9.10 with GCC 4.4.1 and Boost 1.42.
*/
int __builtin_fpclassify (int, int, int, int, int, ...);

/* DQ (11/14/2016): For GNU 6.1 (at least) the isunordered() builtin is not available. */
#if ((__GNUC__ < 6) && (__GNUC_MINOR__ >= 0))

/* GCC provides built-in versions of the ISO C99 floating point comparison macros that
    avoid raising exceptions for unordered operands. They have the same names as the
    standard macros ( isgreater, isgreaterequal, isless, islessequal, islessgreater, and
    isunordered) , with __builtin_ prefixed. We intend for a library implementor to be
    able to simply #define each standard macro to its built-in equivalent.

DQ (6/19/2007): These might required math.h to be included.
*/
# ifndef __builtin_isgreater
#  define __builtin_isgreater(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x > __y; }))
# endif

/* Return nonzero value if X is greater than or equal to Y.  */
# ifndef __builtin_isgreaterequal
#  define __builtin_isgreaterequal(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x >= __y; }))
# endif

/* Return nonzero value if X is less than Y.  */
# ifndef __builtin_isless
#  define __builtin_isless(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x < __y; }))
# endif

/* Return nonzero value if X is less than or equal to Y.  */
# ifndef __builtin_islessequal
#  define __builtin_islessequal(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x <= __y; }))
# endif

/* Return nonzero value if either X is less than Y or Y is less than X.  */
# ifndef __builtin_islessgreater
#  define __builtin_islessgreater(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && (__x < __y || __y < __x); }))
# endif

/* Return nonzero value if arguments are unordered.  */
# ifndef __builtin_isunordered
#  define __builtin_isunordered(u, v) \
  (__extension__							      \
   ({ __typeof__(u) __u = (u); __typeof__(v) __v = (v);			      \
      fpclassify (__u) == FP_NAN || fpclassify (__v) == FP_NAN; }))
# endif

/* DQ (2/21/2017): This is a problem for the Clang compiler. */
#endif

/* DQ (11/14/2016): For GNU 6.1 (at least) the isunordered() builtin is not available. */
#endif

/* #if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) */
#endif

/* DQ (9/12/2016): Comment out to use new automated generation of builtin functions. */
#endif

/* DQ (2/22/2017): Added to support GNU 4.8 in C++11 mode using EDG 4.9. */
#ifdef __cplusplus
// DQ (2/21/2017): These functions are not defined for C language support.
ROSE_CONSTEXPR int __builtin_isgreater(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isgreaterequal(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isless(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_islessequal(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_islessgreater(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_isunordered(...) ROSE_GNUATTR((nothrow, const));
ROSE_CONSTEXPR int __builtin_fpclassify(int, int, int, int, ...) ROSE_GNUATTR((nothrow, const));
int __builtin_isinf_sign(...) ROSE_GNUATTR((nothrow, const));
#endif


/* DQ (9/2/2016): Comment out to use new automated generation of builtin functions. */
#if 0

/* int __builtin_isgreater(x,y) isgreater(x,y);
int __builtin_isgreaterequal(x,y) isgreaterequal(x,y);
int __builtin_isless(x,y) isless(x,y);
int __builtin_islessequal(x,y) islessequal(x,y);
int __builtin_islessgreater(x,y) islessgreater(x,y);
int __builtin_isunordered(x,y) isunordered(x,y);
*/

/* GNU also supports a few other types of builtin functions: */
void * __builtin_return_address (unsigned int level);
void * __builtin_frame_address (unsigned int level);

/* Additional builtin functions that take or return types as arguments
(described at http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/Other-Builtins.html).
There are more difficult to reproduce except as macros that define them away:

int __builtin_types_compatible_p (type1, type2);
type __builtin_choose_expr (const_exp, exp1, exp2);
int __builtin_constant_p (exp);

DQ (6/19/2007): The definitions below are not correct, but should be portable, 
they will only be a problem is the resulting code is unparsed directly from 
the AST.  The detection of the use of these macros in the near future will help 
make this safer.
*/

/* DQ (3/18/2015): This macro should no longer be required since it is defined properly in EDG as a recognized builtin function. */
/* This is not a correct test, but it is a weak form of equivalence that is portable */
/* #define __builtin_types_compatible_p(T1,T2) (sizeof(T1)==sizeof(T2)) */
/* DQ (3/23/2015): We need to use the EDG's version of __builtin_types_compatible_p() 
    which it then translates to _types_compatible() and then we need to conversion back 
    to __builtin_types_compatible_p() for the backend compiler. */
#define __types_compatible(T1,T2) __builtin_types_compatible_p(T1,T2)

/* This is not correct, but it should be portable */
#define __builtin_choose_expr(exp,T1,T2) (T1)

/* DQ (4/9/2015): Comment this out in favor of the supported version.
   This is not correct, but it should be portable, make it always return false for now 
#define __builtin_constant_p(exp) (0) 
*/


/* Additional builtin functions:
(also from http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/Other-Builtins.html):
 */

long        __builtin_expect (long __builtin__exp, long __builtin__c);
void        __builtin_prefetch (const void *__builtin__addr, ...);
double      __builtin_huge_val (void);
float       __builtin_huge_valf (void);
long double __builtin_huge_vall (void);
double      __builtin_inf (void);
float       __builtin_inff (void);
long double __builtin_infl (void);
double      __builtin_nan (const char *__builtin__str);
float       __builtin_nanf (const char *__builtin__str);
long double __builtin_nanl (const char *__builtin__str);
double      __builtin_nans (const char *__builtin__str);
float       __builtin_nansf (const char *__builtin__str);
long double __builtin_nansl (const char *__builtin__str);
/* DQ (6/19/2007): Commented out because in interferred with existing 
                   builtin function defined as int __builtin_ffs (int __builtin__x); in EDG.
int __builtin_ffs (unsigned int __builtin__x); */
int __builtin_clz (unsigned int __builtin__x);
int __builtin_ctz (unsigned int __builtin__x);
int __builtin_popcount (unsigned int __builtin__x);
int __builtin_parity (unsigned int __builtin__x);
int __builtin_ffsl (unsigned long __builtin__x);
int __builtin_clzl (unsigned long __builtin__x);
int __builtin_ctzl (unsigned long __builtin__x);
int __builtin_popcountl (unsigned long __builtin__x);
int __builtin_parityl (unsigned long __builtin__x);
int __builtin_ffsll (unsigned long long __builtin__x);
int __builtin_clzll (unsigned long long __builtin__x);
int __builtin_ctzll (unsigned long long __builtin__x);
int __builtin_popcountll (unsigned long long __builtin__x);
int __builtin_parityll (unsigned long long __builtin__x);
double      __builtin_powi (double __builtin__x, int __builtin__y);
float       __builtin_powif (float __builtin__x, int __builtin__y);
long double __builtin_powil (long double __builtin__x, int __builtin__y);


/* DQ (6/19/2007): I think these defines can be eliminated now in favor of the 
                   builtin function prototypes.
   DQ (8/20/2006): Let the builtin values be equal to the largest possible values.
   DQ (5/20/2006): These should be defined to be appropriate values or defined as 
   function prototypes as others are below.  These can't be functions because they 
   are used to initialize static const variables.

#define __builtin_huge_valf() __FLT_MAX__
#define __builtin_nanf(string) 0
#define __builtin_huge_val() __DBL_MAX__
#define __builtin_nan(string) 0
#define __builtin_huge_vall() __LDBL_MAX__
#define __builtin_nanl(string) 0
*/

/* DQ (8/25/2009): Added another builtin function required to compile ROSE with ROSE. */
#ifdef _GLIBCXX_ATOMIC_BUILTINS
typedef int _Atomic_word;
_Atomic_word  __sync_fetch_and_add(volatile _Atomic_word* __mem, int __val);
#endif

/* DQ and Liao (7/11/2009) Added macros to define away new GNU C++ extension (not required for newer EDG 4.0 use 
// Only for GCC 4.3.0 and above
*/
#if 0
/* DQ (7/3/2013): Implemented mechanism to lie to Boost about what version of GNU we are emulating.
   This is required to fix a bug in EDG 4.7 which is being reported to EDG.
Original code:
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 3 || (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ >= 0)))
*/
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 3 || (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ >= 0))) || defined(LIE_ABOUT_GNU_VERSION_TO_EDG)

#define __has_nothrow_assign sizeof
#define __has_nothrow_copy sizeof
#define __has_nothrow_constructor sizeof
#define __has_trivial_assign sizeof
#define __has_trivial_copy  sizeof
#define __has_trivial_constructor sizeof
#define __has_trivial_destructor sizeof
#define __has_virtual_destructor sizeof
#define __is_abstract sizeof
//#define __is_base_of (base_type, derived_type)
#define __is_class sizeof
#define __is_empty sizeof
#define __is_enum sizeof
#define __is_pod sizeof 
#define __is_polymorphic sizeof
#define __is_union sizeof
// Liao 11/19/2012. EDG 4.4 complains about long long for this prototype
//void* __builtin_memmove(void * target, const void * source, unsigned long long nBytes);
void* __builtin_memmove(void * target, const void * source, unsigned long nBytes);
void* __builtin_memchr(const  void * ptr, int value, unsigned long long num);

/* DQ (9/30/2009): This needs to be unsigned long long for 64-bit and 
   unsigned int for 32-bit (at least on GNU g++ versions 4.3 and greater).
   I think it is better to use size_t than __SIZE_TYPE__. 
   Note that this is a fix for a problem on 32-bit gnu 4.3 systems (nmi:x86_rhas_4).
// void* __builtin_memcpy (void * destination, const void * source, unsigned long long num );
// int __builtin_memcmp ( const void * ptr1, const void * ptr2, unsigned long long num );
// void* __builtin_memcpy (void * destination, const void * source, __SIZE_TYPE__ num );
// int __builtin_memcmp ( const void * ptr1, const void * ptr2, __SIZE_TYPE__ num );
void* __builtin_memcpy (void * destination, const void * source, __SIZE_TYPE__ num );
int __builtin_memcmp ( const void * ptr1, const void * ptr2, __SIZE_TYPE__ num );
*/

// changed it in edg 3.3/src/sys_predef.c instead since va_list is not declared here
//int __builtin_vsnprintf(char *str, unsigned long long size, const char *format, va_list ap);

#endif
#endif
/*
Target specific builtin functions are available at:
  http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/Target-Builtins.html
    * Alpha Built-in Functions
    * ARM Built-in Functions
    * Blackfin Built-in Functions
    * FR-V Built-in Functions
    * X86 Built-in Functions
    * MIPS Paired-Single Support
    * PowerPC AltiVec Built-in Functions
    * SPARC VIS Built-in Functions 
*/

/*
Required builtin function as supported by Intel (gnu builtin functions supported by Intel
C++ compiler).  I am not sure if we should include anything special specific to this, it
appears to be a subset of the more complete handling above.

__builtin_abs
__builtin_labs
__builtin_cos
__builtin_cosf
__builtin_fabs
__builtin_fabsf
__builtin_memcmp
__builtin_memcpy
__builtin_sin
__builtin_sinf
__builtin_sqrt
__builtin_sqrtf
__builtin_strcmp
__builtin_strlen
__builtin_strncmp
__builtin_abort
__builtin_prefetch
__builtin_constant_p
__builtin_printf
__builtin_fprintf
__builtin_fscanf
__builtin_scanf
__builtin_fputs
__builtin_memset
__builtin_strcat
__builtin_strcpy
__builtin_strncpy
__builtin_exit
__builtin_strchr
__builtin_strspn
__builtin_strcspn
__builtin_strstr
__builtin_strpbrk
__builtin_strrchr
__builtin_strncat
__builtin_alloca
__builtin_ffs
__builtin_index
__builtin_rindex
__builtin_bcmp
__builtin_bzero
__builtin_sinl
__builtin_cosl
__builtin_sqrtl
__builtin_fabsl
__builtin_frame_address (IA-32 only)
__builtin_return_address (IA-32 only)
*/

/* EDG 4.8 when used with GNU 4.8.1 appears to require some additional builtin functions */
int __builtin_abs (int __builtin__x);



/* DQ (6/19/2007): For handling of offsetof macro we can't build a function 
prototype so EDG provides a mechanism to support this (from the basics.h 
header file). However, we can use the one defined in the GNU header files is 
we only handle the __offsetof__ macro (so define it way).  Then the builtin
function is just defined as being the offsetof macro.
*/
#define __offsetof__
/*
// DQ (2/9/2008): Don't define __builtin_offsetof in terms of offsetof since this
// causes a recursively defined marco on Fedora Core relase 4, though it works 
// fine on Red Hat Enterprise release 9. New definition taken from common
// implementation of offset, but modified to address specific case of where C++
// defines a operator&() which can be a problem for the more common definition.
// This definition is used to defin the __builtin_offsetof
// #define __builtin_offsetof(t,memb) offsetof(t,memb)
// #define __builtin_offsetof(t,memb) ((size_t)&(((t *)0)->memb))
*/
#ifndef __cplusplus
/* DQ (3/24/2015): Added this back because it is required by Xen. */
/* DQ (3/19/2015): This is now properly defined by EDG so we don't need to define it explicitly (noticed as part of Xen support). */
/* #define __builtin_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER) */
#define __builtin_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#else
/* The cast to "char &" below avoids problems with user-defined "operator &", which can appear in a POD type. */
#define __builtin_offsetof(TYPE, MEMBER)					\
  (__offsetof__ (reinterpret_cast <size_t>			\
                 (&reinterpret_cast <const volatile char &>	\
                  (static_cast<TYPE *> (0)->MEMBER))))
#endif

/* DQ (10/30/2005): Added to allow compilation of g++ 
complex header, but this is likely the wrong thing to do.
This allows us to compile C++ code that uses the complex header file
but it turns "__complex__ double x" into just "double x". All this
is because the "__complex__" keyword is not supported in EDG.  Not
too much of a problem except that it means that the member function:
    complex(_ComplexT __z) : _M_value(__z) { }
needs to be specified using the "explicit" keyword.   Note that
none of this is required in g++ or icc, just in our version of 
ROSE using the g++ header files (because __complex__ is not
defined in EDG and the only thing to map it to is "", I think).

The following solution does NOT work.  Replacing
    typedef __complex__ double _ComplexT;
with
    typedef complex<double> _ComplexT;
in the complex header file.

The following solution does work.  Replacing
    complex(_ComplexT __z) : _M_value(__z) { }
with
    explicit complex(_ComplexT __z) : _M_value(__z) { }
in the complex header file.  It is not a great solution
and it would have to be done in the configuration of ROSE
to the complex header file.  It would also fool any analysis
of the complex class into thinking that the internal type 
was just float or double for complex<float> or complex<double>.
 */

/* DQ (8/22/2006):
   EDG does not appear to support __complex__ as a keyword.
   Thus we have to define it to white space to permit codes using 
   __complex__ to be compiled.  This appears to work fine, but
   we are likely to confuse an analysis that depends upon recognizing 
   complex types.

   Note that _Complex is a C99 type, also fequently recognised by C 
   compilers, so for non C++ codes we can translate __complex__ to 
   _Complex
*/
/* #ifdef __cplusplus */
/* #if ROSE_CPP_MODE */
#if (ROSE_LANGUAGE_MODE == ROSE_CXX_LANGUAGE_MODE)
/* C++ case is different because the header files will define a complex type (see hdrs1/complex) */

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ < 4))
/* DQ (5/02/2016): These need to be defined away for the case of GNU 4.2 (and I think anything less than GNU 4.4). */
/* Note that Clang will pertend to be GNU 4.2.1. */
#ifndef __clang__
  #define __complex__
  #define __real__ 
  #define __imag__ 
  #define _Complex
#endif
#endif

/* DQ (10/13/2012): Added to support complex with EDG 4.x work (defines away _Imaginary, I
   don't like this fix).  This is required when constant folding is turned off.
 */
/* #define _Imaginary __imag__ */
#else
/* This works for both C and C99 modes */
  #define __complex__ _Complex
  #define __real__
  #define __imag__

/* DQ (10/13/2012): Added to support complex with EDG 4.x work (defines away _Imaginary, I
   don't like this fix).  This is required when constant folding is turned off.
 */
  #define _Imaginary __imag__
#endif

/* gcc uses the C99 name _Complex_I in <complex.h>, but our EDG doesn't handle
   the GCC extension that they define it to. */
#ifdef __INTEL_COMPILER
/* #define _Complex_I __I__ */
#else
#define _Complex_I __I__
#endif

/* Disable inclusion of complex.h on Linux */
/* #define _COMPLEX_H */
/* Disable inclusion of complex.h on Mac OS X */
/* #define __COMPLEX__ */

/* DQ (8/25/2009): Added complex builtin functions as required to compile parts of ROSE with ROSE */
/* #if _GLIBCXX_USE_C99_COMPLEX */
/* #if _GLIBCXX_USE_C99_COMPLEX || defined(USE_ROSE) */
#if _GLIBCXX_USE_C99_COMPLEX
float  __builtin_cabsf(__complex__ float __z);
double __builtin_cabs (__complex__ double __z);
long double __builtin_cabsl(const __complex__ long double& __z);

float  __builtin_cargf(__complex__ float __z);
double __builtin_carg(__complex__ double __z);
long double __builtin_cargl(const __complex__ long double& __z);

__complex__ float  __builtin_ccosf(__complex__ float __z);
__complex__ double __builtin_ccos(__complex__ double __z);
__complex__ long double __builtin_ccosl(const __complex__ long double& __z);

__complex__ float  __builtin_ccoshf(__complex__ float __z);
__complex__ double __builtin_ccosh(__complex__ double __z);
__complex__ long double __builtin_ccoshl(const __complex__ long double& __z);

__complex__ float  __builtin_cexpf(__complex__ float __z);
__complex__ double __builtin_cexp(__complex__ double __z);
__complex__ long double __builtin_cexpl(const __complex__ long double& __z);

__complex__ float  __builtin_clogf(__complex__ float __z);
__complex__ double __builtin_clog(__complex__ double __z);
__complex__ long double __builtin_clogl(const __complex__ long double& __z);

__complex__ float  __builtin_csinf(__complex__ float __z);
__complex__ double __builtin_csin(__complex__ double __z);
__complex__ long double __builtin_csinl(const __complex__ long double& __z);

__complex__ float  __builtin_csinhf(__complex__ float __z);
__complex__ double __builtin_csinh(__complex__ double __z);
__complex__ long double __builtin_csinhl(const __complex__ long double& __z);

__complex__ float __builtin_csqrtf(__complex__ float __z);
__complex__ double __builtin_csqrt(__complex__ double __z);
__complex__ long double __builtin_csqrtl(const __complex__ long double& __z);

__complex__ float  __builtin_ctanf (__complex__ float __x);
__complex__ double __builtin_ctan(__complex__ double __z);
__complex__ long double __builtin_ctanl(const __complex__ long double& __z);

__complex__ float  __builtin_ctanhf(__complex__ float __z);
__complex__ double __builtin_ctanh(__complex__ double __z);
__complex__ long double __builtin_ctanhl(const __complex__ long double& __z);

__complex__ float  __builtin_cpowf(__complex__ float __x, __complex__ float __y);
__complex__ double __builtin_cpow(__complex__ double __x, __complex__ double __y);
__complex__ long double __builtin_cpowl(const __complex__ long double& __x,const __complex__ long double& __y);
#endif

#ifdef USE_ROSE
/* DQ (1/26/2010): Define these so that ROSE will compile with ROSE. */
#define __builtin_cabsf(x) 0
#define __builtin_cabs(x)  0
#define __builtin_cabsl(x) 0
#define __builtin_cargf(x) 0
#define __builtin_carg(x) 0
#define __builtin_cargl(x) 0
#define __builtin_ccosf(x) 0
#define __builtin_ccos(x) 0
#define __builtin_ccosl(x) 0
#define __builtin_ccoshf(x) 0
#define __builtin_ccosh(x) 0
#define __builtin_ccoshl(x) 0
#define __builtin_cexpf(x) 0
#define __builtin_cexp(x) 0
#define __builtin_cexpl(x) 0
#define __builtin_clogf(x) 0
#define __builtin_clog(x) 0
#define __builtin_clogl(x) 0
#define __builtin_csinf(x) 0
#define __builtin_csin(x) 0
#define __builtin_csinl(x) 0
#define __builtin_csinhf(x) 0
#define __builtin_csinh(x) 0
#define __builtin_csinhl(x) 0
#define __builtin_csqrtf(x) 0
#define __builtin_csqrt(x) 0
#define __builtin_csqrtl(x) 0
#define __builtin_ctanf(x) 0
#define __builtin_ctan(x) 0
#define __builtin_ctanl(x) 0
#define __builtin_ctanhf(x) 0
#define __builtin_ctanh(x) 0
#define __builtin_ctanhl(x) 0
#define __builtin_cpowf(x,y) 0
#define __builtin_cpow(x,y) 0
#define __builtin_cpowl(x,y) 0
#endif


/* Defined this to avoid warnings (e.g. test2001_11.C) from 3.4.6 systems header files. */
#define __weakref__(NAME)

/* DQ (6/19/2007): I think we can comment this out now, since it is better defined above!
   DQ (1/31/2007): GNU modifier required to handle code using the offsetof macro in C++ g++ 3.4 and greater */
/* #define __offsetof__ */

#if 0
/* DQ (7/11/2009) Added macros to define away new GNU C++ extension (not required for newer EDG 4.0 use */
#define __has_nothrow_assign (type)
#define __has_nothrow_copy (type)
#define __has_nothrow_constructor (type)
#define __has_trivial_assign (type)
#define __has_trivial_copy (type)
#define __has_trivial_constructor (type)
#define __has_trivial_destructor (type)
#define __has_virtual_destructor (type)
#define __is_abstract (type)
#define __is_base_of (base_type, derived_type)
#define __is_class (type)
#define __is_empty (type)
#define __is_enum (type)
#define __is_pod (type)
#define __is_polymorphic (type)
#define __is_union (type)
#endif

/* 
   DQ (7/15/2009): Added support for MS Windows Code 
   It might be that this file in included too late when using WINE.
   DQ (8/15/2009): The problem is that the rose_config.h file is 
   not included so USE_ROSE_WINDOWS_ANALYSIS_SUPPORT is not defined.
 */
/* #ifdef USE_ROSE_WINDOWS_ANALYSIS_SUPPORT */
 #define __builtin_ms_va_list __builtin_va_list
 #define __ms_va_list va_list
/* #endif */

/*
   DQ (9/12/2009): Avoid this GNU extension since it is a problem for EDG.
 */
#define _GLIBCXX_EXTERN_TEMPLATE 0

/*************************************************
        CUDA and OPENCL SPECIFIC SUPPORT
*************************************************/

#if (ROSE_LANGUAGE_MODE == ROSE_CUDA_LANGUAGE_MODE)

/* CUDA Built-in Types */

  /*
    Extract and adapted from:
      CUDA_PATH/include/common_functions.h
      CUDA_PATH/include/common_types.h
      CUDA_PATH/include/device_functions.h
      CUDA_PATH/include/device_types.h
      CUDA_PATH/include/sm_11_atomic_functions.h
      CUDA_PATH/include/sm_12_atomic_functions.h
      CUDA_PATH/include/sm_13_atomic_functions.h
      CUDA_PATH/include/sm_20_atomic_functions.h
      CUDA_PATH/include/sm_20_intrinsics.h
      CUDA_PATH/include/vector_types.h
      CUDA_PATH/include/vector_functions.h
  */

  /* Vector Types */

struct char1
{
  signed char x;
};

struct uchar1 
{
  unsigned char x;
};

struct __builtin_align__(2) char2
{
  signed char x, y;
};

struct __builtin_align__(2) uchar2
{
  unsigned char x, y;
};

struct char3
{
  signed char x, y, z;
};

struct uchar3
{
  unsigned char x, y, z;
};

struct __builtin_align__(4) char4
{
  signed char x, y, z, w;
};

struct __builtin_align__(4) uchar4
{
  unsigned char x, y, z, w;
};

struct short1
{
  short x;
};

struct ushort1
{
  unsigned short x;
};

struct __builtin_align__(4) short2
{
  short x, y;
};

struct __builtin_align__(4) ushort2
{
  unsigned short x, y;
};

struct short3
{
  short x, y, z;
};

struct ushort3
{
  unsigned short x, y, z;
};

struct __builtin_align__(8) short4
{
  short x, y, z, w;
};

struct __builtin_align__(8) ushort4
{
  unsigned short x, y, z, w;
};

struct int1
{
  int x;
};

struct uint1
{
  unsigned int x;
};

struct __builtin_align__(8) int2
{
  int x, y;
};

struct __builtin_align__(8) uint2
{
  unsigned int x, y;
};

struct int3
{
  int x, y, z;
};

struct uint3
{
  unsigned int x, y, z;
};

struct __builtin_align__(16) int4
{
  int x, y, z, w;
};

struct __builtin_align__(16) uint4
{
  unsigned int x, y, z, w;
};

struct long1
{
  long int x;
};

struct ulong1
{
  unsigned long x;
};

struct 
#if defined (_WIN32)
       __builtin_align__(8)
#else /* _WIN32 */
       __builtin_align__(2*sizeof(long int))
#endif /* _WIN32 */
                                             long2
{
  long int x, y;
};

struct 
#if defined (_WIN32)
       __builtin_align__(8)
#else /* _WIN32 */
       __builtin_align__(2*sizeof(unsigned long int))
#endif /* _WIN32 */
                                                      ulong2
{
  unsigned long int x, y;
};

#if !defined(__LP64__)

struct long3
{
  long int x, y, z;
};

struct ulong3
{
  unsigned long int x, y, z;
};

struct __builtin_align__(16) long4
{
  long int x, y, z, w;
};

struct __builtin_align__(16) ulong4
{
  unsigned long int x, y, z, w;
};

#endif /* !__LP64__ */

struct float1
{
  float x;
};

struct __builtin_align__(8) float2
{
  float x, y;
};

struct float3
{
  float x, y, z;
};

struct __builtin_align__(16) float4
{
  float x, y, z, w;
};

struct longlong1
{
  long long int x;
};

struct ulonglong1
{
  unsigned long long int x;
};

struct __builtin_align__(16) longlong2
{
  long long int x, y;
};

struct __builtin_align__(16) ulonglong2
{
  unsigned long long int x, y;
};

struct double1
{
  double x;
};

struct __builtin_align__(16) double2
{
  double x, y;
};

typedef struct char1 char1;
typedef struct uchar1 uchar1;
typedef struct char2 char2;
typedef struct uchar2 uchar2;
typedef struct char3 char3;
typedef struct uchar3 uchar3;
typedef struct char4 char4;
typedef struct uchar4 uchar4;
typedef struct short1 short1;
typedef struct ushort1 ushort1;
typedef struct short2 short2;
typedef struct ushort2 ushort2;
typedef struct short3 short3;
typedef struct ushort3 ushort3;
typedef struct short4 short4;
typedef struct ushort4 ushort4;
typedef struct int1 int1;
typedef struct uint1 uint1;
typedef struct int2 int2;
typedef struct uint2 uint2;
typedef struct int3 int3;
typedef struct uint3 uint3;
typedef struct int4 int4;
typedef struct uint4 uint4;
typedef struct long1 long1;
typedef struct ulong1 ulong1;
typedef struct long2 long2;
typedef struct ulong2 ulong2;
typedef struct long3 long3;
typedef struct ulong3 ulong3;
typedef struct long4 long4;
typedef struct ulong4 ulong4;
typedef struct float1 float1;
typedef struct float2 float2;
typedef struct float3 float3;
typedef struct float4 float4;
typedef struct longlong1 longlong1;
typedef struct ulonglong1 ulonglong1;
typedef struct longlong2 longlong2;
typedef struct ulonglong2 ulonglong2;
typedef struct double1 double1;
typedef struct double2 double2;

typedef struct dim3 dim3;

struct dim3
{
    unsigned int x, y, z;
#if defined(__cplusplus)
    dim3(unsigned int x = 1, unsigned int y = 1, unsigned int z = 1) : x(x), y(y), z(z) {}
    dim3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
    operator uint3(void) { uint3 t; t.x = x; t.y = y; t.z = z; return t; }
#endif /* __cplusplus */
};

/* CUDA Built-in Variables */

dim3  gridDim;
uint3 blockIdx;
dim3  blockDim;
uint3 threadIdx;
int   warpSize;


/* CUDA Built-in Functions */

  /* Vector Functions (constructors) */
  
static __inline__ __host__ __device__ char1 make_char1(signed char x);
static __inline__ __host__ __device__ uchar1 make_uchar1(unsigned char x);
static __inline__ __host__ __device__ char2 make_char2(signed char x, signed char y);
static __inline__ __host__ __device__ uchar2 make_uchar2(unsigned char x, unsigned char y);
static __inline__ __host__ __device__ char3 make_char3(signed char x, signed char y, signed char z);
static __inline__ __host__ __device__ uchar3 make_uchar3(unsigned char x, unsigned char y, unsigned char z);
static __inline__ __host__ __device__ char4 make_char4(signed char x, signed char y, signed char z, signed char w);
static __inline__ __host__ __device__ uchar4 make_uchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w);
static __inline__ __host__ __device__ short1 make_short1(short x);
static __inline__ __host__ __device__ ushort1 make_ushort1(unsigned short x);
static __inline__ __host__ __device__ short2 make_short2(short x, short y);
static __inline__ __host__ __device__ ushort2 make_ushort2(unsigned short x, unsigned short y);
static __inline__ __host__ __device__ short3 make_short3(short x,short y, short z);
static __inline__ __host__ __device__ ushort3 make_ushort3(unsigned short x, unsigned short y, unsigned short z);
static __inline__ __host__ __device__ short4 make_short4(short x, short y, short z, short w);
static __inline__ __host__ __device__ ushort4 make_ushort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w);
static __inline__ __host__ __device__ int1 make_int1(int x);
static __inline__ __host__ __device__ uint1 make_uint1(unsigned int x);
static __inline__ __host__ __device__ int2 make_int2(int x, int y);
static __inline__ __host__ __device__ uint2 make_uint2(unsigned int x, unsigned int y);
static __inline__ __host__ __device__ int3 make_int3(int x, int y, int z);
static __inline__ __host__ __device__ uint3 make_uint3(unsigned int x, unsigned int y, unsigned int z);
static __inline__ __host__ __device__ int4 make_int4(int x, int y, int z, int w);
static __inline__ __host__ __device__ uint4 make_uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
static __inline__ __host__ __device__ long1 make_long1(long int x);
static __inline__ __host__ __device__ ulong1 make_ulong1(unsigned long int x);
static __inline__ __host__ __device__ long2 make_long2(long int x, long int y);
static __inline__ __host__ __device__ ulong2 make_ulong2(unsigned long int x, unsigned long int y);
static __inline__ __host__ __device__ long3 make_long3(long int x, long int y, long int z);
static __inline__ __host__ __device__ ulong3 make_ulong3(unsigned long int x, unsigned long int y, unsigned long int z);
static __inline__ __host__ __device__ long4 make_long4(long int x, long int y, long int z, long int w);
static __inline__ __host__ __device__ ulong4 make_ulong4(unsigned long int x, unsigned long int y, unsigned long int z, unsigned long int w);
static __inline__ __host__ __device__ float1 make_float1(float x);
static __inline__ __host__ __device__ float2 make_float2(float x, float y);
static __inline__ __host__ __device__ float3 make_float3(float x, float y, float z);
static __inline__ __host__ __device__ float4 make_float4(float x, float y, float z, float w);
static __inline__ __host__ __device__ longlong1 make_longlong1(long long int x);
static __inline__ __host__ __device__ ulonglong1 make_ulonglong1(unsigned long long int x);
static __inline__ __host__ __device__ longlong2 make_longlong2(long long int x, long long int y);
static __inline__ __host__ __device__ ulonglong2 make_ulonglong2(unsigned long long int x, unsigned long long int y);
static __inline__ __host__ __device__ double1 make_double1(double x);
static __inline__ __host__ __device__ double2 make_double2(double x, double y);
  
  /* Synchronization functions */

__device__ void __threadfence_block();
__device__ void __threadfence();
__device__ void __threadfence_system();
__device__ void __syncthreads();
__device__ int  __syncthreads_count(int predicate);
__device__ int  __syncthreads_and(int predicate);
__device__ int  __syncthreads_or(int predicate);
  
  /* Time function */
  
__device__ clock_t clock();
  
  /* Atomic functions */

static __inline__ __device__ int atomicAdd(int *address, int val);
static __inline__ __device__ unsigned int atomicAdd(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned long long int atomicAdd(unsigned long long int *address, unsigned long long int val);
static __inline__ __device__ float atomicAdd(float *address, float val);
static __inline__ __device__ int atomicSub(int *address, int val);
static __inline__ __device__ unsigned int atomicSub(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicExch(int *address, int val);
static __inline__ __device__ unsigned int atomicExch(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned long long int atomicExch(unsigned long long int *address, unsigned long long int val);
static __inline__ __device__ float atomicExch(float *address, float val);
static __inline__ __device__ int atomicMin(int *address, int val);
static __inline__ __device__ unsigned int atomicMin(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicMax(int *address, int val);
static __inline__ __device__ unsigned int atomicMax(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned int atomicInc(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned int atomicDec(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicCAS(int *address, int compare, int val);
static __inline__ __device__ unsigned int atomicCAS(unsigned int *address, unsigned int compare, unsigned int val);
static __inline__ __device__ unsigned long long int atomicCAS(unsigned long long int *address, unsigned long long int compare, unsigned long long int val);
static __inline__ __device__ int atomicAnd(int *address, int val);
static __inline__ __device__ unsigned int atomicAnd(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicOr(int *address, int val);
static __inline__ __device__ unsigned int atomicOr(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicXor(int *address, int val);
static __inline__ __device__ unsigned int atomicXor(unsigned int *address, unsigned int val);

  /* Warp Vote functions */
  
__device__ int __all(int cond);
__device__ int __any(int cond);
__device__ unsigned int __ballot(int);

  /* Profiler Counter functions */

__device__ void __prof_trigger(int);

  /* Mathematical functions (TODO) */

  /* Texture functions (TODO) */

/* CUDA API (TODO) */

#endif


#if (ROSE_LANGUAGE_MODE == ROSE_OPENCL_LANGUAGE_MODE)

/* OpenCL Built-in Types */
  
  /* Extract and adapted from: CUDA_HOME/include/CL/cl_platform.h */

  /* Define alignment keys */

#if defined( __GNUC__ )
    #define OPENCL_ALIGNED(_x)          ROSE_GNUATTR((aligned(_x)))
#elif defined( _WIN32) && (_MSC_VER)
    /* Alignment keys neutered on windows because MSVC can't swallow function arguments with alignment requirements     */
    /* http://msdn.microsoft.com/en-us/library/373ak2y1%28VS.71%29.aspx                                                 */
    /* #include <crtdefs.h>                                                                                             */
    /* #define OPENCL_ALIGNED(_x)          _CRT_ALIGN(_x)                                                                   */
    #define OPENCL_ALIGNED(_x)
#else
   #warning  Need to implement some method to align data here
   #define  OPENCL_ALIGNED(_x)
#endif

  /* Scalar Types  */

#if (defined (_WIN32) && defined(_MSC_VER))

typedef signed   __int8         opencl_char;
typedef unsigned __int8         opencl_uchar;
typedef signed   __int16        opencl_short;
typedef unsigned __int16        opencl_ushort;
typedef signed   __int32        opencl_int;
typedef unsigned __int32        opencl_uint;
typedef signed   __int64        opencl_long;
typedef unsigned __int64        opencl_ulong;

typedef float                   opencl_float;
typedef double                  opencl_double;

typedef unsigned __int16        half;

typedef unsigned __int32        size_t;

#else

#include <stdint.h>

typedef int8_t          opencl_char;
typedef uint8_t         opencl_uchar;
typedef int16_t         opencl_short    ROSE_GNUATTR((aligned(2)));
typedef uint16_t        opencl_ushort   ROSE_GNUATTR((aligned(2)));
typedef int32_t         opencl_int      ROSE_GNUATTR((aligned(4)));
typedef uint32_t        opencl_uint     ROSE_GNUATTR((aligned(4)));
typedef int64_t         opencl_long     ROSE_GNUATTR((aligned(8)));
typedef uint64_t        opencl_ulong    ROSE_GNUATTR((aligned(8)));

typedef float           opencl_float    ROSE_GNUATTR((aligned(4)));
typedef double          opencl_double   ROSE_GNUATTR((aligned(8)));

typedef uint16_t        half     ROSE_GNUATTR((aligned(2)));

#endif

typedef uint32_t ptrdiff_t;
typedef uint32_t intptr_t;
typedef uint32_t uintptr_t;

  /* Vector Types */
  
typedef union
{
    opencl_char  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y; };
   __extension__ struct { opencl_char  s0, s1; };
   __extension__ struct { opencl_char  lo, hi; };
#endif
} char2;

typedef union
{
    opencl_char  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y, z, w; };
   __extension__ struct { opencl_char  s0, s1, s2, s3; };
   __extension__ struct { char2 lo, hi; };
#endif
} char4;

typedef union
{
    opencl_char   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y, z, w; };
   __extension__ struct { opencl_char  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { char4 lo, hi; };
#endif
} char8;

typedef union
{
    opencl_char  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_char  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { char8 lo, hi; };
#endif
} char16;

typedef union
{
    opencl_uchar  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y; };
   __extension__ struct { opencl_uchar  s0, s1; };
   __extension__ struct { opencl_uchar  lo, hi; };
#endif
} uchar2;

typedef union
{
    opencl_uchar  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y, z, w; };
   __extension__ struct { opencl_uchar  s0, s1, s2, s3; };
   __extension__ struct { uchar2 lo, hi; };
#endif
} uchar4;

typedef union
{
    opencl_uchar   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y, z, w; };
   __extension__ struct { opencl_uchar  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { uchar4 lo, hi; };
#endif
} uchar8;

typedef union
{
    opencl_uchar  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_uchar  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { uchar8 lo, hi; };
#endif
} uchar16;

typedef union
{
    opencl_short  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y; };
   __extension__ struct { opencl_short  s0, s1; };
   __extension__ struct { opencl_short  lo, hi; };
#endif
} short2;

typedef union
{
    opencl_short  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y, z, w; };
   __extension__ struct { opencl_short  s0, s1, s2, s3; };
   __extension__ struct { short2 lo, hi; };
#endif
} short4;

typedef union
{
    opencl_short   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y, z, w; };
   __extension__ struct { opencl_short  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { short4 lo, hi; };
#endif
} short8;

typedef union
{
    opencl_short  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_short  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { short8 lo, hi; };
#endif
} short16;

typedef union
{
    opencl_ushort  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y; };
   __extension__ struct { opencl_ushort  s0, s1; };
   __extension__ struct { opencl_ushort  lo, hi; };
#endif
} ushort2;

typedef union
{
    opencl_ushort  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y, z, w; };
   __extension__ struct { opencl_ushort  s0, s1, s2, s3; };
   __extension__ struct { ushort2 lo, hi; };
#endif
} ushort4;

typedef union
{
    opencl_ushort   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y, z, w; };
   __extension__ struct { opencl_ushort  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { ushort4 lo, hi; };
#endif
} ushort8;

typedef union
{
    opencl_ushort  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_ushort  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { ushort8 lo, hi; };
#endif
} ushort16;

typedef union
{
    opencl_int  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y; };
   __extension__ struct { opencl_int  s0, s1; };
   __extension__ struct { opencl_int  lo, hi; };
#endif
} int2;

typedef union
{
    opencl_int  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y, z, w; };
   __extension__ struct { opencl_int  s0, s1, s2, s3; };
   __extension__ struct { int2 lo, hi; };
#endif
} int4;

typedef union
{
    opencl_int   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y, z, w; };
   __extension__ struct { opencl_int  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { int4 lo, hi; };
#endif
} int8;

typedef union
{
    opencl_int  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_int  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { int8 lo, hi; };
#endif
} int16;

typedef union
{
    opencl_uint  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y; };
   __extension__ struct { opencl_uint  s0, s1; };
   __extension__ struct { opencl_uint  lo, hi; };
#endif
} uint2;

typedef union
{
    opencl_uint  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y, z, w; };
   __extension__ struct { opencl_uint  s0, s1, s2, s3; };
   __extension__ struct { uint2 lo, hi; };
#endif
} uint4;

typedef union
{
    opencl_uint   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y, z, w; };
   __extension__ struct { opencl_uint  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { uint4 lo, hi; };
#endif
} uint8;

typedef union
{
    opencl_uint  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_uint  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { uint8 lo, hi; };
#endif
} uint16;

typedef union
{
    opencl_long  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y; };
   __extension__ struct { opencl_long  s0, s1; };
   __extension__ struct { opencl_long  lo, hi; };
#endif
} long2;

typedef union
{
    opencl_long  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y, z, w; };
   __extension__ struct { opencl_long  s0, s1, s2, s3; };
   __extension__ struct { long2 lo, hi; };
#endif
} long4;

typedef union
{
    opencl_long   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y, z, w; };
   __extension__ struct { opencl_long  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { long4 lo, hi; };
#endif
} long8;

typedef union
{
    opencl_long  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_long  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { long8 lo, hi; };
#endif
} long16;

typedef union
{
    opencl_ulong  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y; };
   __extension__ struct { opencl_ulong  s0, s1; };
   __extension__ struct { opencl_ulong  lo, hi; };
#endif
} ulong2;

typedef union
{
    opencl_ulong  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y, z, w; };
   __extension__ struct { opencl_ulong  s0, s1, s2, s3; };
   __extension__ struct { ulong2 lo, hi; };
#endif
} ulong4;

typedef union
{
    opencl_ulong   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y, z, w; };
   __extension__ struct { opencl_ulong  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { ulong4 lo, hi; };
#endif
} ulong8;

typedef union
{
    opencl_ulong  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_ulong  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { ulong8 lo, hi; };
#endif
} ulong16;

typedef union
{
    opencl_float  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y; };
   __extension__ struct { opencl_float  s0, s1; };
   __extension__ struct { opencl_float  lo, hi; };
#endif
} float2;

typedef union
{
    opencl_float  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y, z, w; };
   __extension__ struct { opencl_float  s0, s1, s2, s3; };
   __extension__ struct { float2 lo, hi; };
#endif
} float4;

typedef union
{
    opencl_float   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y, z, w; };
   __extension__ struct { opencl_float  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { float4 lo, hi; };
#endif
} float8;

typedef union
{
    opencl_float  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_float  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { float8 lo, hi; };
#endif
} float16;

typedef union
{
    opencl_double  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y; };
   __extension__ struct { opencl_double  s0, s1; };
   __extension__ struct { opencl_double  lo, hi; };
#endif
} double2;

typedef union
{
    opencl_double  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y, z, w; };
   __extension__ struct { opencl_double  s0, s1, s2, s3; };
   __extension__ struct { double2 lo, hi; };
#endif
} double4;

typedef union
{
    opencl_double   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y, z, w; };
   __extension__ struct { opencl_double  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { double4 lo, hi; };
#endif
} double8;

typedef union
{
    opencl_double  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_double  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { double8 lo, hi; };
#endif
} double16;


/* OpenCL Built-in Functions */

  /* Work-Item Functions */

uint get_work_dim();
size_t get_global_size(uint dimindx);
size_t get_global_id(uint dimindx);
size_t get_local_size(uint dimindx);
size_t get_local_id(uint dimindx);
size_t get_num_groups(uint dimindx);
size_t get_group_id(uint dimindx);

  /* Image Functions */
  
float4 read_imagef  (image2d_t image, sampler_t sampler, int2 coord);
float4 read_imagef  (image2d_t image, sampler_t sampler, float2 coord);
int4   read_imagei  (image2d_t image, sampler_t sampler, int2 coord);
int4   read_imagei  (image2d_t image, sampler_t sampler, float2 coord);
uint4  read_imageui (image2d_t image, sampler_t sampler, int2 coord);
uint4  read_imageui (image2d_t image, sampler_t sampler, float2 coord);
void   write_imagef (image2d_t image, int2 coord, ﬂoat4 color);
void   write_imagei (image2d_t image, int2 coord, int4 color);
void   write_imageui(image2d_t image, int2 coord, uint4 color);
float4 read_imagef  (image3d_t image, sampler_t sampler, int4 coord);
float4 read_imagef  (image3d_t image, sampler_t sampler, float4 coord);
int4   read_imagei  (image3d_t image, sampler_t sampler, int4 coord);
int4   read_imagei  (image3d_t image, sampler_t sampler, float4 coord);
uint4  read_imageui (image3d_t image, sampler_t sampler, int4 coord);
uint4  read_imageui (image3d_t image, sampler_t sampler, float4 coord);
int    get_image_width (image2d_t image);
int    get_image_width (image3d_t image);
int    get_image_height(image2d_t image);
int    get_image_height(image3d_t image);
int    get_image_depht (image3d_t image);
int    get_image_channel_data_type (image2d_t image);
int    get_image_channel_data_type (image3d_t image);
int    get_image_channel_order (image2d_t image);
int    get_image_channel_order (image3d_t image);
int2   get_image_dim (image2d_t image);
int4   get_image_dim (image3d_t image);

  /* Synchronization Functions */
  
void barrier (cl_mem_fence_ﬂags ﬂags);

  /* Explicit Memory Fence Functions */
  
void mem_fence (cl_mem_fence_ﬂags ﬂags);
void read_mem_fence (cl_mem_fence_ﬂags ﬂags);
void write_mem_fence (cl_mem_fence_ﬂags ﬂags);

  /* Miscellaneous Functions */
  
void wait_group_events (int num_events, event_t * event_list);
event_t async_work_group_copy (__local char *dst, const __global char *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char *dst, const __local char *src, size_t num_elements, event_t event);
void prefetch (const __global char *p, size_t num_elements);
event_t async_work_group_copy (__local char2 *dst, const __global char2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char2 *dst, const __local char2 *src, size_t num_elements, event_t event);
void prefetch (const __global char2 *p, size_t num_elements);
event_t async_work_group_copy (__local char4 *dst, const __global char4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char4 *dst, const __local char4 *src, size_t num_elements, event_t event);
void prefetch (const __global char4 *p, size_t num_elements);
event_t async_work_group_copy (__local char8 *dst, const __global char8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char8 *dst, const __local char8 *src, size_t num_elements, event_t event);
void prefetch (const __global char8 *p, size_t num_elements);
event_t async_work_group_copy (__local char16 *dst, const __global char16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char16 *dst, const __local char16 *src, size_t num_elements, event_t event);
void prefetch (const __global char16 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar *dst, const __global uchar *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar *dst, const __local uchar *src, size_t num_elements, event_t event);
void prefetch (const __global uchar *p, size_t num_elements);
event_t async_work_group_copy (__local uchar2 *dst, const __global uchar2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar2 *dst, const __local uchar2 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar2 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar4 *dst, const __global uchar4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar4 *dst, const __local uchar4 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar4 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar8 *dst, const __global uchar8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar8 *dst, const __local uchar8 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar8 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar16 *dst, const __global uchar16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar16 *dst, const __local uchar16 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar16 *p, size_t num_elements);
event_t async_work_group_copy (__local short *dst, const __global short *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short *dst, const __local short *src, size_t num_elements, event_t event);
void prefetch (const __global short *p, size_t num_elements);
event_t async_work_group_copy (__local short2 *dst, const __global short2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short2 *dst, const __local short2 *src, size_t num_elements, event_t event);
void prefetch (const __global short2 *p, size_t num_elements);
event_t async_work_group_copy (__local short4 *dst, const __global short4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short4 *dst, const __local short4 *src, size_t num_elements, event_t event);
void prefetch (const __global short4 *p, size_t num_elements);
event_t async_work_group_copy (__local short8 *dst, const __global short8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short8 *dst, const __local short8 *src, size_t num_elements, event_t event);
void prefetch (const __global short8 *p, size_t num_elements);
event_t async_work_group_copy (__local short16 *dst, const __global short16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short16 *dst, const __local short16 *src, size_t num_elements, event_t event);
void prefetch (const __global short16 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort *dst, const __global ushort *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort *dst, const __local ushort *src, size_t num_elements, event_t event);
void prefetch (const __global ushort *p, size_t num_elements);
event_t async_work_group_copy (__local ushort2 *dst, const __global ushort2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort2 *dst, const __local ushort2 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort2 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort4 *dst, const __global ushort4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort4 *dst, const __local ushort4 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort4 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort8 *dst, const __global ushort8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort8 *dst, const __local ushort8 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort8 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort16 *dst, const __global ushort16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort16 *dst, const __local ushort16 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort16 *p, size_t num_elements);
event_t async_work_group_copy (__local int *dst, const __global int *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int *dst, const __local int *src, size_t num_elements, event_t event);
void prefetch (const __global int *p, size_t num_elements);
event_t async_work_group_copy (__local int2 *dst, const __global int2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int2 *dst, const __local int2 *src, size_t num_elements, event_t event);
void prefetch (const __global int2 *p, size_t num_elements);
event_t async_work_group_copy (__local int4 *dst, const __global int4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int4 *dst, const __local int4 *src, size_t num_elements, event_t event);
void prefetch (const __global int4 *p, size_t num_elements);
event_t async_work_group_copy (__local int8 *dst, const __global int8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int8 *dst, const __local int8 *src, size_t num_elements, event_t event);
void prefetch (const __global int8 *p, size_t num_elements);
event_t async_work_group_copy (__local int16 *dst, const __global int16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int16 *dst, const __local int16 *src, size_t num_elements, event_t event);
void prefetch (const __global int16 *p, size_t num_elements);
event_t async_work_group_copy (__local uint *dst, const __global uint *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint *dst, const __local uint *src, size_t num_elements, event_t event);
void prefetch (const __global uint *p, size_t num_elements);
event_t async_work_group_copy (__local uint2 *dst, const __global uint2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint2 *dst, const __local uint2 *src, size_t num_elements, event_t event);
void prefetch (const __global uint2 *p, size_t num_elements);
event_t async_work_group_copy (__local uint4 *dst, const __global uint4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint4 *dst, const __local uint4 *src, size_t num_elements, event_t event);
void prefetch (const __global uint4 *p, size_t num_elements);
event_t async_work_group_copy (__local uint8 *dst, const __global uint8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint8 *dst, const __local uint8 *src, size_t num_elements, event_t event);
void prefetch (const __global uint8 *p, size_t num_elements);
event_t async_work_group_copy (__local uint16 *dst, const __global uint16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint16 *dst, const __local uint16 *src, size_t num_elements, event_t event);
void prefetch (const __global uint16 *p, size_t num_elements);
event_t async_work_group_copy (__local long *dst, const __global long *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long *dst, const __local long *src, size_t num_elements, event_t event);
void prefetch (const __global long *p, size_t num_elements);
event_t async_work_group_copy (__local long2 *dst, const __global long2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long2 *dst, const __local long2 *src, size_t num_elements, event_t event);
void prefetch (const __global long2 *p, size_t num_elements);
event_t async_work_group_copy (__local long4 *dst, const __global long4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long4 *dst, const __local long4 *src, size_t num_elements, event_t event);
void prefetch (const __global long4 *p, size_t num_elements);
event_t async_work_group_copy (__local long8 *dst, const __global long8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long8 *dst, const __local long8 *src, size_t num_elements, event_t event);
void prefetch (const __global long8 *p, size_t num_elements);
event_t async_work_group_copy (__local long16 *dst, const __global long16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long16 *dst, const __local long16 *src, size_t num_elements, event_t event);
void prefetch (const __global long16 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong *dst, const __global ulong *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong *dst, const __local ulong *src, size_t num_elements, event_t event);
void prefetch (const __global ulong *p, size_t num_elements);
event_t async_work_group_copy (__local ulong2 *dst, const __global ulong2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong2 *dst, const __local ulong2 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong2 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong4 *dst, const __global ulong4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong4 *dst, const __local ulong4 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong4 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong8 *dst, const __global ulong8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong8 *dst, const __local ulong8 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong8 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong16 *dst, const __global ulong16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong16 *dst, const __local ulong16 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong16 *p, size_t num_elements);
event_t async_work_group_copy (__local float *dst, const __global float *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float *dst, const __local float *src, size_t num_elements, event_t event);
void prefetch (const __global float *p, size_t num_elements);
event_t async_work_group_copy (__local float2 *dst, const __global float2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float2 *dst, const __local float2 *src, size_t num_elements, event_t event);
void prefetch (const __global float2 *p, size_t num_elements);
event_t async_work_group_copy (__local float4 *dst, const __global float4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float4 *dst, const __local float4 *src, size_t num_elements, event_t event);
void prefetch (const __global float4 *p, size_t num_elements);
event_t async_work_group_copy (__local float8 *dst, const __global float8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float8 *dst, const __local float8 *src, size_t num_elements, event_t event);
void prefetch (const __global float8 *p, size_t num_elements);
event_t async_work_group_copy (__local float16 *dst, const __global float16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float16 *dst, const __local float16 *src, size_t num_elements, event_t event);
void prefetch (const __global float16 *p, size_t num_elements);

  /* Miscellaneous Functions (TODO) */



#endif

/* DQ (9/2/2016): Comment out to use new automated generation of builtin functions. */
#endif


/* DQ (11/20/2016): This should be implemented only for EDG 4.11. */
/* DQ (9/12/2016): Comment out to use new automated generation of builtin functions. */
/* DQ (10/18/2016): Turn this on for EDG 4.11 but not wanted for EDG 4.12 (we use our own mechanism for EDG versions before EDG 4.11) */
#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 411

/***************************************************************************************
  SSE and MMX support (declarations for builtin functions defined in GNU header files)
****************************************************************************************/

// DQ (1/20/2017): Changed to allow testing of EDG 4.12 with GNU 4.8 and GNU 4.9.
#if 1
/* SSE3 builtin functions required for GNU 6.1 compiler */
/* SSE2 */
typedef double __v2df ROSE_GNUATTR((__vector_size__ (16)));
typedef long long __v2di ROSE_GNUATTR((__vector_size__ (16)));
typedef int __v4si ROSE_GNUATTR((__vector_size__ (16)));
typedef short __v8hi ROSE_GNUATTR((__vector_size__ (16)));
typedef char __v16qi ROSE_GNUATTR((__vector_size__ (16)));

/* Internal data types for implementing the intrinsics.  */
typedef int __v2si ROSE_GNUATTR((__vector_size__ (8)));
typedef char __v8qi ROSE_GNUATTR((__vector_size__ (8)));

/* The Intel API is flexible enough that we must allow aliasing with other
   vector types, and their scalar components.  */
// typedef float __m128 ROSE_GNUATTR((__vector_size__ (32), __may_alias__));
typedef float __m128 ROSE_GNUATTR((__vector_size__ (16), __may_alias__));
typedef long long __m128i ROSE_GNUATTR((__vector_size__ (16), __may_alias__));
typedef double __m128d ROSE_GNUATTR((__vector_size__ (16), __may_alias__));

/* Not clear what these should be  */
typedef int __m64 ROSE_GNUATTR((__vector_size__ (8), __may_alias__));
typedef short __v4hi ROSE_GNUATTR((__vector_size__ (8)));
// typedef long long __m64 ROSE_GNUATTR((__vector_size__ (16)));
// typedef int __v4hi ROSE_GNUATTR((__vector_size__ (16)));
typedef float __v2sf ROSE_GNUATTR((__vector_size__ (8)));
typedef float __v4sf ROSE_GNUATTR((__vector_size__ (16)));

typedef long long __v1di ROSE_GNUATTR((__vector_size__ (8)));

/* From: mmintrin.h */
void __builtin_ia32_emms();
__m64 __builtin_ia32_vec_init_v2si(int,int);
int __builtin_ia32_vec_ext_v2si(__v2si,int);
__m64 __builtin_ia32_packsswb(__v4hi,__v4hi);
__m64 __builtin_ia32_packssdw(__v2si,__v2si);
__m64 __builtin_ia32_packuswb(__v4hi,__v4hi);
__m64 __builtin_ia32_punpckhbw(__v8qi,__v8qi);
__m64 __builtin_ia32_punpckhwd(__v4hi,__v4hi);
__m64 __builtin_ia32_punpckhdq(__v2si,__v2si);
__m64 __builtin_ia32_punpcklbw(__v8qi,__v8qi);
__m64 __builtin_ia32_punpcklwd(__v4hi,__v4hi);
__m64 __builtin_ia32_punpckldq(__v2si,__v2si);
__m64 __builtin_ia32_paddb(__v8qi,__v8qi);
__m64 __builtin_ia32_paddw(__v4hi,__v4hi);
__m64 __builtin_ia32_paddd(__v2si,__v2si);
__m64 __builtin_ia32_paddq(__v1di,__v1di);
__m64 __builtin_ia32_paddsb(__v8qi,__v8qi);
__m64 __builtin_ia32_paddsw(__v4hi,__v4hi);
__m64 __builtin_ia32_paddusb(__v8qi,__v8qi);
__m64 __builtin_ia32_paddusw(__v4hi,__v4hi);
__m64 __builtin_ia32_psubb(__v8qi,__v8qi);
__m64 __builtin_ia32_psubw(__v4hi,__v4hi);
__m64 __builtin_ia32_psubd(__v2si,__v2si);
__m64 __builtin_ia32_psubq(__v1di,__v1di);
__m64 __builtin_ia32_psubsb(__v8qi,__v8qi);
__m64 __builtin_ia32_psubsw(__v4hi,__v4hi);
__m64 __builtin_ia32_psubusb(__v8qi,__v8qi);
__m64 __builtin_ia32_psubusw(__v4hi,__v4hi);
__m64 __builtin_ia32_pmaddwd(__v4hi,__v4hi);
__m64 __builtin_ia32_pmulhw(__v4hi,__v4hi);
__m64 __builtin_ia32_pmullw(__v4hi,__v4hi);
__m64 __builtin_ia32_psllw(__v4hi,__v4hi);
__m64 __builtin_ia32_psllwi(__v4hi,int);
__m64 __builtin_ia32_pslld(__v2si,__v2si);
__m64 __builtin_ia32_pslldi(__v2si,int);
__m64 __builtin_ia32_psllq(__v1di,__v1di);
__m64 __builtin_ia32_psllqi(__v1di,long long);
__m64 __builtin_ia32_psraw(__v4hi,__v4hi);
__m64 __builtin_ia32_psrawi(__v4hi,long long);
__m64 __builtin_ia32_psrad(__v2si,__v2si);
__m64 __builtin_ia32_psradi(__v2si, long long);
__m64 __builtin_ia32_psrlw(__v4hi,__v4hi);
__m64 __builtin_ia32_psrlwi(__v4hi,long long);
__m64 __builtin_ia32_psrld(__v2si,__v2si);
__m64 __builtin_ia32_psrldi(__v2si, long long);
__m64 __builtin_ia32_psrlq(__v1di,__v1di);
__m64 __builtin_ia32_psrlqi(__v1di, long long);
__m64 __builtin_ia32_pand(__m64,__m64);
__m64 __builtin_ia32_pandn(__m64,__m64);
__m64 __builtin_ia32_por(__m64,__m64);
__m64 __builtin_ia32_pxor(__m64,__m64);
__m64 __builtin_ia32_pcmpeqb(__v8qi,__v8qi);
__v8qi __builtin_ia32_pcmpgtb(__v8qi,__v8qi);
__m64 __builtin_ia32_pcmpeqw(__v4hi,__v4hi);
__v4hi __builtin_ia32_pcmpgtw(__v4hi,__v4hi);
__m64 __builtin_ia32_pcmpeqd(__v2si,__v2si);
__m64 __builtin_ia32_pcmpgtd(__v2si,__v2si);
// int __builtin_ia32_vec_init_v2si(int,int);
__m64 __builtin_ia32_vec_init_v4hi(short,short,short,short);
__m64 __builtin_ia32_vec_init_v8qi(char,char,char,char,char,char,char,char);

/* From: xmmintrin.h */
__m128 __builtin_ia32_addss(__v4sf,__v4sf);
__m128 __builtin_ia32_subss(__v4sf,__v4sf);
__m128 __builtin_ia32_mulss(__v4sf,__v4sf);
__m128 __builtin_ia32_divss(__v4sf,__v4sf);
__m128 __builtin_ia32_sqrtss(__v4sf);
__m128 __builtin_ia32_rcpss(__v4sf);
__m128 __builtin_ia32_rsqrtss(__v4sf);
__m128 __builtin_ia32_minss(__v4sf,__v4sf);
__m128 __builtin_ia32_maxss(__v4sf,__v4sf);
__m128 __builtin_ia32_addps(__v4sf,__v4sf);
__m128 __builtin_ia32_subps(__v4sf,__v4sf);
__m128 __builtin_ia32_mulps(__v4sf,__v4sf);
__m128 __builtin_ia32_divps(__v4sf,__v4sf);
__m128 __builtin_ia32_sqrtps(__v4sf);
__m128 __builtin_ia32_rcpps(__v4sf);
__m128 __builtin_ia32_rsqrtps(__v4sf);
__m128 __builtin_ia32_minps(__v4sf,__v4sf);
__m128 __builtin_ia32_maxps(__v4sf,__v4sf);
__m128 __builtin_ia32_andps(__v4sf,__v4sf);
__m128 __builtin_ia32_andnps(__v4sf,__v4sf);
__m128 __builtin_ia32_orps(__v4sf,__v4sf);
__m128 __builtin_ia32_xorps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpeqss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpltss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpless(__v4sf,__v4sf);
// int __builtin_ia32_cmpltss(float,float);
__m128 __builtin_ia32_movss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpless(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpneqss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpnltss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpnless(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpordss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpunordss(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpeqps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpltps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpleps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpgtps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpgeps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpneqps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpnltps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpnleps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpngtps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpngeps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpordps(__v4sf,__v4sf);
__m128 __builtin_ia32_cmpunordps(__v4sf,__v4sf);
int __builtin_ia32_comieq(__v4sf,__v4sf);
int __builtin_ia32_comilt(__v4sf,__v4sf);
int __builtin_ia32_comile(__v4sf,__v4sf);
int __builtin_ia32_comigt(__v4sf,__v4sf);
int __builtin_ia32_comige(__v4sf,__v4sf);
int __builtin_ia32_comineq(__v4sf,__v4sf);
int __builtin_ia32_ucomieq(__v4sf,__v4sf);
int __builtin_ia32_ucomilt(__v4sf,__v4sf);
int __builtin_ia32_ucomile(__v4sf,__v4sf);
int __builtin_ia32_ucomigt(__v4sf,__v4sf);
int __builtin_ia32_ucomige(__v4sf,__v4sf);
int __builtin_ia32_ucomineq(__v4sf,__v4sf);
int __builtin_ia32_cvtss2si(__v4sf);
int __builtin_ia32_cvtss2si64(__v4sf);
__m64 __builtin_ia32_cvtps2pi(__v4sf);
int __builtin_ia32_cvttss2si(__v4sf);
int __builtin_ia32_cvttss2si64(__v4sf);
__m64 __builtin_ia32_cvttps2pi(__v4sf);
__m128 __builtin_ia32_cvtsi2ss(__v4sf,int);
__m128 __builtin_ia32_cvtsi642ss(__v4sf,long long);
__m128 __builtin_ia32_cvtpi2ps(__v4sf,__v2si);
__m128 __builtin_ia32_movlhps(__v4sf,__v4sf);
__m128 __builtin_ia32_movhlps(__v4sf,__v4sf);
__m64 __builtin_ia32_cvtps2pi(__v4sf);
__m128 __builtin_ia32_unpckhps(__v4sf,__v4sf);
__m128 __builtin_ia32_unpcklps(__v4sf,__v4sf);
__m128 __builtin_ia32_loadhps(__v4sf,const __v2sf *);
__m128 __builtin_ia32_storehps(__v2sf *,__v4sf);
__m128 __builtin_ia32_loadlps(__v4sf,const __v2sf *);
__m128 __builtin_ia32_storelps(__v2sf *,__v4sf);
int __builtin_ia32_movmskps(__v4sf);
unsigned int __builtin_ia32_stmxcsr();
unsigned int __builtin_ia32_ldmxcsr(int);
__m128 __builtin_ia32_loadups(float const *);
__m128 __builtin_ia32_shufps(__v4sf,__v4sf,int);
float __builtin_ia32_vec_ext_v4sf(__v4sf,int);
void  __builtin_ia32_storeups(float *,__v4sf);
__m64 __builtin_ia32_pmaxsw(__v4hi,__v4hi);
__m64 __builtin_ia32_pmaxub(__v8qi,__v8qi);
__m64 __builtin_ia32_pminsw(__v4hi,__v4hi);
__m64 __builtin_ia32_pminub(__v8qi,__v8qi);
int __builtin_ia32_pmovmskb(__v8qi);
__m64 __builtin_ia32_pmulhuw(__v4hi,__v4hi);
void __builtin_ia32_maskmovq(__v8qi,__v8qi,char*);
__m64 __builtin_ia32_pavgb(__v8qi,__v8qi);
__m64 __builtin_ia32_pavgw(__v4hi,__v4hi);
__m64 __builtin_ia32_psadbw(__v8qi,__v8qi);
int __builtin_ia32_movntq(unsigned long long*,unsigned long long);
void __builtin_ia32_movntps(float *,__v4sf);
int __builtin_ia32_sfence();


/* From: emmintrin.h */
__m128d __builtin_ia32_movsd(__v2df,__v2df);
__m128d __builtin_ia32_loadupd(double const *);
__m128d __builtin_ia32_shufpd(__m128d,__m128d,int);
void __builtin_ia32_storeupd(double*,__m128d);
double __builtin_ia32_vec_ext_v2df(__m128d,int);
/* int __builtin_ia32_shufpd(); */
int __builtin_ia32_vec_ext_v4si(__v4si,int);
long long __builtin_ia32_vec_ext_v2di(__v2di,int);
__m128d __builtin_ia32_addpd(__v2df,__v2df);
__m128d __builtin_ia32_addsd(__v2df,__v2df);
__m128d __builtin_ia32_subpd(__v2df,__v2df);
__m128d __builtin_ia32_subsd(__v2df,__v2df);
__m128d __builtin_ia32_mulpd(__v2df,__v2df);
__m128d __builtin_ia32_mulsd(__v2df,__v2df);
__m128d __builtin_ia32_divpd(__v2df,__v2df);
__m128d __builtin_ia32_divsd(__v2df,__v2df);
__m128d __builtin_ia32_sqrtpd(__v2df);
__m128d __builtin_ia32_sqrtsd(__v2df);
__m128d __builtin_ia32_minpd(__v2df,__v2df);
__m128d __builtin_ia32_minsd(__v2df,__v2df);
__m128d __builtin_ia32_maxpd(__v2df,__v2df);
__m128d __builtin_ia32_maxsd(__v2df,__v2df);
__m128d __builtin_ia32_andpd(__v2df,__v2df);
__m128d __builtin_ia32_andnpd(__v2df,__v2df);
__m128d __builtin_ia32_orpd(__v2df,__v2df);
__m128d __builtin_ia32_xorpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpeqpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpltpd(__v2df,__v2df);
__m128d __builtin_ia32_cmplepd(__v2df,__v2df);
__m128d __builtin_ia32_cmpgtpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpgepd(__v2df,__v2df);
__m128d __builtin_ia32_cmpneqpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpnltpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpnlepd(__v2df,__v2df);
__m128d __builtin_ia32_cmpngtpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpngepd(__v2df,__v2df);
__m128d __builtin_ia32_cmpordpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpunordpd(__v2df,__v2df);
__m128d __builtin_ia32_cmpeqsd(__v2df,__v2df);
__m128d __builtin_ia32_cmpltsd(__v2df,__v2df);
__m128d __builtin_ia32_cmplesd(__v2df,__v2df);
__m128d __builtin_ia32_cmpltsd(__v2df,__v2df);
__m128d __builtin_ia32_cmplesd(__v2df,__v2df);
__m128d __builtin_ia32_cmpneqsd(__v2df,__v2df);
__m128d __builtin_ia32_cmpnltsd(__v2df,__v2df);
__m128d __builtin_ia32_cmpnlesd(__v2df,__v2df);
__m128d __builtin_ia32_cmpordsd(__v2df,__v2df);
__m128d __builtin_ia32_cmpunordsd(__v2df,__v2df);
int __builtin_ia32_comisdeq(__v2df,__v2df);
int __builtin_ia32_comisdlt(__v2df,__v2df);
int __builtin_ia32_comisdle(__v2df,__v2df);
int __builtin_ia32_comisdgt(__v2df,__v2df);
int __builtin_ia32_comisdge(__v2df,__v2df);
int __builtin_ia32_comisdg(__v2df,__v2df);
int __builtin_ia32_comisdneq(__v2df,__v2df);
int __builtin_ia32_ucomisdeq(__v2df,__v2df);
int __builtin_ia32_ucomisdlt(__v2df,__v2df);
int __builtin_ia32_ucomisdle(__v2df,__v2df);
int __builtin_ia32_ucomisdgt(__v2df,__v2df);
int __builtin_ia32_ucomisdge(__v2df,__v2df);
int __builtin_ia32_ucomisdneq(__v2df,__v2df);
__m128i __builtin_ia32_loaddqu(char const *);
void    __builtin_ia32_storedqu(char*, __v16qi);
__m128d __builtin_ia32_cvtdq2pd(__v4si);
__m128  __builtin_ia32_cvtdq2ps(__v4si);
__m128i __builtin_ia32_cvtpd2dq(__v2df);
__m128i __builtin_ia32_movq128 (__v2di);
__m64   __builtin_ia32_cvtpd2pi(__v2df);
__m128  __builtin_ia32_cvtpd2ps(__v2df);
__m128i __builtin_ia32_cvttpd2dq(__v2df);
__m64 __builtin_ia32_cvttpd2pi(__v2df);
__m128d __builtin_ia32_cvtpi2pd(__v2si);
__m128i __builtin_ia32_cvtps2dq(__v4sf);
__m128i __builtin_ia32_cvttps2dq(__v4sf);
__m128d __builtin_ia32_cvtps2pd(__v4sf);
int __builtin_ia32_cvtsd2si(__v2df);
int __builtin_ia32_cvtsd2si64(__v2df);
int __builtin_ia32_cvtsd2si64(__v2df);
int __builtin_ia32_cvttsd2si(__v2df);
int __builtin_ia32_cvttsd2si64(__v2df);
__m128 __builtin_ia32_cvtsd2ss(__v4sf,__v2df);
__m128d __builtin_ia32_cvtsi2sd (__v2df , int);
__m128d __builtin_ia32_cvtsi642sd (__v2df, long long);
__m128d __builtin_ia32_cvtss2sd (__v2df, __v4sf);
__m128d __builtin_ia32_unpckhpd (__v2df, __v2df);
__m128d __builtin_ia32_unpcklpd (__v2df, __v2df);
__m128d __builtin_ia32_loadhpd (__v2df, double const *);
__m128d __builtin_ia32_loadlpd(__v2df, double const *);
__m128i __builtin_ia32_packsswb128 (__v8hi, __v8hi);
__m128i __builtin_ia32_packssdw128 (__v4si, __v4si);
__m128i __builtin_ia32_packuswb128 (__v8hi, __v8hi);
__m128i __builtin_ia32_punpckhbw128 (__v16qi, __v16qi);
__m128i __builtin_ia32_punpckhwd128 (__v8hi, __v8hi);
__m128i __builtin_ia32_punpckhdq128 (__v4si, __v4si);
__m128i __builtin_ia32_punpckhqdq128 (__v2di, __v2di);
__m128i __builtin_ia32_punpcklbw128 (__v16qi, __v16qi);
__m128i __builtin_ia32_punpcklwd128 (__v8hi, __v8hi);
__m128i __builtin_ia32_punpckldq128 (__v4si, __v4si);
__m128i __builtin_ia32_punpcklqdq128 (__v2di, __v2di);
__m128i __builtin_ia32_paddb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_paddw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_paddd128 (__v4si, __v4si);
__m128i __builtin_ia32_paddq128 (__v2di, __v2di);
__m128i __builtin_ia32_paddsb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_paddsw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_paddusb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_paddusw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psubb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_psubw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psubd128 (__v4si, __v4si);
__m128i __builtin_ia32_psubq128 (__v2di, __v2di);
__m128i __builtin_ia32_psubsb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_psubsw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psubusb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_psubusw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pmaddwd128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pmulhw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pmullw128 (__v8hi, __v8hi);
__m64 __builtin_ia32_pmuludq (__v2si, __v2si);
__m128i __builtin_ia32_pmuludq128 (__v4si, __v4si);
__v2di  __builtin_ia32_pslldqi128 (__v2di, int);
__v8hi __builtin_ia32_pshuflw (__v8hi, int);
__m128i __builtin_ia32_psllwi128 (__v8hi, int);
__m128i __builtin_ia32_pslldi128 (__v4si, int);
__m128i __builtin_ia32_psllqi128 (__v2di, int);
__v2di  __builtin_ia32_psrldqi128 (__v2di, int);
__m128i __builtin_ia32_psrawi128 (__v8hi, int);
__m128i __builtin_ia32_psradi128 (__v4si, int);
__m128i __builtin_ia32_psrlwi128 (__v8hi, int);
__m128i __builtin_ia32_psrldi128 (__v4si, int);
__m128i __builtin_ia32_psrlqi128 (__v2di, int);
__m128i __builtin_ia32_psllw128(__v8hi, __v8hi);
__m128i __builtin_ia32_pslld128(__v4si, __v4si);
__m128i __builtin_ia32_psllq128(__v2di, __v2di);
__m128i __builtin_ia32_psraw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psrad128 (__v4si, __v4si);
__m128i __builtin_ia32_psrlw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psrld128 (__v4si, __v4si);
__m128i __builtin_ia32_psrlq128 (__v2di, __v2di);
__m128i __builtin_ia32_pand128 (__v2di, __v2di);
__m128i __builtin_ia32_pandn128 (__v2di, __v2di);
__m128i __builtin_ia32_por128 (__v2di, __v2di);
__m128i __builtin_ia32_pxor128 (__v2di, __v2di);
__m128i __builtin_ia32_pcmpeqb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pcmpeqw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pcmpeqd128 (__v4si, __v4si);
__m128i __builtin_ia32_pcmpgtb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pcmpgtw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pcmpgtd128 (__v4si, __v4si);
__m128i __builtin_ia32_pcmpgtb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pcmpgtw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pcmpgtd128 (__v4si, __v4si);
__m128i __builtin_ia32_pmaxsw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pmaxub128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pminsw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pminub128 (__v16qi, __v16qi);
int     __builtin_ia32_pmovmskb128 (__v16qi);
__m128i __builtin_ia32_pmulhuw128 (__v8hi, __v8hi);
void    __builtin_ia32_maskmovdqu (__v16qi, __v16qi, char*);
__m128i __builtin_ia32_pavgb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pavgw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psadbw128 (__v16qi, __v16qi);
void __builtin_ia32_movnti64 (long long int *,long long int);
void __builtin_ia32_movntdq (__v2di *, __v2di);
void __builtin_ia32_movntpd (double *, __v2df);
int __builtin_ia32_movmskpd(__v2df);
void __builtin_ia32_movnti(int*,int);

int __builtin_ia32_clflush(void const *);
int __builtin_ia32_lfence();
int __builtin_ia32_mfence();

// Builtin functions specific to GNU 4.4.1 (likely 4.4.x)
// int __builtin_ia32_psllwi(short,int);
// int __builtin_ia32_pslldi(int,int);
// int __builtin_ia32_psllqi(long long,int);
// int __builtin_ia32_psrawi(short,int);
// int __builtin_ia32_psradi(int,int);
// int __builtin_ia32_psrlwi(int,int);
// int __builtin_ia32_psrldi(int,int);
// int __builtin_ia32_psrlqi(long long,int);

// DQ (1/21/2017): Added SSE builtins
#endif

#if 1
// DQ (1/22/2017): Added AVX builtin functions support.
typedef double __v4df ROSE_GNUATTR((__vector_size__ (32)));
typedef float __v8sf ROSE_GNUATTR((__vector_size__ (32)));
typedef long long __v4di ROSE_GNUATTR((__vector_size__ (32)));
typedef int __v8si ROSE_GNUATTR((__vector_size__ (32)));
typedef short __v16hi ROSE_GNUATTR((__vector_size__ (32)));
typedef char __v32qi ROSE_GNUATTR((__vector_size__ (32)));

typedef float __m256 ROSE_GNUATTR((__vector_size__ (32), __may_alias__));
typedef long long __m256i ROSE_GNUATTR((__vector_size__ (32), __may_alias__));
typedef double __m256d ROSE_GNUATTR((__vector_size__ (32), __may_alias__));

typedef double __v8df ROSE_GNUATTR((__vector_size__ (64)));
typedef float __v16sf ROSE_GNUATTR((__vector_size__ (64)));
typedef long long __v8di ROSE_GNUATTR((__vector_size__ (64)));
typedef int __v16si ROSE_GNUATTR((__vector_size__ (64)));
typedef short __v32hi ROSE_GNUATTR((__vector_size__ (64)));
typedef char __v64qi ROSE_GNUATTR((__vector_size__ (64)));
typedef float __m512 ROSE_GNUATTR((__vector_size__ (64), __may_alias__));
typedef long long __m512i ROSE_GNUATTR((__vector_size__ (64), __may_alias__));
typedef double __m512d ROSE_GNUATTR((__vector_size__ (64), __may_alias__));
typedef unsigned char __mmask8;
typedef unsigned short __mmask16;

__m128 __builtin_ia32_addsubps (__v4sf, __v4sf);
__m128 __builtin_ia32_haddps (__v4sf, __v4sf);
__m128 __builtin_ia32_hsubps (__v4sf, __v4sf);
__m128 __builtin_ia32_movshdup (__v4sf);
__m128 __builtin_ia32_movsldup (__v4sf);
__m128d __builtin_ia32_addsubpd (__v2df, __v2df);
__m128d __builtin_ia32_haddpd (__v2df, __v2df);
__m128d __builtin_ia32_hsubpd (__v2df, __v2df);
__m128i __builtin_ia32_lddqu (char const *__P);
void __builtin_ia32_monitor (void const *, unsigned int, unsigned int);
void __builtin_ia32_mwait (unsigned int, unsigned int);
__m128i __builtin_ia32_phaddw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_phaddd128 (__v4si, __v4si);
__m128i __builtin_ia32_phaddsw128 (__v8hi, __v8hi);
__m64 __builtin_ia32_phaddw (__v4hi, __v4hi);
__m64 __builtin_ia32_phaddd (__v2si, __v2si);
__m64 __builtin_ia32_phaddsw (__v4hi, __v4hi);
__m128i __builtin_ia32_phsubw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_phsubd128 (__v4si, __v4si);
__m128i __builtin_ia32_phsubsw128 (__v8hi, __v8hi);
__m64 __builtin_ia32_phsubw (__v4hi, __v4hi);
__m64 __builtin_ia32_phsubd (__v2si, __v2si);
__m64 __builtin_ia32_phsubsw (__v4hi, __v4hi);
__m128i __builtin_ia32_pmaddubsw128 (__v16qi, __v16qi);
__m64 __builtin_ia32_pmaddubsw (__v8qi, __v8qi);
__m128i __builtin_ia32_pmulhrsw128 (__v8hi, __v8hi);
__m64 __builtin_ia32_pmulhrsw (__v4hi, __v4hi);
__m128i __builtin_ia32_pshufb128 (__v16qi, __v16qi);
__m64 __builtin_ia32_pshufb (__v8qi, __v8qi);
__m128i __builtin_ia32_psignb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_psignw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_psignd128 (__v4si, __v4si);
__m64 __builtin_ia32_psignb (__v8qi, __v8qi);
__m64 __builtin_ia32_psignw (__v4hi, __v4hi);
__m64 __builtin_ia32_psignd (__v2si, __v2si);
__m128i __builtin_ia32_pabsb128 (__v16qi);
__m128i __builtin_ia32_pabsw128 (__v8hi);
__m128i __builtin_ia32_pabsd128 (__v4si);
__m64 __builtin_ia32_pabsb (__v8qi);
__m64 __builtin_ia32_pabsw (__v4hi);
__m64 __builtin_ia32_pabsd (__v2si);
void __builtin_ia32_movntsd (double*, __v2df);
void __builtin_ia32_movntss (float*, __v4sf);
__m128i __builtin_ia32_extrq (__v2di, __v16qi);
__m128i __builtin_ia32_insertq (__v2di, __v2di);
int __builtin_ia32_ptestz128 (__v2di, __v2di);
int __builtin_ia32_ptestc128 (__v2di, __v2di);
int __builtin_ia32_ptestnzc128 (__v2di, __v2di);
__m128i __builtin_ia32_pblendvb128 (__v16qi, __v16qi, __v16qi);
__m128 __builtin_ia32_blendvps (__v4sf, __v4sf, __v4sf );
__m128d __builtin_ia32_blendvpd (__v2df, __v2df, __v2df );
__m128i __builtin_ia32_pcmpeqq (__v2di, __v2di);
__m128i __builtin_ia32_pminsb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pmaxsb128 (__v16qi, __v16qi);
__m128i __builtin_ia32_pminuw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pmaxuw128 (__v8hi, __v8hi);
__m128i __builtin_ia32_pminsd128 (__v4si, __v4si);
__m128i __builtin_ia32_pmaxsd128 (__v4si, __v4si);
__m128i __builtin_ia32_pminud128 (__v4si, __v4si);
__m128i __builtin_ia32_pmaxud128 (__v4si, __v4si);
__m128i __builtin_ia32_pmulld128 (__v4si, __v4si);
__m128i __builtin_ia32_pmuldq128 (__v4si, __v4si);
__m128i __builtin_ia32_phminposuw128 (__v8hi);
__m128i __builtin_ia32_pmovsxbd128 (__v16qi);
__m128i __builtin_ia32_pmovsxwd128 (__v8hi);
__m128i __builtin_ia32_pmovsxbq128 (__v16qi);
__m128i __builtin_ia32_pmovsxdq128 (__v4si);
__m128i __builtin_ia32_pmovsxwq128 (__v8hi);
__m128i __builtin_ia32_pmovsxbw128 (__v16qi);
__m128i __builtin_ia32_pmovzxbd128 (__v16qi);
__m128i __builtin_ia32_pmovzxwd128 (__v8hi);
__m128i __builtin_ia32_pmovzxbq128 (__v16qi);
__m128i __builtin_ia32_pmovzxdq128 (__v4si);
__m128i __builtin_ia32_pmovzxwq128 (__v8hi);
__m128i __builtin_ia32_pmovzxbw128 (__v16qi);
__m128i __builtin_ia32_packusdw128 (__v4si, __v4si);
__m128i __builtin_ia32_movntdqa (__v2di *);
__m128i __builtin_ia32_pcmpgtq (__v2di, __v2di);
__m128i __builtin_ia32_aesdec128 (__v2di, __v2di);
__m128i __builtin_ia32_aesdeclast128 (__v2di, __v2di );
__m128i __builtin_ia32_aesenc128 (__v2di, __v2di);
__m128i __builtin_ia32_aesenclast128 (__v2di, __v2di);
__m128i __builtin_ia32_aesimc128 (__v2di);
__m256d __builtin_ia32_addpd256 (__v4df, __v4df);
__m256 __builtin_ia32_addps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_addsubpd256 (__v4df, __v4df);
__m256 __builtin_ia32_addsubps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_andpd256 (__v4df, __v4df);
__m256 __builtin_ia32_andps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_andnpd256 (__v4df, __v4df);
__m256 __builtin_ia32_andnps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_blendvpd256 (__v4df, __v4df, __v4df );
__m256 __builtin_ia32_blendvps256 (__v8sf, __v8sf, __v8sf );
__m256d __builtin_ia32_divpd256 (__v4df, __v4df);
__m256 __builtin_ia32_divps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_haddpd256 (__v4df, __v4df);
__m256 __builtin_ia32_haddps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_hsubpd256 (__v4df, __v4df);
__m256 __builtin_ia32_hsubps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_maxpd256 (__v4df, __v4df);
__m256 __builtin_ia32_maxps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_minpd256 (__v4df, __v4df);
__m256 __builtin_ia32_minps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_mulpd256 (__v4df, __v4df);
__m256 __builtin_ia32_mulps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_orpd256 (__v4df, __v4df);
__m256 __builtin_ia32_orps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_subpd256 (__v4df, __v4df);
__m256 __builtin_ia32_subps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_xorpd256 (__v4df, __v4df);
__m256 __builtin_ia32_xorps256 (__v8sf, __v8sf);
__m256d __builtin_ia32_cvtdq2pd256 (__v4si);
__m256 __builtin_ia32_cvtdq2ps256 (__v8si);
__m128 __builtin_ia32_cvtpd2ps256 (__v4df);
__m256i __builtin_ia32_cvtps2dq256 (__v8sf);
__m256d __builtin_ia32_cvtps2pd256 (__v4sf);
__m128i __builtin_ia32_cvttpd2dq256 (__v4df);
__m128i __builtin_ia32_cvtpd2dq256 (__v4df);
__m256i __builtin_ia32_cvttps2dq256 (__v8sf);
void __builtin_ia32_vzeroall ();
void __builtin_ia32_vzeroupper ();
__m128d __builtin_ia32_vpermilvarpd (__v2df, __v2di );
__m256d __builtin_ia32_vpermilvarpd256 (__v4df, __v4di );
__m128 __builtin_ia32_vpermilvarps (__v4sf, __v4si );
__m256 __builtin_ia32_vpermilvarps256 (__v8sf, __v8si );
__m128 __builtin_ia32_vbroadcastss (float const*);
__m256d __builtin_ia32_vbroadcastsd256 (double const*);
__m256 __builtin_ia32_vbroadcastss256 (float const*);
// __m256d __builtin_ia32_vbroadcastf128_pd256 (__X);
__m256d __builtin_ia32_vbroadcastf128_pd256 (__m128d const *__X);
// __m256 __builtin_ia32_vbroadcastf128_ps256 (__X);
__m256 __builtin_ia32_vbroadcastf128_ps256 (__m128 const *__X);

__m256d __builtin_ia32_loadupd256 (double const *__P);
void __builtin_ia32_storeupd256 (double *__P, __v4df __A);
__m256 __builtin_ia32_loadups256 (float const *__P);
void __builtin_ia32_storeups256 (float *__P, __v8sf __A);
__m256i __builtin_ia32_loaddqu256 (char const * __P);
void __builtin_ia32_storedqu256 (char *__P, __v32qi __A);
__m128d __builtin_ia32_maskloadpd (const __v2df *__P, __v2di __M);
void __builtin_ia32_maskstorepd (__v2df *__P, __v2di __M, __v2df __A);
__m256d __builtin_ia32_maskloadpd256 (const __v4df *__P, __v4di __M);
void __builtin_ia32_maskstorepd256 (__v4df *__P, __v4di __M, __v4df __A);
__m128 __builtin_ia32_maskloadps (const __v4sf *__P, __v4si __M);
void __builtin_ia32_maskstoreps (__v4sf * __P, __v4si __M, __v4sf __A);
__m256 __builtin_ia32_maskloadps256 (const __v8sf *__P, __v8si __M);
void __builtin_ia32_maskstoreps256 (__v8sf *__P, __v8si __M, __v8sf __A);
__m256 __builtin_ia32_movshdup256 (__v8sf __X);
__m256 __builtin_ia32_movsldup256 (__v8sf __X);
__m256d __builtin_ia32_movddup256 (__v4df __X);
__m256i __builtin_ia32_lddqu256 (char const *__P);
void __builtin_ia32_movntdq256 (__v4di * __A, __v4di __B);
void __builtin_ia32_movntpd256 (double *__A, __v4df __B);
void __builtin_ia32_movntps256 (float *__P, __v8sf __A);
__m256 __builtin_ia32_rcpps256 (__v8sf __A);
__m256 __builtin_ia32_rsqrtps256 (__v8sf __A);
__m256d __builtin_ia32_sqrtpd256 (__v4df __A);
__m256 __builtin_ia32_sqrtps256 (__v8sf __A);

__m256d __builtin_ia32_unpckhpd256 (__v4df, __v4df);
__m256d __builtin_ia32_unpcklpd256 (__v4df, __v4df);
__m256 __builtin_ia32_unpckhps256 (__v8sf, __v8sf);
__m256 __builtin_ia32_unpcklps256 (__v8sf, __v8sf);
int __builtin_ia32_vtestzpd (__v2df, __v2df);
int __builtin_ia32_vtestcpd (__v2df, __v2df);
int __builtin_ia32_vtestnzcpd (__v2df, __v2df);
int __builtin_ia32_vtestzps (__v4sf, __v4sf);
int __builtin_ia32_vtestcps (__v4sf, __v4sf);

int __builtin_ia32_vtestnzcps (__v4sf, __v4sf);
int __builtin_ia32_vtestzpd256 (__v4df, __v4df);
int __builtin_ia32_vtestcpd256 (__v4df, __v4df);
int __builtin_ia32_vtestnzcpd256 (__v4df, __v4df);
int __builtin_ia32_vtestzps256 (__v8sf, __v8sf);
int __builtin_ia32_vtestcps256 (__v8sf, __v8sf);
int __builtin_ia32_vtestnzcps256 (__v8sf, __v8sf);
int __builtin_ia32_ptestz256 (__v4di, __v4di);
int __builtin_ia32_ptestc256 (__v4di, __v4di);
int __builtin_ia32_ptestnzc256 (__v4di, __v4di);
int __builtin_ia32_movmskpd256 (__v4df);
int __builtin_ia32_movmskps256 (__v8sf);
__m128d __builtin_ia32_pd_pd256 (__v4df);
__m128 __builtin_ia32_ps_ps256 (__v8sf);
__m128i __builtin_ia32_si_si256 (__v8si);
__m256d __builtin_ia32_pd256_pd (__v2df);
__m256 __builtin_ia32_ps256_ps (__v4sf);
__m256i __builtin_ia32_si256_si (__v4si);
__m256i __builtin_ia32_pabsb256 (__v32qi);
__m256i __builtin_ia32_pabsw256 (__v16hi);
__m256i __builtin_ia32_pabsd256 (__v8si);
__m256i __builtin_ia32_packssdw256 (__v8si, __v8si);
__m256i __builtin_ia32_packsswb256 (__v16hi, __v16hi);
__m256i __builtin_ia32_packusdw256 (__v8si, __v8si);
__m256i __builtin_ia32_packuswb256 (__v16hi, __v16hi);
__m256i __builtin_ia32_paddb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_paddw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_paddd256 (__v8si, __v8si);
__m256i __builtin_ia32_paddq256 (__v4di, __v4di);
__m256i __builtin_ia32_paddsb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_paddsw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_paddusb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_paddusw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_andsi256 (__v4di, __v4di);
__m256i __builtin_ia32_andnotsi256 (__v4di, __v4di);
__m256i __builtin_ia32_pavgb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pavgw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pblendvb256 (__v32qi, __v32qi, __v32qi );
__m256i __builtin_ia32_pcmpeqb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pcmpeqw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pcmpeqd256 (__v8si, __v8si);
__m256i __builtin_ia32_pcmpeqq256 (__v4di, __v4di);
__m256i __builtin_ia32_pcmpgtb256 (__v32qi, __v32qi );
__m256i __builtin_ia32_pcmpgtw256 (__v16hi, __v16hi );
__m256i __builtin_ia32_pcmpgtd256 (__v8si, __v8si );
__m256i __builtin_ia32_pcmpgtq256 (__v4di, __v4di);
__m256i __builtin_ia32_phaddw256 (__v16hi, __v16hi );
__m256i __builtin_ia32_phaddd256 (__v8si, __v8si);
__m256i __builtin_ia32_phaddsw256 (__v16hi, __v16hi );
__m256i __builtin_ia32_phsubw256 (__v16hi, __v16hi );
__m256i __builtin_ia32_phsubd256 (__v8si, __v8si);
__m256i __builtin_ia32_phsubsw256 (__v16hi, __v16hi );
__m256i __builtin_ia32_pmaddubsw256 (__v32qi, __v32qi );
__m256i __builtin_ia32_pmaddwd256 (__v16hi, __v16hi );
__m256i __builtin_ia32_pmaxsb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pmaxsw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pmaxsd256 (__v8si, __v8si);
__m256i __builtin_ia32_pmaxub256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pmaxuw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pmaxud256 (__v8si, __v8si);
__m256i __builtin_ia32_pminsb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pminsw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pminsd256 (__v8si, __v8si);
__m256i __builtin_ia32_pminub256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pminuw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pminud256 (__v8si, __v8si);
int     __builtin_ia32_pmovmskb256 (__v32qi);
__m256i __builtin_ia32_pmovsxbw256 (__v16qi);
__m256i __builtin_ia32_pmovsxbd256 (__v16qi);
__m256i __builtin_ia32_pmovsxbq256 (__v16qi);
__m256i __builtin_ia32_pmovsxwd256 (__v8hi);
__m256i __builtin_ia32_pmovsxwq256 (__v8hi);
__m256i __builtin_ia32_pmovsxdq256 (__v4si);
__m256i __builtin_ia32_pmovzxbw256 (__v16qi);
__m256i __builtin_ia32_pmovzxbd256 (__v16qi);
__m256i __builtin_ia32_pmovzxbq256 (__v16qi);
__m256i __builtin_ia32_pmovzxwd256 (__v8hi);
__m256i __builtin_ia32_pmovzxwq256 (__v8hi);
__m256i __builtin_ia32_pmovzxdq256 (__v4si);
__m256i __builtin_ia32_pmuldq256 (__v8si, __v8si);
__m256i __builtin_ia32_pmulhrsw256 (__v16hi, __v16hi );
__m256i __builtin_ia32_pmulhuw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pmulhw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pmullw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_pmulld256 (__v8si, __v8si);
__m256i __builtin_ia32_pmuludq256 (__v8si, __v8si);
__m256i __builtin_ia32_por256 (__v4di, __v4di);
__m256i __builtin_ia32_psadbw256 (__v32qi, __v32qi);
__m256i __builtin_ia32_pshufb256 (__v32qi, __v32qi );
__m256i __builtin_ia32_psignb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_psignw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_psignd256 (__v8si, __v8si);
__m256i __builtin_ia32_psllwi256 (__v16hi, int);
__m256i __builtin_ia32_psllw256(__v16hi, __v8hi);
__m256i __builtin_ia32_pslldi256 (__v8si, int);
__m256i __builtin_ia32_pslld256(__v8si, __v4si);
__m256i __builtin_ia32_psllqi256 (__v4di, int);
__m256i __builtin_ia32_psllq256(__v4di, __v2di);
__m256i __builtin_ia32_psrawi256 (__v16hi, int);
__m256i __builtin_ia32_psraw256 (__v16hi, __v8hi);
__m256i __builtin_ia32_psradi256 (__v8si, int);
__m256i __builtin_ia32_psrad256 (__v8si, __v4si);
__m256i __builtin_ia32_psrlwi256 (__v16hi, int);
__m256i __builtin_ia32_psrlw256(__v16hi, __v8hi);
__m256i __builtin_ia32_psrldi256 (__v8si, int);
__m256i __builtin_ia32_psrld256(__v8si, __v4si);
__m256i __builtin_ia32_psrlqi256 (__v4di, int);
__m256i __builtin_ia32_psrlq256(__v4di, __v2di);
__m256i __builtin_ia32_psubb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_psubw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_psubd256 (__v8si, __v8si);
__m256i __builtin_ia32_psubq256 (__v4di, __v4di);
__m256i __builtin_ia32_psubsb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_psubsw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_psubusb256 (__v32qi, __v32qi);
__m256i __builtin_ia32_psubusw256 (__v16hi, __v16hi);
__m256i __builtin_ia32_punpckhbw256 (__v32qi, __v32qi);
__m256i __builtin_ia32_punpckhwd256 (__v16hi, __v16hi);
__m256i __builtin_ia32_punpckhdq256 (__v8si, __v8si);
__m256i __builtin_ia32_punpckhqdq256 (__v4di, __v4di);
__m256i __builtin_ia32_punpcklbw256 (__v32qi, __v32qi);
__m256i __builtin_ia32_punpcklwd256 (__v16hi, __v16hi);
__m256i __builtin_ia32_punpckldq256 (__v8si, __v8si);
__m256i __builtin_ia32_punpcklqdq256 (__v4di, __v4di);
__m256i __builtin_ia32_pxor256 (__v4di, __v4di);
__m256i __builtin_ia32_movntdqa256 (__v4di *);
__m128 __builtin_ia32_vbroadcastss_ps (__v4sf);
__m256 __builtin_ia32_vbroadcastss_ps256 (__v4sf);
__m256d __builtin_ia32_vbroadcastsd_pd256 (__v2df);
__m256i __builtin_ia32_vbroadcastsi256 (__v2di);
__m256i __builtin_ia32_pbroadcastb256 (__v16qi);
__m256i __builtin_ia32_pbroadcastw256 (__v8hi);
__m256i __builtin_ia32_pbroadcastd256 (__v4si);
__m256i __builtin_ia32_pbroadcastq256 (__v2di);
__m128i __builtin_ia32_pbroadcastb128 (__v16qi);
__m128i __builtin_ia32_pbroadcastw128 (__v8hi);
__m128i __builtin_ia32_pbroadcastd128 (__v4si);
__m128i __builtin_ia32_pbroadcastq128 (__v2di);
__m256i __builtin_ia32_permvarsi256 (__v8si, __v8si);
__m256 __builtin_ia32_permvarsf256 (__v8sf, __v8si);
__m256i __builtin_ia32_maskloadd256 (const __v8si *, __v8si );
__m256i __builtin_ia32_maskloadq256 (const __v4di *, __v4di );
__m128i __builtin_ia32_maskloadd (const __v4si *, __v4si );
__m128i __builtin_ia32_maskloadq (const __v2di *, __v2di );
void __builtin_ia32_maskstored256 (__v8si *, __v8si, __v8si);
void __builtin_ia32_maskstoreq256 (__v4di *, __v4di, __v4di);
void __builtin_ia32_maskstored (__v4si *, __v4si, __v4si);
void __builtin_ia32_maskstoreq (__v2di *, __v2di, __v2di);
__m256i __builtin_ia32_psllv8si (__v8si, __v8si);
__m128i __builtin_ia32_psllv4si (__v4si, __v4si);
__m256i __builtin_ia32_psllv4di (__v4di, __v4di);
__m128i __builtin_ia32_psllv2di (__v2di, __v2di);
__m256i __builtin_ia32_psrav8si (__v8si, __v8si);
__m128i __builtin_ia32_psrav4si (__v4si, __v4si);
__m256i __builtin_ia32_psrlv8si (__v8si, __v8si);
__m128i __builtin_ia32_psrlv4si (__v4si, __v4si);
__m256i __builtin_ia32_psrlv4di (__v4di, __v4di);
__m128i __builtin_ia32_psrlv2di (__v2di, __v2di);
// __m512d __builtin_ia32_broadcastsd512 ( int );
__m512d __builtin_ia32_broadcastsd512 (__v2df, __v8df, __mmask8);
__m512 __builtin_ia32_broadcastss512 ( __v4sf, __v16sf, __mmask16);
// __m512d __builtin_ia32_movapd512_mask (__v8df, int );
__m512d __builtin_ia32_movapd512_mask (__v8df, __v8df, __mmask8);
// __m512 __builtin_ia32_movaps512_mask (__v16sf, int );
   __m512 __builtin_ia32_movaps512_mask (__v16sf, __v16sf , __mmask16);
// __m512d __builtin_ia32_loadapd512_mask (const __v8df *, int );
__m512d __builtin_ia32_loadapd512_mask (const __v8df *, __v8df, __mmask8);
void __builtin_ia32_storeapd512_mask (__v8df *, __v8df, int );
// __m512 __builtin_ia32_loadaps512_mask (const __v16sf *, int );
__m512 __builtin_ia32_loadaps512_mask (const __v16sf *, __v16sf, __mmask16);
void __builtin_ia32_storeaps512_mask (__v16sf *, __v16sf, int );
// __m512i __builtin_ia32_movdqa64_512_mask (__v8di, int );
__m512i __builtin_ia32_movdqa64_512_mask (__v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_movdqa64load512_mask (const __v8di *, int );
__m512i __builtin_ia32_movdqa64load512_mask (const __v8di *, __v8di, __mmask8);
void __builtin_ia32_movdqa64store512_mask (__v8di *, __v8di, int );
// __m512i __builtin_ia32_movdqa32_512_mask (__v16si, int );
__m512i __builtin_ia32_movdqa32_512_mask (__v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_movdqa32load512_mask (const __v16si *, int );
__m512i __builtin_ia32_movdqa32load512_mask (const __v16si *, __v16si, __mmask16);
void __builtin_ia32_movdqa32store512_mask (__v16si *, __v16si, __mmask16);
// __m512i __builtin_ia32_pmulld512_mask (__v16si, int );
// __m512i __builtin_ia32_pmulld512_mask (__v16si, int );

// DQ (1/25/2017): Unclear which if these to use for C langauge support (can't be overloaded in C).
#ifdef __cplusplus
__m512i __builtin_ia32_pmulld512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_psllv16si_mask (__v16si, __v16si, __mmask16 );
__m512i __builtin_ia32_psrav16si_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_psrlv16si_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_paddq512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psubq512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psllv8di_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psrav8di_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psrlv8di_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_paddd512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_psubd512_mask (__v16si, __v16si, __mmask8);
__m512d __builtin_ia32_rsqrt14pd512_mask (__v8df, __v8df, __mmask8);
__m512  __builtin_ia32_rsqrt14ps512_mask (__v16sf, __v16sf, __mmask16);
// __m512  __builtin_ia32_broadcastss512 (__v4sf, __v4sf, __mmask8);
__m512  __builtin_ia32_broadcastss512 (__v4sf, __v4sf, __v4sf, __mmask8);
__m512i __builtin_ia32_pbroadcastd512 (__v4si, int );
__m512i __builtin_ia32_pbroadcastd512_gpr_mask (int, int );
#endif


// DQ (7/31/2018): Added support for another builtin function (support for Cxx11_tests/test2018_136.C).
__m128i __builtin_ia32_pshufd (__v4si, int);


// __m512i __builtin_ia32_pmulld512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_pmulld512_mask (__v16si, __v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_psllv16si_mask (__v16si, __v16si, __mmask16 );
__m512i __builtin_ia32_psllv16si_mask (__v16si, __v16si, __v16si, __mmask16 );
// __m512i __builtin_ia32_psllv16si_mask (__v16si, __v16si, __mmask16 );
// __m512i __builtin_ia32_psrav16si_mask (__v16si, int );
__m512i __builtin_ia32_psrav16si_mask (__v16si, __v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_psrav16si_mask (__v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_psrlv16si_mask (__v16si, int );
__m512i __builtin_ia32_psrlv16si_mask (__v16si, __v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_psrlv16si_mask (__v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_paddq512_mask (__v8di, int );
__m512i __builtin_ia32_paddq512_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_paddq512_mask (__v8di, __v8di, __mmask8);

// __m512i __builtin_ia32_psubq512_mask (__v8di, int );
// __m512i __builtin_ia32_psubq512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psubq512_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_psllv8di_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psllv8di_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_psllv8di_mask (__v8di, int );
// __m512i __builtin_ia32_psrav8di_mask (__v8di, int );
// __m512i __builtin_ia32_psrav8di_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psrav8di_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_psrlv8di_mask (__v8di, int );
// __m512i __builtin_ia32_psrlv8di_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psrlv8di_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_paddd512_mask (__v16si, int );
__m512i __builtin_ia32_paddd512_mask (__v16si, __v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_paddd512_mask (__v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_pmuldq512_mask (__v16si, int );
// __m512i __builtin_ia32_pmuldq512_mask (__v16si, __v16si, __mmask8);
__m512i __builtin_ia32_pmuldq512_mask (__v16si, __v16si, __v8di, __mmask8);
// __m512i __builtin_ia32_psubd512_mask (__v16si, int );
// __m512i __builtin_ia32_psubd512_mask (__v16si, __v16si, __mmask8);
__m512i __builtin_ia32_psubd512_mask (__v16si, __v16si, __v16si, __mmask8);
// __m512i __builtin_ia32_pmuludq512_mask (__v16si, int );
// __m512i __builtin_ia32_pmuludq512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_pmuludq512_mask (__v16si, __v16si, __v8di, __mmask16);
// __m512i __builtin_ia32_psllq512_mask (__v8di, int );
// __m512i __builtin_ia32_psllq512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psllq512_mask (__v8di, __v2di, __v8di, __mmask8 );
// __m512i __builtin_ia32_psrlq512_mask (__v8di, int );
// __m512i __builtin_ia32_psrlq512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psrlq512_mask (__v8di, __v2di, __v8di, __mmask8);
// __m512i __builtin_ia32_psraq512_mask (__v8di, int );
// __m512i __builtin_ia32_psraq512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_psraq512_mask (__v8di, __v2di, __v8di, __mmask8);
// __m512i __builtin_ia32_pslld512_mask (__v16si, int );
// __m512i __builtin_ia32_pslld512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_pslld512_mask (__v16si, __v4si, __v16si, __mmask16);
// __m512i __builtin_ia32_psrld512_mask (__v16si, int );
// __m512i __builtin_ia32_psrld512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_psrld512_mask (__v16si, __v4si, __v16si, __mmask16);
// __m512i __builtin_ia32_psrad512_mask (__v16si, int );
// __m512i __builtin_ia32_psrad512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_psrad512_mask (__v16si, __v4si, __v16si, __mmask16);
// __m512d __builtin_ia32_rcp14pd512_mask (__v8df, int );
__m512d __builtin_ia32_rcp14pd512_mask (__v8df, __v8df, __mmask8);
// __m512d __builtin_ia32_rcp14pd512_mask (__v8df, __v8df, __v8df, __mmask8);
// __m512 __builtin_ia32_rcp14ps512_mask (__v16sf, int );
// __m512 __builtin_ia32_rcp14ps512_mask (__v16sf, __v16sf, __mmask16);
__m512 __builtin_ia32_rcp14ps512_mask (__v16sf, __v16sf, __mmask16);
__m128d __builtin_ia32_rcp14sd (__v2df, __v2df );
__m128 __builtin_ia32_rcp14ss (__v4sf, __v4sf );
// __m512d __builtin_ia32_rsqrt14pd512_mask (__v8df, int );
// __m512d __builtin_ia32_rsqrt14pd512_mask (__v8df, __v8df, __mmask8);
__m512d __builtin_ia32_rsqrt14pd512_mask (__v8df, __v8df, __v8df, __mmask8);
// __m512 __builtin_ia32_rsqrt14ps512_mask (__v16sf, int );
// __m512 __builtin_ia32_rsqrt14ps512_mask (__v16sf, __v16sf, __mmask16);
__m512 __builtin_ia32_rsqrt14ps512_mask (__v16sf, __v16sf, __v16sf, __mmask16);
__m128d __builtin_ia32_rsqrt14sd (__v2df, __v2df );
__m128 __builtin_ia32_rsqrt14ss (__v4sf, __v4sf );
// __m512i __builtin_ia32_pmovsxbd512_mask (__v16qi, int );
__m512i __builtin_ia32_pmovsxbd512_mask (__v16qi, __v16si, __mmask16);
// __m512i __builtin_ia32_pmovsxbd512_mask (__v16qi, __v16qi, __v16qi, __mmask16);
// __m512i __builtin_ia32_pmovsxbq512_mask (__v16qi, int );
__m512i __builtin_ia32_pmovsxbq512_mask (__v16qi, __v8di, __mmask16);
// __m512i __builtin_ia32_pmovsxbq512_mask (__v16qi, __v16qi, __v16qi, __mmask16);
// __m512i __builtin_ia32_pmovsxwd512_mask (__v16hi, int );
__m512i __builtin_ia32_pmovsxwd512_mask (__v16hi, __v16si, __mmask16);
// __m512i __builtin_ia32_pmovsxwd512_mask (__v16hi, __v16hi, __v16hi, __mmask16);
// __m512i __builtin_ia32_pmovsxwq512_mask (__v8hi, int );
__m512i __builtin_ia32_pmovsxwq512_mask (__v8hi, __v8di, __mmask8);
// __m512i __builtin_ia32_pmovsxwq512_mask (__v8hi, __v8hi, __v8hi, __mmask8);
// __m512i __builtin_ia32_pmovsxdq512_mask (__v8si, int );
__m512i __builtin_ia32_pmovsxdq512_mask (__v8si, __v8di, __mmask8);
// __m512i __builtin_ia32_pmovsxdq512_mask (__v8si, __v8si, __v8si, __mmask8);
// __m512i __builtin_ia32_pmovzxbd512_mask (__v16qi, int );
__m512i __builtin_ia32_pmovzxbd512_mask (__v16qi, __v16si, __mmask16);
// __m512i __builtin_ia32_pmovzxbd512_mask (__v16qi, __v16qi, __v16qi, __mmask16);
// __m512i __builtin_ia32_pmovzxbq512_mask (__v16qi, int );
__m512i __builtin_ia32_pmovzxbq512_mask (__v16qi, __v8di, __mmask16);
// __m512i __builtin_ia32_pmovzxbq512_mask (__v16qi, __v16qi, __v16qi, __mmask16);
// __m512i __builtin_ia32_pmovzxwd512_mask (__v16hi, int );
__m512i __builtin_ia32_pmovzxwd512_mask (__v16hi, __v16si, __mmask16);
// __m512i __builtin_ia32_pmovzxwd512_mask (__v16hi, __v16hi, __v16hi, __mmask16);
// __m512i __builtin_ia32_pmovzxwq512_mask (__v8hi, int );
__m512i __builtin_ia32_pmovzxwq512_mask (__v8hi, __v8di, __mmask8);
// __m512i __builtin_ia32_pmovzxwq512_mask (__v8hi, __v8hi, __v8hi, __mmask8);
// __m512i __builtin_ia32_pmovzxdq512_mask (__v8si, int );
__m512i __builtin_ia32_pmovzxdq512_mask (__v8si, __v8di, __mmask8);
// __m512i __builtin_ia32_pmovzxdq512_mask (__v8si, __v8si, __v8si, __mmask8);
// __m512i __builtin_ia32_pabsq512_mask (__v8di, int );
__m512i __builtin_ia32_pabsq512_mask (__v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_pabsq512_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_pabsd512_mask (__v16si, int );
__m512i __builtin_ia32_pabsd512_mask (__v16si, __v16si, __mmask16);
// __m512i __builtin_ia32_pabsd512_mask (__v16si, __v16si, __v16si, __mmask16);
// __m512 __builtin_ia32_broadcastss512 (__v4sf, int );
// __m512 __builtin_ia32_broadcastss512 (__v4sf, __v4sf, __mmask8);
// __m512 __builtin_ia32_broadcastss512 (__v4sf, __v4sf, __v4sf, __mmask8);
// __m512d __builtin_ia32_broadcastsd512 (__v2df, int );
// __m512d __builtin_ia32_broadcastsd512 (__v2df, int );
// __m512d __builtin_ia32_broadcastsd512 (__v2df, int );

// __m512i __builtin_ia32_pbroadcastd512 (__v4si, int );
__m512i __builtin_ia32_pbroadcastd512 (__v4si, __v16si, __mmask16);
// __m512i __builtin_ia32_pbroadcastd512 (__v4si, int );
// __m512i __builtin_ia32_pbroadcastd512_gpr_mask (int, int );
__m512i __builtin_ia32_pbroadcastd512_gpr_mask (int, __v16si, int );
// void __builtin_ia32_pbroadcastd512_gpr_mask (int, int );
__m512i __builtin_ia32_pbroadcastq512 (__v2di, __v8di, __mmask8);
// __m512i __builtin_ia32_pbroadcastq512 (__v2di, int );
// __m512i __builtin_ia32_pbroadcastq512 (__v2di, int );
// __m512i __builtin_ia32_pbroadcastq512_mem_mask (long long, int );
__m512i __builtin_ia32_pbroadcastq512_mem_mask (long long, __v8di, __mmask8);
// void __builtin_ia32_pbroadcastq512_mem_mask (long long, int );

// __m512 __builtin_ia32_broadcastf32x4_512 (__v4sf, int );
// __m512 __builtin_ia32_broadcastf32x4_512 (__v4sf, int );
__m512 __builtin_ia32_broadcastf32x4_512 (__v4sf, __v16sf, __mmask16);

__m512i __builtin_ia32_broadcasti32x4_512 (__v4si, __v16si, __mmask16);
// __m512i __builtin_ia32_broadcasti32x4_512 (__v4si, int );
// __m512i __builtin_ia32_broadcasti32x4_512 (__v4si, int );

// __m512d __builtin_ia32_broadcastf64x4_512 (__v4df, int );
// __m512d __builtin_ia32_broadcastf64x4_512 (__v4df, int );
__m512d __builtin_ia32_broadcastf64x4_512 (__v4df, __v8df, __mmask8);

// __m512i __builtin_ia32_broadcasti64x4_512 (__v4di, int );
// __m512i __builtin_ia32_broadcasti64x4_512 (__v4di, int );
__m512i __builtin_ia32_broadcasti64x4_512 (__v4di, __v8di, __mmask8);

// __m512i __builtin_ia32_prolvd512_mask (__v16si, int );
// __m512i __builtin_ia32_prolvd512_mask (__v16si, int );
__m512i __builtin_ia32_prolvd512_mask (__v16si, __v16si, __v16si, __mmask16);

// __m512i __builtin_ia32_prorvd512_mask (__v16si, int );
// __m512i __builtin_ia32_prorvd512_mask (__v16si, int );
__m512i __builtin_ia32_prorvd512_mask (__v16si, __v16si, __v16si, __mmask16);

// __m512i __builtin_ia32_prolvq512_mask (__v8di, int );
// __m512i __builtin_ia32_prolvq512_mask (__v8di, int );
__m512i __builtin_ia32_prolvq512_mask (__v8di, __v8di, __v8di, __mmask8);

// __m512i __builtin_ia32_prorvq512_mask (__v8di, int );
// __m512i __builtin_ia32_prorvq512_mask (__v8di, int );
__m512i __builtin_ia32_prorvq512_mask (__v8di, __v8di, __v8di, __mmask8);

__m128d __builtin_ia32_cvtusi2sd32 (__v2df, unsigned);
void __builtin_ia32_pmovdb512mem_mask (__v16qi *, __v16si, __mmask16);
// __m128i __builtin_ia32_pmovdb512_mask (__v16si, int );
// __m128i __builtin_ia32_pmovdb512_mask (__v16si, int );
__m128i __builtin_ia32_pmovdb512_mask (__v16si, __v16qi, __mmask16);

void __builtin_ia32_pmovsdb512mem_mask (__v16qi *, __v16si, __mmask16);

// __m128i __builtin_ia32_pmovsdb512_mask (__v16si, int );
// __m128i __builtin_ia32_pmovsdb512_mask (__v16si, int );
__m128i __builtin_ia32_pmovsdb512_mask (__v16si, __v16qi, __mmask16);
void __builtin_ia32_pmovusdb512mem_mask (__v16qi *, __v16si, __mmask16);

// __m128i __builtin_ia32_pmovusdb512_mask (__v16si, int );
// __m128i __builtin_ia32_pmovusdb512_mask (__v16si, int );
__m128i __builtin_ia32_pmovusdb512_mask (__v16si, __v16qi, __mmask16);

void __builtin_ia32_pmovdw512mem_mask (__v16hi *, __v16si, __mmask16);
// __m256i __builtin_ia32_pmovdw512_mask (__v16si, int );
// __m256i __builtin_ia32_pmovdw512_mask (__v16si, int );
__m256i __builtin_ia32_pmovdw512_mask (__v16si, __v16hi, __mmask16);

void __builtin_ia32_pmovsdw512mem_mask (__v16hi*, __v16si, __mmask16);
// __m256i __builtin_ia32_pmovsdw512_mask (__v16si, int );
// __m256i __builtin_ia32_pmovsdw512_mask (__v16si, int );
__m256i __builtin_ia32_pmovsdw512_mask (__v16si, __v16hi, __mmask16);

void __builtin_ia32_pmovusdw512mem_mask (__v16hi*, __v16si, __mmask16);
// __m256i __builtin_ia32_pmovusdw512_mask (__v16si, int );
// __m256i __builtin_ia32_pmovusdw512_mask (__v16si, int );
__m256i __builtin_ia32_pmovusdw512_mask (__v16si, __v16hi, __mmask16);

void __builtin_ia32_pmovqd512mem_mask (__v8si *, __v8di, __mmask16);
// __m256i __builtin_ia32_pmovqd512_mask (__v8di, int );
// __m256i __builtin_ia32_pmovqd512_mask (__v8di, int );
__m256i __builtin_ia32_pmovqd512_mask (__v8di, __v8si, __mmask8);

void __builtin_ia32_pmovsqd512mem_mask (__v8si *, __v8di, __mmask16);
// __m256i __builtin_ia32_pmovsqd512_mask (__v8di, int );
// __m256i __builtin_ia32_pmovsqd512_mask (__v8di, int );
__m256i __builtin_ia32_pmovsqd512_mask (__v8di, __v8si, __mmask8);

void __builtin_ia32_pmovusqd512mem_mask (__v8si*, __v8di, __mmask16);
// __m256i __builtin_ia32_pmovusqd512_mask (__v8di, int );
// __m256i __builtin_ia32_pmovusqd512_mask (__v8di, int );
__m256i __builtin_ia32_pmovusqd512_mask (__v8di, __v8si, __mmask8);

void __builtin_ia32_pmovqw512mem_mask (__v8hi *, __v8di, __mmask16);
// __m128i __builtin_ia32_pmovqw512_mask (__v8di, int );
// __m128i __builtin_ia32_pmovqw512_mask (__v8di, int );
__m128i __builtin_ia32_pmovqw512_mask (__v8di, __v8hi, __mmask8);

void __builtin_ia32_pmovsqw512mem_mask (__v8hi *, __v8di, __mmask16);
// __m128i __builtin_ia32_pmovsqw512_mask (__v8di, int );
// __m128i __builtin_ia32_pmovsqw512_mask (__v8di, int );
__m128i __builtin_ia32_pmovsqw512_mask (__v8di, __v8hi, __mmask8);

void __builtin_ia32_pmovusqw512mem_mask (__v8hi*, __v8di, __mmask16);
// __m128i __builtin_ia32_pmovusqw512_mask (__v8di, int );
// __m128i __builtin_ia32_pmovusqw512_mask (__v8di, int );
__m128i __builtin_ia32_pmovusqw512_mask (__v8di, __v8hi, __mmask8);

void __builtin_ia32_pmovqb512mem_mask (__v16qi *, __v8di, __mmask16);
// __m128i __builtin_ia32_pmovqb512_mask (__v8di, int );
// __m128i __builtin_ia32_pmovqb512_mask (__v8di, int );
__m128i __builtin_ia32_pmovqb512_mask (__v8di, __v16qi, __mmask8);

void __builtin_ia32_pmovsqb512mem_mask (__v16qi *, __v8di, __mmask16);
// __m128i __builtin_ia32_pmovsqb512_mask (__v8di, int );
// __m128i __builtin_ia32_pmovsqb512_mask (__v8di, int );
__m128i __builtin_ia32_pmovsqb512_mask (__v8di, __v16qi, __mmask8);

void __builtin_ia32_pmovusqb512mem_mask (__v16qi *, __v8di, __mmask16);
// __m128i __builtin_ia32_pmovusqb512_mask (__v8di, int );
// __m128i __builtin_ia32_pmovusqb512_mask (__v8di, int );
__m128i __builtin_ia32_pmovusqb512_mask (__v8di, __v16qi, __mmask8);

// __m512d __builtin_ia32_cvtdq2pd512_mask (__v8si, int );
// __m512d __builtin_ia32_cvtdq2pd512_mask (__v8si, int );
__m512d __builtin_ia32_cvtdq2pd512_mask (__v8si, __v8df, __mmask8);

// __m512d __builtin_ia32_cvtudq2pd512_mask (__v8si, int );
// __m512d __builtin_ia32_cvtudq2pd512_mask (__v8si, int );
__m512d __builtin_ia32_cvtudq2pd512_mask (__v8si, __v8df, __mmask8);

// __m512d __builtin_ia32_loadupd512_mask (const __v8df *, int );
// __m512d __builtin_ia32_loadupd512_mask (const __v8df *, int );
__m512d __builtin_ia32_loadupd512_mask (const __v8df *, __v8df, __mmask8);

void __builtin_ia32_storeupd512_mask (__v8df *, __v8df, int );
void __builtin_ia32_storeupd512_mask (__v8df *, __v8df, int );

// __m512 __builtin_ia32_loadups512_mask (const __v16sf *, int );
// __m512 __builtin_ia32_loadups512_mask (const __v16sf *, int );
__m512 __builtin_ia32_loadups512_mask (const __v16sf *, __v16sf, __mmask16);

void __builtin_ia32_storeups512_mask (__v16sf *, __v16sf, int );
void __builtin_ia32_storeups512_mask (__v16sf *, __v16sf, int );

// __m512i __builtin_ia32_loaddqudi512_mask (const __v8di *, int );
__m512i __builtin_ia32_loaddqudi512_mask (const __v8di *, __v8di, __mmask8);

void __builtin_ia32_storedqudi512_mask (__v8di *, __v8di, int );

// __m512i __builtin_ia32_loaddqusi512_mask (const __v16si *, int );
// __m512i __builtin_ia32_loaddqusi512_mask (const __v16si *, int );
__m512i __builtin_ia32_loaddqusi512_mask (const __v16si *, __v16si, __mmask16);

void __builtin_ia32_storedqusi512_mask (__v16si *, __v16si, int );
void __builtin_ia32_storedqusi512_mask (__v16si *, __v16si, int );

// __m512d __builtin_ia32_vpermilvarpd512_mask (__v8df, int );
// __m512d __builtin_ia32_vpermilvarpd512_mask (__v8df, int );
__m512d __builtin_ia32_vpermilvarpd512_mask (__v8df, __v8di, __v8df, __mmask8);

// __m512 __builtin_ia32_vpermilvarps512_mask (__v16sf, int );
// __m512 __builtin_ia32_vpermilvarps512_mask (__v16sf, int );
__m512 __builtin_ia32_vpermilvarps512_mask (__v16sf, __v16si, __v16sf, __mmask16);

// __m512i __builtin_ia32_vpermt2varq512_mask (__v8di, int );
__m512i __builtin_ia32_vpermt2varq512_mask (__v8di, __v8di,__v8di, __mmask8);

__m512i __builtin_ia32_vpermi2varq512_mask (__v8di, __v8di,__v8di, __mmask8);

// __m512i __builtin_ia32_vpermt2vard512_mask (__v16si, int );
__m512i __builtin_ia32_vpermt2varq512_maskz (__v8di, __v8di,__v8di, __mmask8);

__m512i __builtin_ia32_vpermt2vard512_mask (__v16si, __v16si,__v16si, __mmask16);

__m512i __builtin_ia32_vpermi2vard512_mask (__v16si, __v16si,__v16si, __mmask16);

__m512i __builtin_ia32_vpermt2vard512_maskz (__v16si, __v16si,__v16si, __mmask16);

// __m512d __builtin_ia32_vpermt2varpd512_mask (__v8di, int );
__m512d __builtin_ia32_vpermt2varpd512_mask (__v8di, __v8df,__v8df, __mmask8);

__m512d __builtin_ia32_vpermi2varpd512_mask (__v8df, __v8di,__v8df, __mmask8);

__m512d __builtin_ia32_vpermt2varpd512_maskz (__v8di, __v8df,__v8df, __mmask8);

// __m512 __builtin_ia32_vpermt2varps512_mask (__v16si, int );
__m512 __builtin_ia32_vpermt2varps512_mask (__v16si, __v16sf,__v16sf, __mmask16);

__m512 __builtin_ia32_vpermi2varps512_mask (__v16sf, __v16si,__v16sf, __mmask16);

__m512 __builtin_ia32_vpermt2varps512_maskz (__v16si, __v16sf,__v16sf, __mmask16);

__m512i __builtin_ia32_permvardi512_mask (__v8di, __v8di,__v8di, __mmask8);
// __m512i __builtin_ia32_permvardi512_mask (__v8di, int );
// __m512i __builtin_ia32_permvardi512_mask (__v8di, int );

// __m512i __builtin_ia32_permvarsi512_mask (__v16si, int );
// __m512i __builtin_ia32_permvarsi512_mask (__v16si, int );
__m512i __builtin_ia32_permvarsi512_mask (__v16si, __v16si,__v16si, __mmask16);

// __m512d __builtin_ia32_permvardf512_mask (__v8df, int );
// __m512d __builtin_ia32_permvardf512_mask (__v8df, int );
__m512d __builtin_ia32_permvardf512_mask (__v8df, __v8di,__v8df, __mmask8);

// __m512 __builtin_ia32_permvarsf512_mask (__v16sf, int );
// __m512 __builtin_ia32_permvarsf512_mask (__v16sf, int );
__m512 __builtin_ia32_permvarsf512_mask (__v16sf, __v16si,__v16sf, __mmask16);

// __m512 __builtin_ia32_movshdup512_mask (__v16sf, int );
// __m512 __builtin_ia32_movshdup512_mask (__v16sf, int );
__m512 __builtin_ia32_movshdup512_mask (__v16sf, __v16sf, __mmask16);

// __m512 __builtin_ia32_movsldup512_mask (__v16sf, int );
// __m512 __builtin_ia32_movsldup512_mask (__v16sf, int );
__m512 __builtin_ia32_movsldup512_mask (__v16sf, __v16sf, __mmask16);

// __m512i __builtin_ia32_pord512_mask (__v16si, int );
// __m512i __builtin_ia32_pord512_mask (__v16si, int );
// __m512i __builtin_ia32_pord512_mask (__v16si, int );
__m512i __builtin_ia32_pord512_mask (__v16si, __v16si,__v16si, __mmask16);

__m512i __builtin_ia32_porq512_mask (__v8di, __v8di,__v8di, __mmask8);
// __m512i __builtin_ia32_porq512_mask (__v8di, int );
// __m512i __builtin_ia32_porq512_mask (__v8di, int );

// __m512i __builtin_ia32_pxord512_mask (__v16si, int );
// __m512i __builtin_ia32_pxord512_mask (__v16si, int );
// __m512i __builtin_ia32_pxord512_mask (__v16si, int );
__m512i __builtin_ia32_pxord512_mask (__v16si, __v16si,__v16si, __mmask16);

// __m512i __builtin_ia32_pxorq512_mask (__v8di, int );
// __m512i __builtin_ia32_pxorq512_mask (__v8di, int );
__m512i __builtin_ia32_pxorq512_mask (__v8di, __v8di,__v8di, __mmask8);

// __m512i __builtin_ia32_pandd512_mask (__v16si, int );
// __m512i __builtin_ia32_pandd512_mask (__v16si, int );
// __m512i __builtin_ia32_pandd512_mask (__v16si, int );
__m512i __builtin_ia32_pandd512_mask (__v16si, __v16si,__v16si, __mmask16);

// __m512i __builtin_ia32_pandq512_mask (__v8di, int );
// __m512i __builtin_ia32_pandq512_mask (__v8di, int );
__m512i __builtin_ia32_pandq512_mask (__v8di, __v8di,__v8di, __mmask16);

// __m512i __builtin_ia32_pandnd512_mask (__v16si, int );
// __m512i __builtin_ia32_pandnd512_mask (__v16si, int );
// __m512i __builtin_ia32_pandnd512_mask (__v16si, int );
__m512i __builtin_ia32_pandnd512_mask (__v16si, __v16si,__v16si, __mmask16);

// __m512i __builtin_ia32_pandnq512_mask (__v8di, int );
// __m512i __builtin_ia32_pandnq512_mask (__v8di, int );
__m512i __builtin_ia32_pandnq512_mask (__v8di, __v8di,__v8di, __mmask8);

// __mmask16 __builtin_ia32_ptestmd512 (__v16si, int );
__mmask16 __builtin_ia32_ptestmd512 (__v16si, __v16si, __mmask16);

// __mmask8 __builtin_ia32_ptestmq512 (__v8di, int );
__mmask8 __builtin_ia32_ptestmq512 (__v8di, __v8di, __mmask8);

// __mmask16 __builtin_ia32_ptestnmd512 (__v16si, int );
__mmask16 __builtin_ia32_ptestnmd512 (__v16si, __v16si, __mmask16);

// __mmask8 __builtin_ia32_ptestnmq512 (__v8di, int );
__mmask8 __builtin_ia32_ptestnmq512 (__v8di, __v8di, __mmask8);

// __m512i __builtin_ia32_punpckhdq512_mask (__v16si, int );
// __m512i __builtin_ia32_punpckhdq512_mask (__v16si, int );
__m512i __builtin_ia32_punpckhdq512_mask (__v16si, __v16si,__v16si, __mmask16);

// __m512i __builtin_ia32_punpckhqdq512_mask (__v8di, int );
// __m512i __builtin_ia32_punpckhqdq512_mask (__v8di, int );
__m512i __builtin_ia32_punpckhqdq512_mask (__v8di, __v8di,__v8di, __mmask8);

// __m512i __builtin_ia32_punpckldq512_mask (__v16si, int );
// __m512i __builtin_ia32_punpckldq512_mask (__v16si, int );
__m512i __builtin_ia32_punpckldq512_mask (__v16si, __v16si,__v16si, __mmask16);

// __m512i __builtin_ia32_punpcklqdq512_mask (__v8di, int );
// __m512i __builtin_ia32_punpcklqdq512_mask (__v8di, int );
__m512i __builtin_ia32_punpcklqdq512_mask (__v8di, __v8di,__v8di, __mmask8);

// __m512d __builtin_ia32_movddup512_mask (__v8df, int );
// __m512d __builtin_ia32_movddup512_mask (__v8df, int );
__m512d __builtin_ia32_movddup512_mask (__v8df, __v8df, __mmask8);

// __m512d __builtin_ia32_unpcklpd512_mask (__v8df, int );
// __m512d __builtin_ia32_unpcklpd512_mask (__v8df, int );
__m512d __builtin_ia32_unpcklpd512_mask (__v8df, __v8df, __v8df, __mmask8);

// __m512d __builtin_ia32_unpckhpd512_mask (__v8df, int );
// __m512d __builtin_ia32_unpckhpd512_mask (__v8df, int );
__m512d __builtin_ia32_unpckhpd512_mask (__v8df, __v8df, __v8df, __mmask8);

// __m512 __builtin_ia32_unpckhps512_mask (__v16sf, int );
// __m512 __builtin_ia32_unpckhps512_mask (__v16sf, int );
__m512 __builtin_ia32_unpckhps512_mask (__v16sf, __v16sf,__v16sf, __mmask16);

void __builtin_ia32_movntdq512 (__v8di *, __v8di);
void __builtin_ia32_movntps512 (float*, __v16sf);
void __builtin_ia32_movntpd512 (double*, __v8df);
__m512i __builtin_ia32_movntdqa512 (__v8di *);

// __m512 __builtin_ia32_rndscaleps_mask (__v16sf, int );
// __m512 __builtin_ia32_rndscaleps_mask (__v16sf, int );
// __m512 __builtin_ia32_rndscaleps_mask (__v16sf, int );
__m512 __builtin_ia32_rndscaleps_mask (__v16sf, int,__v16sf, int, int);

// __m512d __builtin_ia32_rndscalepd_mask (__v8df, int );
// __m512d __builtin_ia32_rndscalepd_mask (__v8df, int );
// __m512d __builtin_ia32_rndscalepd_mask (__v8df, int );
__m512d __builtin_ia32_rndscalepd_mask (__v8df, int, __v8df, int, int);

// __mmask16 __builtin_ia32_pcmpeqd512_mask (__v16si, int );
__mmask16 __builtin_ia32_pcmpeqd512_mask (__v16si, __v16si, __mmask16);

// __mmask8 __builtin_ia32_pcmpeqq512_mask (__v8di, int );
__mmask8 __builtin_ia32_pcmpeqq512_mask (__v8di, __v8di, __mmask8);

// __mmask16 __builtin_ia32_pcmpgtd512_mask (__v16si, int );
__mmask16 __builtin_ia32_pcmpgtd512_mask (__v16si, __v16si, __mmask16);

// __mmask8 __builtin_ia32_pcmpgtq512_mask (__v8di, int );
__mmask8 __builtin_ia32_pcmpgtq512_mask (__v8di, __v8di, __mmask8);

// __mmask16 __builtin_ia32_cmpd512_mask (__v16si, int );
// __mmask16 __builtin_ia32_cmpd512_mask (__v16si, int );
// __mmask16 __builtin_ia32_cmpd512_mask (__v16si, int );
__mmask16 __builtin_ia32_cmpd512_mask (__v16si, __v16si, int, __mmask16);


// __mmask16 __builtin_ia32_ucmpd512_mask (__v16si, int );
// __mmask16 __builtin_ia32_ucmpd512_mask (__v16si, int );
// __mmask16 __builtin_ia32_ucmpd512_mask (__v16si, int );
__mmask16 __builtin_ia32_ucmpd512_mask (__v16si, __v16si, int, __mmask16);

__mmask8 __builtin_ia32_ucmpq512_mask (__v8di, __v8di, int, __mmask8);
// __mmask8 __builtin_ia32_ucmpq512_mask (__v8di, int );
// __mmask8 __builtin_ia32_ucmpq512_mask (__v8di, int );

// __mmask8 __builtin_ia32_cmpq512_mask (__v8di, int );
// __mmask8 __builtin_ia32_cmpq512_mask (__v8di, int );
// __mmask8 __builtin_ia32_cmpq512_mask (__v8di, int );
__mmask8 __builtin_ia32_cmpq512_mask (__v8di,  __v8di, int, __mmask8);

__mmask8 __builtin_ia32_ucmpq512_mask (__v8di,  __v8di, int, __mmask8);

// __m512d __builtin_ia32_compressdf512_mask (__v8df, int );
__m512d __builtin_ia32_compressdf512_mask (__v8df, __v8df, __mmask8);

void   __builtin_ia32_compressstoredf512_mask (__v8df *, __v8df, int );

// __m512 __builtin_ia32_compresssf512_mask (__v16sf, int );
__m512 __builtin_ia32_compresssf512_mask (__v16sf, __v16sf, __mmask16);

void __builtin_ia32_compressstoresf512_mask (__v16sf *, __v16sf, int );

// __m512i __builtin_ia32_compressdi512_mask (__v8di, int );
__m512i __builtin_ia32_compressdi512_mask (__v8di,  __v8di, __mmask8);

void __builtin_ia32_compressstoredi512_mask (__v8di *, __v8di, int );

// __m512i __builtin_ia32_compresssi512_mask (__v16si, int );
__m512i __builtin_ia32_compresssi512_mask (__v16si, __v16si, __mmask16);

void __builtin_ia32_compressstoresi512_mask (__v16si *, __v16si, int );

__m512d __builtin_ia32_expanddf512_mask (__v8df, __v8df, __mmask8);
__m512d __builtin_ia32_expanddf512_maskz (__v8df, __v8df, __mmask8);
__m512d __builtin_ia32_expandloaddf512_mask (const __v8df *, __v8df, __mmask8);
__m512d __builtin_ia32_expandloaddf512_maskz (const __v8df *, __v8df, __mmask8);

__m512 __builtin_ia32_expandsf512_mask (__v16sf, __v16sf, __mmask16);
__m512 __builtin_ia32_expandsf512_maskz (__v16sf, __v16sf, __mmask16);
__m512 __builtin_ia32_expandloadsf512_mask (const __v16sf *, __v16sf, __mmask16);
__m512 __builtin_ia32_expandloadsf512_maskz (const __v16sf *, __v16sf, __mmask16);

__m512i __builtin_ia32_expanddi512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_expanddi512_maskz (__v8di, __v8di, __mmask8);

__m512i __builtin_ia32_expandloaddi512_mask (const __v8di *, __v8di, __mmask8);
__m512i __builtin_ia32_expandloaddi512_maskz (const __v8di *, __v8di, __mmask8);

__m512i __builtin_ia32_expandsi512_mask (__v16si, __v16si, __mmask16);
__m512i __builtin_ia32_expandsi512_maskz (__v16si, __v16si, __mmask16);

__m512i __builtin_ia32_expandloadsi512_mask (const __v16si *, __v16si, __mmask16);
__m512i __builtin_ia32_expandloadsi512_maskz (const __v16si *, __v16si, __mmask16);

__mmask16 __builtin_ia32_kandhi (__mmask16, __mmask16);
__mmask16 __builtin_ia32_kandnhi (__mmask16, __mmask16);
__mmask16 __builtin_ia32_korhi (__mmask16, __mmask16);
__mmask16 __builtin_ia32_kortestzhi (__mmask16, int );
__mmask16 __builtin_ia32_kortestchi (__mmask16, int );
__mmask16 __builtin_ia32_kxnorhi (__mmask16, __mmask16);
__mmask16 __builtin_ia32_kxorhi (__mmask16, __mmask16);
__mmask16 __builtin_ia32_knothi (__mmask16);
__mmask16 __builtin_ia32_kunpckhi (__mmask16, __mmask16);

__m512i __builtin_ia32_pmaxsq512_mask (__v8di, __v8di, __v8di, __mmask8);
// __m512i __builtin_ia32_pmaxsq512_mask (__v8di, int );
// __m512i __builtin_ia32_pmaxsq512_mask (__v8di, int );

// __m512i __builtin_ia32_pminsq512_mask (__v8di, int );
// __m512i __builtin_ia32_pminsq512_mask (__v8di, int );
__m512i __builtin_ia32_pminsq512_mask (__v8di, __v8di, __v8di, __mmask8);

// __m512i __builtin_ia32_pmaxuq512_mask (__v8di, int );
// __m512i __builtin_ia32_pmaxuq512_mask (__v8di, int );
__m512i __builtin_ia32_pmaxuq512_mask (__v8di, __v8di, __v8di, __mmask8);

// __m512i __builtin_ia32_pminuq512_mask (__v8di, int );
// __m512i __builtin_ia32_pminuq512_mask (__v8di, int );
__m512i __builtin_ia32_pminuq512_mask (__v8di, __v8di, __v8di, __mmask8);

// __m512i __builtin_ia32_pmaxsd512_mask (__v16si, int );
// __m512i __builtin_ia32_pmaxsd512_mask (__v16si, int );
__m512i __builtin_ia32_pmaxsd512_mask (__v16si, __v16si, __v16si, __mmask16);

// __m512i __builtin_ia32_pminsd512_mask (__v16si, int );
// __m512i __builtin_ia32_pminsd512_mask (__v16si, int );
__m512i __builtin_ia32_pminsd512_mask (__v16si, __v16si, __v16si, __mmask16);

// __m512i __builtin_ia32_pmaxud512_mask (__v16si, int );
// __m512i __builtin_ia32_pmaxud512_mask (__v16si, int );
__m512i __builtin_ia32_pmaxud512_mask (__v16si, __v16si, __v16si, __mmask16);

// __m512i __builtin_ia32_pminud512_mask (__v16si, int );
// __m512i __builtin_ia32_pminud512_mask (__v16si, int );
__m512i __builtin_ia32_pminud512_mask (__v16si, __v16si, __v16si, __mmask16);

// __m512 __builtin_ia32_unpcklps512_mask (__v16sf, int );
// __m512 __builtin_ia32_unpcklps512_mask (__v16sf, int );
__m512 __builtin_ia32_unpcklps512_mask (__v16sf, __v16sf, __v16sf, __mmask16);

__m512d __builtin_ia32_blendmpd_512_mask (__v8df, __v8df, __mmask8);

__m512  __builtin_ia32_blendmps_512_mask (__v16sf, __v16sf, __mmask16);

__m512i __builtin_ia32_blendmq_512_mask (__v8di, __v8di, __mmask8);
__m512i __builtin_ia32_blendmd_512_mask (__v16si, __v16si, __mmask16);

// __m512d __builtin_ia32_sqrtpd512_mask (__v8df, int );
// __m512d __builtin_ia32_sqrtpd512_mask (__v8df, int );
__m512d __builtin_ia32_sqrtpd512_mask (__v8df,  __v8df, __mmask8, int);

// __m512 __builtin_ia32_sqrtps512_mask (__v16sf, int );
// __m512 __builtin_ia32_sqrtps512_mask (__v16sf, int );
__m512 __builtin_ia32_sqrtps512_mask (__v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_addpd512_mask (__v8df, int );
// __m512d __builtin_ia32_addpd512_mask (__v8df, int );
__m512d __builtin_ia32_addpd512_mask (__v8df, __v8df, __v8df, __mmask8, int);

// __m512 __builtin_ia32_addps512_mask (__v16sf, int );
// __m512 __builtin_ia32_addps512_mask (__v16sf, int );
__m512 __builtin_ia32_addps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_subpd512_mask (__v8df, int );
// __m512d __builtin_ia32_subpd512_mask (__v8df, int );
__m512d __builtin_ia32_subpd512_mask (__v8df, __v8df, __v8df, __mmask8, int);

// __m512 __builtin_ia32_subps512_mask (__v16sf, int );
// __m512 __builtin_ia32_subps512_mask (__v16sf, int );
__m512 __builtin_ia32_subps512_mask (__v16sf, __v16sf,__v16sf, __mmask16, int);

// __m512d __builtin_ia32_mulpd512_mask (__v8df, int );
// __m512d __builtin_ia32_mulpd512_mask (__v8df, int );
__m512d __builtin_ia32_mulpd512_mask (__v8df, __v8df, __v8df, __mmask8, int);

// __m512 __builtin_ia32_mulps512_mask (__v16sf, int );
// __m512 __builtin_ia32_mulps512_mask (__v16sf, int );
__m512 __builtin_ia32_mulps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_divpd512_mask (__v8df, int );
// __m512d __builtin_ia32_divpd512_mask (__v8df, int );
__m512d __builtin_ia32_divpd512_mask (__v8df, __v8df, __v8df, __mmask8, int);

// __m512 __builtin_ia32_divps512_mask (__v16sf, int );
// __m512 __builtin_ia32_divps512_mask (__v16sf, int );
__m512 __builtin_ia32_divps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_maxpd512_mask (__v8df, int );
// __m512d __builtin_ia32_maxpd512_mask (__v8df, int );
__m512d __builtin_ia32_maxpd512_mask (__v8df, __v8df, __v8df, __mmask8, int);

// __m512 __builtin_ia32_maxps512_mask (__v16sf, int );
// __m512 __builtin_ia32_maxps512_mask (__v16sf, int );
__m512 __builtin_ia32_maxps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_minpd512_mask (__v8df, int );
// __m512d __builtin_ia32_minpd512_mask (__v8df, int );
__m512d __builtin_ia32_minpd512_mask (__v8df, __v8df, __v8df, __mmask8, int);

// __m512 __builtin_ia32_minps512_mask (__v16sf, int );
// __m512 __builtin_ia32_minps512_mask (__v16sf, int );
__m512 __builtin_ia32_minps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_scalefpd512_mask (__v8df, int );
// __m512d __builtin_ia32_scalefpd512_mask (__v8df, int );
__m512d __builtin_ia32_scalefpd512_mask (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_scalefps512_mask (__v16sf, int );
// __m512 __builtin_ia32_scalefps512_mask (__v16sf, int );
__m512 __builtin_ia32_scalefps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);

__m128d __builtin_ia32_scalefsd_round (__v2df, __v2df, int );
__m128 __builtin_ia32_scalefss_round (__v4sf, __v4sf, int );

// __m512d __builtin_ia32_vfmaddpd512_mask (__v8df, int );
// __m512d __builtin_ia32_vfmaddpd512_mask (__v8df, int );
// __m512d __builtin_ia32_vfmaddpd512_mask (__v8df, int );
__m512d __builtin_ia32_vfmaddpd512_mask (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddpd512_mask3 (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddpd512_maskz (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_vfmaddps512_mask (__v16sf, int );
__m512 __builtin_ia32_vfmaddps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddps512_mask3 (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddps512_maskz (__v16sf, __v16sf, __v16sf, __mmask16, int);

__m512d __builtin_ia32_vfmsubpd512_mask3 (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddpd512_maskz (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_vfmaddps512_mask (__v16sf, int );
__m512 __builtin_ia32_vfmaddps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmsubps512_mask3 (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddps512_maskz (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_vfmaddsubpd512_mask (__v8df, int );
__m512d __builtin_ia32_vfmaddsubpd512_mask (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddsubpd512_mask3 (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddsubpd512_maskz (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_vfmaddsubps512_mask (__v16sf, int );
__m512 __builtin_ia32_vfmaddsubps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddsubps512_mask3 (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddsubps512_maskz (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_vfmaddsubpd512_mask (__v8df, int );
__m512d __builtin_ia32_vfmaddsubpd512_mask (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmsubaddpd512_mask3 (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddsubpd512_maskz (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_vfmaddsubps512_mask (__v16sf, int );
__m512 __builtin_ia32_vfmaddsubps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmsubaddps512_mask3 (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddsubps512_maskz (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_vfmaddpd512_mask (__v8df, int );
__m512d __builtin_ia32_vfnmaddpd512_mask (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddpd512_mask3 (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddpd512_maskz (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_vfmaddps512_mask (__v16sf, int );
__m512 __builtin_ia32_vfnmaddps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddps512_mask3 (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddps512_maskz (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m512d __builtin_ia32_vfmaddpd512_mask (__v8df, int );
__m512d __builtin_ia32_vfnmsubpd512_mask (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfnmsubpd512_mask3 (__v8df, __v8df,__v8df,  __mmask8, int);
__m512d __builtin_ia32_vfmaddpd512_maskz (__v8df, __v8df,__v8df,  __mmask8, int);

// __m512 __builtin_ia32_vfmaddps512_mask (__v16sf, int );
__m512 __builtin_ia32_vfnmsubps512_mask (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfnmsubps512_mask3 (__v16sf, __v16sf, __v16sf, __mmask16, int);
__m512 __builtin_ia32_vfmaddps512_maskz (__v16sf, __v16sf, __v16sf, __mmask16, int);

// __m256i __builtin_ia32_cvttpd2dq512_mask (__v8df, int );
// __m256i __builtin_ia32_cvttpd2dq512_mask (__v8df, int );
__m256i __builtin_ia32_cvttpd2dq512_mask (__v8df, __v8si, __mmask8, int);

// __m256i __builtin_ia32_cvttpd2udq512_mask (__v8df, int );
// __m256i __builtin_ia32_cvttpd2udq512_mask (__v8df, int );
__m256i __builtin_ia32_cvttpd2udq512_mask (__v8df, __v8si, __mmask8, int );

// __m256i __builtin_ia32_cvtpd2dq512_mask (__v8df, int );
// __m256i __builtin_ia32_cvtpd2dq512_mask (__v8df, int );
__m256i __builtin_ia32_cvtpd2dq512_mask (__v8df, __v8si, __mmask8, int);

// __m256i __builtin_ia32_cvtpd2udq512_mask (__v8df, int );
// __m256i __builtin_ia32_cvtpd2udq512_mask (__v8df, int );
__m256i __builtin_ia32_cvtpd2udq512_mask (__v8df, __v8si, __mmask8, int);

// __m512i __builtin_ia32_cvttps2dq512_mask (__v16sf, int );
// __m512i __builtin_ia32_cvttps2dq512_mask (__v16sf, int );
__m512i __builtin_ia32_cvttps2dq512_mask (__v16sf, __v16si, __mmask16, int);

// __m512i __builtin_ia32_cvttps2udq512_mask (__v16sf, int );
// __m512i __builtin_ia32_cvttps2udq512_mask (__v16sf, int );
__m512i __builtin_ia32_cvttps2udq512_mask (__v16sf, __v16si, __mmask16, int);

// __m512i __builtin_ia32_cvtps2dq512_mask (__v16sf, int );
// __m512i __builtin_ia32_cvtps2dq512_mask (__v16sf, int );
__m512i __builtin_ia32_cvtps2dq512_mask (__v16sf, __v16si, __mmask16, int);

// __m512i __builtin_ia32_cvtps2udq512_mask (__v16sf, int );
// __m512i __builtin_ia32_cvtps2udq512_mask (__v16sf, int );
__m512i __builtin_ia32_cvtps2udq512_mask (__v16sf, __v16si, __mmask16, int);

__m128  __builtin_ia32_cvtusi2ss64 (__v4sf, unsigned long long, int );
__m128d __builtin_ia32_cvtusi2sd64 (__v2df, unsigned long long, int );
__m128  __builtin_ia32_cvtusi2ss32 (__v4sf, unsigned, int );

// __m512 __builtin_ia32_cvtdq2ps512_mask (__v16si, int );
// __m512 __builtin_ia32_cvtdq2ps512_mask (__v16si, int );
__m512 __builtin_ia32_cvtdq2ps512_mask (__v16si, __v16sf, __mmask16, int);

// __m512 __builtin_ia32_cvtudq2ps512_mask (__v16si, int );
// __m512 __builtin_ia32_cvtudq2ps512_mask (__v16si, int );
__m512 __builtin_ia32_cvtudq2ps512_mask (__v16si, __v16sf, __mmask16, int);

unsigned long long __builtin_ia32_vcvtss2usi64 (__v4sf, int );
unsigned long long __builtin_ia32_vcvttss2usi64 (__v4sf, int );

long long __builtin_ia32_vcvttss2si64 (__v4sf, int );

unsigned __builtin_ia32_vcvtss2usi32 (__v4sf, int );
unsigned __builtin_ia32_vcvttss2usi32 (__v4sf, int );

int __builtin_ia32_vcvttss2si32 (__v4sf, int );

unsigned long long __builtin_ia32_vcvtsd2usi64 (__v2df, int );
unsigned long long __builtin_ia32_vcvttsd2usi64 (__v2df, int );

long long __builtin_ia32_vcvttsd2si64 (__v2df, int );

unsigned __builtin_ia32_vcvtsd2usi32 (__v2df, int );
unsigned __builtin_ia32_vcvttsd2usi32 (__v2df, int );

int __builtin_ia32_vcvttsd2si32 (__v2df, int );

// __m512d __builtin_ia32_cvtps2pd512_mask (__v8sf, int );
// __m512d __builtin_ia32_cvtps2pd512_mask (__v8sf, int );
__m512d __builtin_ia32_cvtps2pd512_mask (__v8sf, __v8df, __mmask8, int);

// __m512 __builtin_ia32_vcvtph2ps512_mask (__v16hi, int );
// __m512 __builtin_ia32_vcvtph2ps512_mask (__v16hi, int );
__m512 __builtin_ia32_vcvtph2ps512_mask (__v16hi, __v16sf, __mmask16, int);

// __m256 __builtin_ia32_cvtpd2ps512_mask (__v8df, int );
// __m256 __builtin_ia32_cvtpd2ps512_mask (__v8df, int );
__m256 __builtin_ia32_cvtpd2ps512_mask (__v8df, __v8sf, __mmask8, int);

__mmask16 __builtin_ia32_kmov16 (__mmask16);

// void __builtin_ia32_vpconflictsi_512_mask (__v16si, int );
// void __builtin_ia32_vpconflictsi_512_mask (__v16si, int );
__m512i __builtin_ia32_vpconflictsi_512_mask (__v16si, __v16si, __mmask16);

// void __builtin_ia32_vpconflictdi_512_mask (__v8di, int );
__m512i __builtin_ia32_vpconflictdi_512_mask (__v8di, __v8di, __mmask8);
// void __builtin_ia32_vpconflictdi_512_mask (__v8di, int );

// void __builtin_ia32_vplzcntq_512_mask (__v8di, int );
__m512i __builtin_ia32_vplzcntq_512_mask (__v8di, __v8di, __mmask8);
// void __builtin_ia32_vplzcntq_512_mask (__v8di, int );
// void __builtin_ia32_vplzcntd_512_mask (__v16si, int );
__m512i __builtin_ia32_vplzcntd_512_mask (__v16si, __v16si, __mmask16);
// void __builtin_ia32_vplzcntd_512_mask (__v16si, int );

__m512i __builtin_ia32_broadcastmb512 (__mmask8);
__m512i __builtin_ia32_broadcastmw512 (__mmask16);

__m128i __builtin_ia32_sha1msg1 (__v4si, __v4si);
__m128i __builtin_ia32_sha1msg2 (__v4si, __v4si);
__m128i __builtin_ia32_sha1nexte (__v4si, __v4si);
__m128i __builtin_ia32_sha256msg1 (__v4si, __v4si);
__m128i __builtin_ia32_sha256msg2 (__v4si, __v4si);
__m128i __builtin_ia32_sha256rnds2 (__v4si, __v4si, __v4si);

unsigned short __builtin_clzs (unsigned short);
unsigned short __builtin_ctzs (unsigned int);
unsigned int __builtin_ia32_bextr_u32 (unsigned int, unsigned int);
// void __builtin_ia32_bextr_u32 (unsigned int, int);
// void __builtin_ia32_bextr_u64 (unsigned int, int);
unsigned long long __builtin_ia32_bextr_u64 (unsigned long long, unsigned long long);

unsigned int __builtin_ia32_bzhi_si (unsigned int, unsigned int);
unsigned int __builtin_ia32_pdep_si (unsigned int, unsigned int);
unsigned int __builtin_ia32_pext_si (unsigned int, unsigned int);
unsigned long __builtin_ia32_bzhi_di (unsigned long, unsigned long);
unsigned long __builtin_ia32_pdep_di (unsigned long, unsigned long);
unsigned long __builtin_ia32_pext_di (unsigned long, unsigned long);

__m128d __builtin_ia32_vfmaddpd (__v2df, __v2df, __v2df );
__m256d __builtin_ia32_vfmaddpd256 (__v4df, __v4df, __v4df );
__m128 __builtin_ia32_vfmaddps (__v4sf, __v4sf, __v4sf );
__m256 __builtin_ia32_vfmaddps256 (__v8sf, __v8sf, __v8sf );
__m128d __builtin_ia32_vfmaddsd3 (__v2df, __v2df, __v2df );
__m128 __builtin_ia32_vfmaddss3 (__v4sf, __v4sf, __v4sf );
__m128d __builtin_ia32_vfmaddpd (__v2df, __v2df, __v2df );
__m256d __builtin_ia32_vfmaddpd256 (__v4df, __v4df, __v4df );
__m128 __builtin_ia32_vfmaddps (__v4sf, __v4sf, __v4sf );
__m256 __builtin_ia32_vfmaddps256 (__v8sf, __v8sf, __v8sf );
__m128d __builtin_ia32_vfmaddsd3 (__v2df, __v2df, __v2df );
__m128 __builtin_ia32_vfmaddss3 (__v4sf, __v4sf, __v4sf );
__m128d __builtin_ia32_vfmaddpd (__v2df, __v2df, __v2df );
__m256d __builtin_ia32_vfmaddpd256 (__v4df, __v4df, __v4df );
__m128 __builtin_ia32_vfmaddps (__v4sf, __v4sf, __v4sf );
__m256 __builtin_ia32_vfmaddps256 (__v8sf, __v8sf, __v8sf );
__m128d __builtin_ia32_vfmaddsd3 (__v2df, __v2df,__v2df );
__m128 __builtin_ia32_vfmaddss3 (__v4sf, __v4sf, __v4sf );
__m128d __builtin_ia32_vfmaddpd (__v2df, __v2df, __v2df );
__m256d __builtin_ia32_vfmaddpd256 (__v4df, __v4df, __v4df );
__m128 __builtin_ia32_vfmaddps (__v4sf, __v4sf, __v4sf );
__m256 __builtin_ia32_vfmaddps256 (__v8sf, __v8sf, __v8sf );
__m128d __builtin_ia32_vfmaddsd3 (__v2df, __v2df, __v2df );
__m128 __builtin_ia32_vfmaddss3 (__v4sf, __v4sf, __v4sf );
__m128d __builtin_ia32_vfmaddsubpd (__v2df, __v2df, __v2df );
__m256d __builtin_ia32_vfmaddsubpd256 (__v4df, __v4df, __v4df );
__m128 __builtin_ia32_vfmaddsubps (__v4sf, __v4sf, __v4sf );
__m256 __builtin_ia32_vfmaddsubps256 (__v8sf, __v8sf, __v8sf);
__m128d __builtin_ia32_vfmaddsubpd (__v2df, __v2df, __v2df );
__m256d __builtin_ia32_vfmaddsubpd256 (__v4df, __v4df, __v4df );
__m128 __builtin_ia32_vfmaddsubps (__v4sf, __v4sf, __v4sf );



// DQ (1/25/2017): Unclear which if these to use for C langauge support (can't be overloaded in C).
#ifdef __cplusplus
__v4sf __builtin_ia32_vcvtph2ps (__v4sf);
#endif

// __v4sf __builtin_ia32_vcvtph2ps (__v4sf);
__m128 __builtin_ia32_vcvtph2ps (__v8hi);
__m256 __builtin_ia32_vcvtph2ps256 (__v8hi);
unsigned int __builtin_ia32_xbegin ();
void __builtin_ia32_xend ();
int  __builtin_ia32_xtest ();
int  __builtin_ia32_rdrand16_step (unsigned short*);
int  __builtin_ia32_rdrand32_step (unsigned int *);
unsigned int __builtin_ia32_rdfsbase32 ();
unsigned long __builtin_ia32_rdfsbase64 ();
unsigned int __builtin_ia32_rdgsbase32 ();
unsigned long __builtin_ia32_rdgsbase64 ();
void __builtin_ia32_wrfsbase32 (unsigned int);
void __builtin_ia32_wrfsbase64 (unsigned long);
void __builtin_ia32_wrgsbase32 (unsigned int);
void __builtin_ia32_wrgsbase64 (unsigned long);
int  __builtin_ia32_rdrand64_step (unsigned long long*);
void __builtin_ia32_femms();
__m64 __builtin_ia32_pavgusb (__v8qi, __v8qi );
__m64 __builtin_ia32_pf2id (__v2sf);
__m64 __builtin_ia32_pfacc (__v2sf, __v2sf);
__m64 __builtin_ia32_pfadd (__v2sf, __v2sf);
__m64 __builtin_ia32_pfcmpeq (__v2sf, __v2sf);
__m64 __builtin_ia32_pfcmpge (__v2sf, __v2sf);
__m64 __builtin_ia32_pfcmpgt (__v2sf, __v2sf);
__m64 __builtin_ia32_pfmax (__v2sf, __v2sf);
__m64 __builtin_ia32_pfmin (__v2sf, __v2sf);
__m64 __builtin_ia32_pfmul (__v2sf, __v2sf);
__m64 __builtin_ia32_pfrcp (__v2sf);
__m64 __builtin_ia32_pfrcpit1 (__v2sf, __v2sf);
__m64 __builtin_ia32_pfrcpit2 (__v2sf, __v2sf);
__m64 __builtin_ia32_pfrsqrt (__v2sf);
__m64 __builtin_ia32_pfrsqit1 (__v2sf, __v2sf);
__m64 __builtin_ia32_pfsub (__v2sf, __v2sf);
__m64 __builtin_ia32_pfsubr (__v2sf , __v2sf );
__m64 __builtin_ia32_pi2fd (__v2si);
__m64 __builtin_ia32_pmulhrw (__v4hi, __v4hi);
__m128 __builtin_ia32_vfmaddss (__v4sf, __v4sf, __v4sf);
__m128d __builtin_ia32_vfmaddsd (__v2df, __v2df, __v2df);
__m128 __builtin_ia32_vfmaddss (__v4sf, __v4sf, __v4sf);
__m128d __builtin_ia32_vfmaddsd (__v2df, __v2df, __v2df);
__m128 __builtin_ia32_vfmaddss (__v4sf, __v4sf, __v4sf);
__m128d __builtin_ia32_vfmaddsd (__v2df, __v2df, __v2df);
__m128 __builtin_ia32_vfmaddss (__v4sf, __v4sf, __v4sf);
__m128d __builtin_ia32_vfmaddsd (__v2df, __v2df, __v2df);
__m128i __builtin_ia32_vpmacssww (__v8hi,__v8hi, __v8hi);
__m128i __builtin_ia32_vpmacsww (__v8hi, __v8hi, __v8hi);
__m128i __builtin_ia32_vpmacsswd (__v8hi, __v8hi, __v4si);
__m128i __builtin_ia32_vpmacswd (__v8hi, __v8hi, __v4si);
__m128i __builtin_ia32_vpmacssdd (__v4si, __v4si, __v4si);
__m128i __builtin_ia32_vpmacsdd (__v4si, __v4si, __v4si);
__m128i __builtin_ia32_vpmacssdql (__v4si, __v4si, __v2di);
__m128i __builtin_ia32_vpmacsdql (__v4si, __v4si, __v2di);
__m128i __builtin_ia32_vpmacssdqh (__v4si, __v4si, __v2di);
__m128i __builtin_ia32_vpmacsdqh (__v4si, __v4si, __v2di);
__m128i __builtin_ia32_vpmadcsswd (__v8hi,__v8hi,__v4si);
__m128i __builtin_ia32_vpmadcswd (__v8hi,__v8hi,__v4si);
__m128i __builtin_ia32_vphaddbw (__v16qi);
__m128i __builtin_ia32_vphaddbd (__v16qi);
__m128i __builtin_ia32_vphaddbq (__v16qi);
__m128i __builtin_ia32_vphaddwd (__v8hi);
__m128i __builtin_ia32_vphaddwq (__v8hi);
__m128i __builtin_ia32_vphadddq (__v4si);
__m128i __builtin_ia32_vphaddubw (__v16qi);
__m128i __builtin_ia32_vphaddubd (__v16qi);
__m128i __builtin_ia32_vphaddubq (__v16qi);
__m128i __builtin_ia32_vphadduwd (__v8hi);
__m128i __builtin_ia32_vphadduwq (__v8hi);
__m128i __builtin_ia32_vphaddudq (__v4si);
__m128i __builtin_ia32_vphsubbw (__v16qi);
__m128i __builtin_ia32_vphsubwd (__v8hi);
__m128i __builtin_ia32_vphsubdq (__v4si);
__m128i __builtin_ia32_vpcmov (__m128i, __m128i, __m128i);
__m128i __builtin_ia32_vpperm (__v16qi, __v16qi, __v16qi);
__m128i __builtin_ia32_vprotb (__v16qi, __v16qi);
__m128i __builtin_ia32_vprotw (__v8hi, __v8hi);
__m128i __builtin_ia32_vprotd (__v4si, __v4si);
__m128i __builtin_ia32_vprotq (__v2di, __v2di);
__m128i __builtin_ia32_vpshlb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpshlw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpshld (__v4si, __v4si);
__m128i __builtin_ia32_vpshlq (__v2di, __v2di);
__m128i __builtin_ia32_vpshab (__v16qi, __v16qi);
__m128i __builtin_ia32_vpshaw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpshad (__v4si, __v4si);
__m128i __builtin_ia32_vpshaq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomltub (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomleub (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomgtub (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomgeub (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomequb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomnequb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomfalseub (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomtrueub (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomltuw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomleuw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomgtuw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomgeuw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomequw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomnequw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomfalseuw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomtrueuw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomltud (__v4si, __v4si);
__m128i __builtin_ia32_vpcomleud (__v4si, __v4si);
__m128i __builtin_ia32_vpcomgtud (__v4si, __v4si);
__m128i __builtin_ia32_vpcomgeud (__v4si, __v4si);
__m128i __builtin_ia32_vpcomequd (__v4si, __v4si);
__m128i __builtin_ia32_vpcomnequd (__v4si, __v4si);
__m128i __builtin_ia32_vpcomfalseud (__v4si, __v4si);
__m128i __builtin_ia32_vpcomtrueud (__v4si, __v4si);
__m128i __builtin_ia32_vpcomltuq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomleuq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomgtuq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomgeuq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomequq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomnequq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomfalseuq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomtrueuq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomltb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomleb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomgtb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomgeb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomeqb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomneqb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomfalseb (__v16qi, __v16qi);
__m128i __builtin_ia32_vpcomtrueb (__v16qi, __v16qi );
__m128i __builtin_ia32_vpcomltw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomlew (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomgtw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomgew (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomeqw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomneqw (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomfalsew (__v8hi, __v8hi);
__m128i __builtin_ia32_vpcomtruew (__v8hi, __v8hi );
__m128i __builtin_ia32_vpcomltd (__v4si, __v4si);
__m128i __builtin_ia32_vpcomled (__v4si, __v4si);
__m128i __builtin_ia32_vpcomgtd (__v4si, __v4si);
__m128i __builtin_ia32_vpcomged (__v4si, __v4si);
__m128i __builtin_ia32_vpcomeqd (__v4si, __v4si);
__m128i __builtin_ia32_vpcomneqd (__v4si, __v4si);
__m128i __builtin_ia32_vpcomfalsed (__v4si, __v4si);
__m128i __builtin_ia32_vpcomtrued (__v4si, __v4si );
__m128i __builtin_ia32_vpcomltq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomleq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomgtq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomgeq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomeqq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomneqq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomfalseq (__v2di, __v2di);
__m128i __builtin_ia32_vpcomtrueq (__v2di, __v2di );
__m128  __builtin_ia32_vfrczps (__v4sf);
__m128d __builtin_ia32_vfrczpd (__v2df);
__v4sf  __builtin_ia32_vfrczss (__v4sf);
__v2df  __builtin_ia32_vfrczsd (__v2df);
__m256  __builtin_ia32_vfrczps256 (__v8sf);
__m256d __builtin_ia32_vfrczpd256 (__v4df);
void    __builtin_ia32_llwpcb (void*);
void*   __builtin_ia32_slwpcb ();
int __builtin_ia32_rdseed_hi_step (unsigned short*);
int __builtin_ia32_rdseed_si_step (unsigned int*);
int __builtin_ia32_rdseed_di_step (unsigned long long*);
void __builtin_ia32_xsave (void* __P, long long __M);
void __builtin_ia32_xrstor (void* __P, long long __M);
void __builtin_ia32_xsave64 (void* __P, long long __M);
void __builtin_ia32_xrstor64 (void*__P, long long __M);
void __builtin_ia32_xsaveopt (void*__P, long long __M);
void __builtin_ia32_xsaveopt64 (void*__P, long long __M);

// DQ (3/12/2017): Added refinements to support Clang.
#if defined(__clang__)
  #undef __m64
#endif

#endif


#if 1
// DQ (1/22/2017): More builtins specific to GNU 5.1 (__mmask32 is required for C++11 mode in GNU 4.8 and likely later versions).

// DQ (1/25/2017): Note that __mmask32 is used template arguments and so in the generated code it is not defined.
// Use a CPP macro instead to get it into the generated code more directly.
// typedef unsigned int __mmask32;
#define __mmask32 unsigned int

typedef unsigned long long __mmask64;

// typedef short __v32hi ROSE_GNUATTR((__vector_size__ (64)));
// typedef char __v64qi ROSE_GNUATTR((__vector_size__ (64)));

__m512i __builtin_ia32_pbroadcastq512_gpr_mask (long long, __v8di, __mmask8);
// __m512i __builtin_ia32_pbroadcastq512_gpr_mask (__A, __v8di, int );
//         __builtin_ia32_pbroadcastq512_gpr_mask (__A, int );
__m512d __builtin_ia32_pd512_pd(__m128d);
__m512  __builtin_ia32_ps512_ps(__m128);
__m512i __builtin_ia32_si512_si(__v4si);
// __m512  __builtin_ia32_pd512_256pd (__m256d);
__m512d __builtin_ia32_pd512_256pd (__m256d);
__m512  __builtin_ia32_ps512_256ps (__m256);
__m512i __builtin_ia32_si512_256si (__v8si);
__m256d __builtin_ia32_movapd256_mask (__v4df, __v4df, __mmask8);
__m128d __builtin_ia32_movapd128_mask (__v2df, __v2df, __mmask8);
__m256d __builtin_ia32_loadapd256_mask (__v4df *, __v4df, __mmask8);
__m128d __builtin_ia32_loadapd128_mask (__v2df *, __v2df, __mmask8);
void    __builtin_ia32_storeapd256_mask (__v4df *, __v4df, __mmask8);
void    __builtin_ia32_storeapd128_mask (__v2df *, __v2df, __mmask8);
__m256 __builtin_ia32_movaps256_mask (__v8sf, __v8sf, __mmask8);
__m128 __builtin_ia32_movaps128_mask (__v4sf, __v4sf, __mmask8);
__m256 __builtin_ia32_loadaps256_mask (__v8sf *, __v8sf, __mmask8);
__m128 __builtin_ia32_loadaps128_mask (__v4sf *, __v4sf, __mmask8);
void   __builtin_ia32_storeaps256_mask (__v8sf *, __v8sf, __mmask8);
void   __builtin_ia32_storeaps128_mask (__v4sf *, __v4sf, __mmask8);
__m256i __builtin_ia32_movdqa64_256_mask (__v4di, __v4di, __mmask8);
__m128i __builtin_ia32_movdqa64_128_mask (__v2di, __v2di, __mmask8);
__m256i __builtin_ia32_movdqa64load256_mask (__v4di *, __v4di, __mmask8);
__m128i __builtin_ia32_movdqa64load128_mask (__v2di *, __v2di, __mmask8);
void    __builtin_ia32_movdqa64store256_mask (__v4di *, __v4di, __mmask8);
void    __builtin_ia32_movdqa64store128_mask (__v2di *, __v2di, __mmask8);
__m256i __builtin_ia32_movdqa32_256_mask (__v8si, __v8si, __mmask8);
__m128i __builtin_ia32_movdqa32_128_mask (__v4si, __v4si, __mmask8);
__m256i __builtin_ia32_movdqa32load256_mask (__v8si *, __v8si, __mmask8);
__m128i __builtin_ia32_movdqa32load128_mask (__v4si *, __v4si, __mmask8);
void    __builtin_ia32_movdqa32store256_mask (__v8si *, __v8si, __mmask8);
void    __builtin_ia32_movdqa32store128_mask (__v4si *, __v4si, __mmask8);
__m128d __builtin_ia32_addpd128_mask (__v2df, __v2df, __v2df, __mmask8);
__m256d __builtin_ia32_addpd256_mask (__v4df, __v4df, __v4df, __mmask8);
__m128 __builtin_ia32_addps128_mask (__v4sf, __v4sf, __v4sf, __mmask8);
__m256 __builtin_ia32_addps256_mask (__v8sf, __v8sf, __v8sf, __mmask8);
__m128d __builtin_ia32_subpd128_mask (__v2df,__v2df, __v2df, __mmask8);
__m256d __builtin_ia32_subpd256_mask (__v4df, __v4df, __v4df, __mmask8);
__m128 __builtin_ia32_subps128_mask (__v4sf, __v4sf, __v4sf, __mmask8);
__m256 __builtin_ia32_subps256_mask (__v8sf, __v8sf, __v8sf, __mmask8);
__m256d __builtin_ia32_loadupd256_mask (__v4df *, __v4df, __mmask8);
__m128d __builtin_ia32_loadupd128_mask (__v2df *, __v2df, __mmask8);
void    __builtin_ia32_storeupd256_mask (__v4df *, __v4df, __mmask8);
void    __builtin_ia32_storeupd128_mask (__v2df *, __v2df, __mmask8);
__m256 __builtin_ia32_loadups256_mask (__v8sf *, __v8sf, __mmask8);
__m128 __builtin_ia32_loadups128_mask (__v4sf *, __v4sf, __mmask8);
void    __builtin_ia32_storeups256_mask (__v8sf *, __v8sf, __mmask8);
void    __builtin_ia32_storeups128_mask (__v4sf *, __v4sf, __mmask8);
__m256i __builtin_ia32_loaddqudi256_mask (__v4di *, __v4di, __mmask8);
__m128i __builtin_ia32_loaddqudi128_mask (__v2di *, __v2di, __mmask8);
void    __builtin_ia32_storedqudi256_mask (__v4di *, __v4di, __mmask8);
void    __builtin_ia32_storedqudi128_mask (__v2di *, __v2di, __mmask8);
__m256i __builtin_ia32_loaddqusi256_mask (__v8si *, __v8si, __mmask8);
__m128i __builtin_ia32_loaddqusi128_mask (__v4si *, __v4si, __mmask8);
void    __builtin_ia32_storedqusi256_mask (__v8si *, __v8si, __mmask8);
void    __builtin_ia32_storedqusi128_mask (__v4si *, __v4si, __mmask8);
__m256i __builtin_ia32_pabsd256_mask (__v8si, __v8si, __mmask8);
__m128i __builtin_ia32_pabsd128_mask (__v4si, __v4si, __mmask8);
__m256i __builtin_ia32_pabsq256_mask (__v4di, __v4di, __mmask8);
__m128i __builtin_ia32_pabsq128_mask (__v2di, __v2di, __mmask8);
__m128i __builtin_ia32_cvtpd2udq256_mask (__v4df, __v4si, __mmask8);
__m128i __builtin_ia32_cvtpd2udq128_mask (__v2df, __v4si, __mmask8);
__m256i __builtin_ia32_cvttps2dq256_mask (__v8sf, __v8si, __mmask8);


// DQ (3/4/2017): Adding more builting functions (specific to GNU 5.1 compiler - C++11 mode).
// __m128 __builtin_ia32_extractf32x4_mask (__m512 __A, int, __m128, __mmask8);
// __m128i __builtin_ia32_extracti32x4_mask (__m512i, int, __m128i, __mmask8);

__m128  __builtin_ia32_extractf32x4_mask (__v16sf __A, int, __v4sf, __mmask8);
__m128i __builtin_ia32_extracti32x4_mask (__v16si __A, int, __v4si, __mmask8);
__m256d __builtin_ia32_extractf64x4_mask (__v8df  __A, int, __v4df, __mmask8);
// __m256d __builtin_ia32_extractf64x4_mask ((__v8df)(__m512d) ((__m512d)__A), (int) (0), (__v4df)(__m256d)_mm256_undefined_pd(), (__mmask8)-1));
// __m256d __builtin_ia32_extractf64x4_mask ((__v8df)(__m512d) ((__m512d)__A), (int) (0), (__v4df)(__m256d)_mm256_undefined_pd(), (__mmask8)-1));

__m128i __builtin_ia32_cvttps2dq128_mask  (__v4sf __A, __v4si __W, __mmask8 __U);
__m256i __builtin_ia32_cvttps2udq256_mask (__v8sf __A, __v8si __W, __mmask8 __U);
__m128i __builtin_ia32_cvttps2udq128_mask (__v4sf __A, __v4si, __mmask8);

// DQ (3/4/2017): Added more builtin function declaration for GNU 5.1 (required for STL tests using C++11 mode).
// #include "/home/quinlan1/ROSE/git_rose_development/new_builtin_list_2.txt"
#if 1
__m128i __builtin_ia32_cvttps2udq128_mask (__v4sf __A, __v4si setzero, __mmask8 minus_one);
__m128i __builtin_ia32_cvttps2udq128_mask (__v4sf __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvttps2udq128_mask (__v4sf __A, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2dq256_mask (__v4df __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2dq256_mask (__v4df __A, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2dq128_mask (__v2df __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2dq128_mask (__v2df __A, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2udq256_mask (__v4df __A, __v4si setzero, __mmask8 minus_one);
__m128i __builtin_ia32_cvttpd2udq256_mask (__v4df __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2udq256_mask (__v4df __A, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2udq128_mask (__v2df __A, __v4si setzero, __mmask8 minus_one);
__m128i __builtin_ia32_cvttpd2udq128_mask (__v2df __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2udq128_mask (__v2df __A, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_cvtpd2dq256_mask (__v4df __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvtpd2dq256_mask (__v4df __A, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_cvtpd2dq128_mask (__v2df __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_cvtpd2dq128_mask (__v2df __A, __v4si setzero, __mmask8 __U);
__m256d __builtin_ia32_cvtdq2pd256_mask (__v4si __A, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_cvtdq2pd256_mask (__v4si __A, __v4df setzero, __mmask8 __U);
__m128d __builtin_ia32_cvtdq2pd128_mask (__v4si __A, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_cvtdq2pd128_mask (__v4si __A, __v2df setzero, __mmask8 __U);
__m256d __builtin_ia32_cvtudq2pd256_mask (__v4si __A, __v4df setzero, __mmask8 minus_one);
__m256d __builtin_ia32_cvtudq2pd256_mask (__v4si __A, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_cvtudq2pd256_mask (__v4si __A, __v4df setzero, __mmask8 __U);
__m128d __builtin_ia32_cvtudq2pd128_mask (__v4si __A, __v2df setzero, __mmask8 minus_one);
__m128d __builtin_ia32_cvtudq2pd128_mask (__v4si __A, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_cvtudq2pd128_mask (__v4si __A, __v2df setzero, __mmask8 __U);
__m256 __builtin_ia32_cvtdq2ps256_mask (__v8si __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_cvtdq2ps256_mask (__v8si __A, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_cvtdq2ps128_mask (__v4si __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_cvtdq2ps128_mask (__v4si __A, __v4sf setzero, __mmask8 __U);
__m256 __builtin_ia32_cvtudq2ps256_mask (__v8si __A, __v8sf setzero, __mmask8 minus_one);
__m256 __builtin_ia32_cvtudq2ps256_mask (__v8si __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_cvtudq2ps256_mask (__v8si __A, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_cvtudq2ps128_mask (__v4si __A, __v4sf setzero, __mmask8 minus_one);
__m128 __builtin_ia32_cvtudq2ps128_mask (__v4si __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_cvtudq2ps128_mask (__v4si __A, __v4sf setzero, __mmask8 __U);
__m256d __builtin_ia32_cvtps2pd256_mask (__v4sf __A, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_cvtps2pd256_mask (__v4sf __A, __v4df setzero, __mmask8 __U);
__m128d __builtin_ia32_cvtps2pd128_mask (__v4sf __A, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_cvtps2pd128_mask (__v4sf __A, __v2df setzero, __mmask8 __U);
__m128i __builtin_ia32_pmovdb128_mask (__v4si __A, __v16qi undefined, __mmask8 minus_one);
  void __builtin_ia32_pmovdb128mem_mask (__v16qi* __P, __v4si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovdb128_mask (__v4si __A, __v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovdb128_mask (__v4si __A, __v16qi  setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovdb256_mask (__v8si __A, __v16qi  undefined, __mmask8 minus_one);
__m128i __builtin_ia32_pmovdb256_mask (__v8si __A, __v16qi  __O, __mmask8 __M);
  void __builtin_ia32_pmovdb256mem_mask (__v16qi * __P, __v8si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovdb256_mask (__v8si __A, __v16qi  setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovsdb128_mask (__v4si __A, __v16qi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsdb128mem_mask (__v16qi * __P, __v4si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsdb128_mask (__v4si __A, __v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsdb128_mask (__v4si __A, __v16qi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovsdb256_mask (__v8si __A, __v16qi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsdb256mem_mask (__v16qi * __P, __v8si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsdb256_mask (__v8si __A, __v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsdb256_mask (__v8si __A, __v16qi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovusdb128_mask (__v4si __A,  __v16qi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusdb128mem_mask (__v16qi * __P, __v4si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusdb128_mask (__v4si __A,  __v16qi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pmovusdb128_mask (__v4si __A,  __v16qi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovusdb256_mask (__v8si __A,  __v16qi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusdb256mem_mask (__v16qi* __P, __v8si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusdb256_mask (__v8si __A,  __v16qi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pmovusdb256_mask (__v8si __A,  __v16qi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovdw128_mask (__v4si __A,__v8hi  setzero,__mmask8 minus_one);
void  __builtin_ia32_pmovdw128mem_mask (__v8hi * __P, __v4si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovdw128_mask (__v4si __A,__v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovdw128_mask (__v4si __A,__v8hi setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovdw256_mask (__v8si __A,__v8hi setzero,__mmask8 minus_one);
void  __builtin_ia32_pmovdw256mem_mask (__v8hi * __P, __v8si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovdw256_mask (__v8si __A,__v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovdw256_mask (__v8si __A,__v8hi setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovsdw128_mask (__v4si __A, __v8hi setzero, __mmask8 minus_one);
void  __builtin_ia32_pmovsdw128mem_mask (__v8hi * __P, __v4si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsdw128_mask (__v4si __A, __v8hi __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsdw128_mask (__v4si __A, __v8hi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovsdw256_mask (__v8si __A, __v8hi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsdw256mem_mask (__v8hi * __P, __v8si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsdw256_mask (__v8si __A, __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsdw256_mask (__v8si __A, __v8hi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovusdw128_mask (__v4si __A,  __v8hi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusdw128mem_mask (__v8hi * __P, __v4si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusdw128_mask (__v4si __A,  __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovusdw128_mask (__v4si __A,  __v8hi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovusdw256_mask (__v8si __A,  __v8hi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusdw256mem_mask (__v8hi * __P, __v8si __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusdw256_mask (__v8si __A,  __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovusdw256_mask (__v8si __A,  __v8hi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovqb128_mask (__v2di  __A,__v16qi undefined,__mmask8 minus_one);
void  __builtin_ia32_pmovqb128mem_mask (__v16qi * __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovqb128_mask (__v2di  __A,__v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovqb128_mask (__v2di  __A,__v16qi setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovqb256_mask (__v4di  __A,__v16qi undefined,__mmask8 minus_one);
void  __builtin_ia32_pmovqb256mem_mask (__v16qi * __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovqb256_mask (__v4di  __A,__v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovqb256_mask (__v4di  __A,__v16qi setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovsqb128_mask (__v2di  __A, __v16qi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsqb128mem_mask (__v16qi * __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsqb128_mask (__v2di  __A, __v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsqb128_mask (__v2di  __A, __v16qi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovsqb256_mask (__v4di  __A, __v16qi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsqb256mem_mask (__v16qi * __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsqb256_mask (__v4di  __A, __v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsqb256_mask (__v4di  __A, __v16qi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovusqb128_mask (__v2di  __A,  __v16qi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusqb128mem_mask (__v16qi * __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusqb128_mask (__v2di  __A,  __v16qi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pmovusqb128_mask (__v2di  __A,  __v16qi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovusqb256_mask (__v4di  __A,  __v16qi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusqb256mem_mask (__v16qi * __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusqb256_mask (__v4di  __A,  __v16qi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pmovusqb256_mask (__v4di  __A,  __v16qi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovqw128_mask (__v2di  __A,__v8hi undefined,__mmask8 minus_one);
void  __builtin_ia32_pmovqw128mem_mask (__v8hi * __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovqw128_mask (__v2di  __A,__v8hi __O,__mmask8 __M);
__m128i __builtin_ia32_pmovqw128_mask (__v2di  __A,__v8hi setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovqw256_mask (__v4di  __A,__v8hi undefined,__mmask8 minus_one);
void  __builtin_ia32_pmovqw256mem_mask (__v8hi * __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovqw256_mask (__v4di  __A,__v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovqw256_mask (__v4di  __A,__v8hi setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovsqw128_mask (__v2di  __A, __v8hi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsqw128mem_mask (__v8hi * __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsqw128_mask (__v2di  __A, __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsqw128_mask (__v2di  __A, __v8hi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovsqw256_mask (__v4di  __A, __v8hi undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsqw256mem_mask (__v8hi * __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsqw256_mask (__v4di  __A, __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsqw256_mask (__v4di  __A, __v8hi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovusqw128_mask (__v2di  __A,  __v8hi undefined,  __mmask8 minus_one);
void __builtin_ia32_pmovusqw128mem_mask (__v8hi * __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusqw128_mask (__v2di  __A,  __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovusqw128_mask (__v2di  __A,  __v8hi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovusqw256_mask (__v4di  __A,  __v8hi undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusqw256mem_mask (__v8hi * __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusqw256_mask (__v4di  __A,  __v8hi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovusqw256_mask (__v4di  __A,  __v8hi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovqd128_mask (__v2di  __A,__v4si undefined,__mmask8 minus_one);
void  __builtin_ia32_pmovqd128mem_mask (__v4si *  __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovqd128_mask (__v2di  __A,__v4si __O, __mmask8 __M);
__m128i __builtin_ia32_pmovqd128_mask (__v2di  __A,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovqd256_mask (__v4di  __A,__v4si undefined,__mmask8 minus_one);
void  __builtin_ia32_pmovqd256mem_mask (__v4si *  __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovqd256_mask (__v4di  __A,__v4si __O, __mmask8 __M);
__m128i __builtin_ia32_pmovqd256_mask (__v4di  __A,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pmovsqd128_mask (__v2di  __A, __v4si undefined, __mmask8 minus_one);
void __builtin_ia32_pmovsqd128mem_mask (__v4si *  __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsqd128_mask (__v2di  __A, __v4si __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsqd128_mask (__v2di  __A, __v4si setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovsqd256_mask (__v4di  __A, __v4si undefined, __mmask8 minus_one);
void  __builtin_ia32_pmovsqd256mem_mask (__v4si *  __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovsqd256_mask (__v4di  __A, __v4si __O, __mmask8 __M);
__m128i __builtin_ia32_pmovsqd256_mask (__v4di  __A, __v4si setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovusqd128_mask (__v2di  __A,  __v4si undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusqd128mem_mask (__v4si *  __P, __v2di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusqd128_mask (__v2di  __A,  __v4si __O, __mmask8 __M);
__m128i __builtin_ia32_pmovusqd128_mask (__v2di  __A,  __v4si setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovusqd256_mask (__v4di  __A,  __v4si undefined,  __mmask8 minus_one);
void  __builtin_ia32_pmovusqd256mem_mask (__v4si *  __P, __v4di  __A, __mmask8 __M);
__m128i __builtin_ia32_pmovusqd256_mask (__v4di  __A,  __v4si __O, __mmask8 __M);
__m128i __builtin_ia32_pmovusqd256_mask (__v4di  __A,  __v4si setzero,  __mmask8 __M);
__m256 __builtin_ia32_broadcastss256_mask (__v4sf __A, __v8sf __O, __mmask8 __M);
__m256 __builtin_ia32_broadcastss256_mask (__v4sf __A, __v8sf setzero, __mmask8 __M);
__m128 __builtin_ia32_broadcastss128_mask (__v4sf __A, __v4sf __O, __mmask8 __M);
__m128 __builtin_ia32_broadcastss128_mask (__v4sf __A, __v4sf setzero, __mmask8 __M);
__m256d __builtin_ia32_broadcastsd256_mask (__v2df __A,  __v4df __O,  __mmask8 __M);
__m256d __builtin_ia32_broadcastsd256_mask (__v2df __A,  __v4df setzero,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastd256_mask (__v4si __A,  __v8si __O,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastd256_mask (__v4si __A,  __v8si setzero,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastd256_gpr_mask (int __A, __v8si __O,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastd256_gpr_mask (int __A,  __v8si setzero,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastd128_mask (__v4si __A,  __v4si __O,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastd128_mask (__v4si __A,  __v4si setzero,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastd128_gpr_mask (int __A, __v4si __O,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastd128_gpr_mask (int __A,  __v4si setzero,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastq256_mask (__v2di  __A,  __v4di  __O,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastq256_mask (__v2di  __A,  __v4di setzero,  __mmask8 __M);
__m256i __builtin_ia32_pbroadcastq256_gpr_mask (long long __A, __v4di  __O, __mmask8 __M);
__m256i __builtin_ia32_pbroadcastq256_gpr_mask (long long __A, __v4di setzero, __mmask8 __M);
__m128i __builtin_ia32_pbroadcastq128_mask (__v2di  __A,  __v2di  __O,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastq128_mask (__v2di  __A,  __v2di setzero,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastq128_gpr_mask (long long __A, __v2di  __O, __mmask8 __M);
__m128i __builtin_ia32_pbroadcastq128_gpr_mask (long long __A, __v2di setzero, __mmask8 __M);
__m256 __builtin_ia32_broadcastf32x4_256_mask (__v4sf __A, __v8sf undefined, __mmask8 minus_one);
__m256 __builtin_ia32_broadcastf32x4_256_mask (__v4sf __A, __v8sf __O, __mmask8 __M);
__m256 __builtin_ia32_broadcastf32x4_256_mask (__v4sf __A, __v8sf setzero, __mmask8 __M);
__m256i __builtin_ia32_broadcasti32x4_256_mask (__v4si __A,__v8si undefined,  __mmask8 minus_one);
__m256i __builtin_ia32_broadcasti32x4_256_mask (__v4si __A,  __v8si __O, __mmask8 __M);
__m256i __builtin_ia32_broadcasti32x4_256_mask (__v4si __A,  __v8si setzero,  __mmask8 __M);
__m256i __builtin_ia32_pmovsxbd256_mask (__v16qi  __A,  __v8si __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxbd256_mask (__v16qi  __A,  __v8si setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxbd128_mask (__v16qi  __A,  __v4si __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxbd128_mask (__v16qi  __A,  __v4si setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxbq256_mask (__v16qi  __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxbq256_mask (__v16qi  __A,  __v4di  setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxbq128_mask (__v16qi  __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxbq128_mask (__v16qi  __A,  __v2di  setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxwd256_mask (__v8hi  __A,  __v8si __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxwd256_mask (__v8hi  __A,  __v8si setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxwd128_mask (__v8hi  __A,  __v4si __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxwd128_mask (__v8hi  __A,  __v4si setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxwq256_mask (__v8hi  __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxwq256_mask (__v8hi  __A,  __v4di  setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxwq128_mask (__v8hi  __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxwq128_mask (__v8hi  __A,  __v2di  setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxdq256_mask (__v4si __X,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovsxdq256_mask (__v4si __X,  __v4di  setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxdq128_mask (__v4si __X,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxdq128_mask (__v4si __X,  __v2di  setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxbd256_mask (__v16qi  __A,  __v8si __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxbd256_mask (__v16qi  __A,  __v8si setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxbd128_mask (__v16qi  __A,  __v4si __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxbd128_mask (__v16qi  __A,  __v4si setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxbq256_mask (__v16qi  __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxbq256_mask (__v16qi  __A,  __v4di  setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxbq128_mask (__v16qi  __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxbq128_mask (__v16qi  __A,  __v2di  setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxwd256_mask (__v8hi  __A,  __v8si __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxwd256_mask (__v8hi  __A,  __v8si setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxwd128_mask (__v8hi  __A,  __v4si __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxwd128_mask (__v8hi  __A,  __v4si setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxwq256_mask (__v8hi  __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxwq256_mask (__v8hi  __A,  __v4di  setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxwq128_mask (__v8hi  __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxwq128_mask (__v8hi  __A,  __v2di  setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxdq256_mask (__v4si __X,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxdq256_mask (__v4si __X,  __v4di  setzero,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxdq128_mask (__v4si __X,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxdq128_mask (__v4si __X,  __v2di  setzero,  __mmask8 __U);
__m256d __builtin_ia32_rcp14pd256_mask (__v4df __A,__v4df  setzero,__mmask8 minus_one);
__m256d __builtin_ia32_rcp14pd256_mask (__v4df __A,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_rcp14pd256_mask (__v4df __A,__v4df  setzero,__mmask8 __U);
__m128d __builtin_ia32_rcp14pd128_mask (__v2df __A,__v2df  setzero,__mmask8 minus_one);
__m128d __builtin_ia32_rcp14pd128_mask (__v2df __A,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_rcp14pd128_mask (__v2df __A,__v2df  setzero,__mmask8 __U);
__m256 __builtin_ia32_rcp14ps256_mask (__v8sf __A,  __v8sf setzero,  __mmask8 minus_one);
__m256 __builtin_ia32_rcp14ps256_mask (__v8sf __A,  __v8sf __W,  __mmask8 __U);
__m256 __builtin_ia32_rcp14ps256_mask (__v8sf __A,  __v8sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_rcp14ps128_mask (__v4sf __A,  __v4sf setzero,  __mmask8 minus_one);
__m128 __builtin_ia32_rcp14ps128_mask (__v4sf __A,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_rcp14ps128_mask (__v4sf __A,  __v4sf setzero,  __mmask8 __U);
__m256d __builtin_ia32_rsqrt14pd256_mask (__v4df __A,__v4df  setzero,__mmask8 minus_one);
__m256d __builtin_ia32_rsqrt14pd256_mask (__v4df __A,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_rsqrt14pd256_mask (__v4df __A,__v4df  setzero,__mmask8 __U);
__m128d __builtin_ia32_rsqrt14pd128_mask (__v2df __A,__v2df  setzero,__mmask8 minus_one);
__m128d __builtin_ia32_rsqrt14pd128_mask (__v2df __A,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_rsqrt14pd128_mask (__v2df __A,__v2df  setzero,__mmask8 __U);
__m256 __builtin_ia32_rsqrt14ps256_mask (__v8sf __A,  __v8sf setzero,  __mmask8 minus_one);
__m256 __builtin_ia32_rsqrt14ps256_mask (__v8sf __A,  __v8sf __W,  __mmask8 __U);
__m256 __builtin_ia32_rsqrt14ps256_mask (__v8sf __A,  __v8sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_rsqrt14ps128_mask (__v4sf __A,  __v4sf setzero,  __mmask8 minus_one);
__m128 __builtin_ia32_rsqrt14ps128_mask (__v4sf __A,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_rsqrt14ps128_mask (__v4sf __A,  __v4sf setzero,  __mmask8 __U);
__m256d __builtin_ia32_sqrtpd256_mask (__v4df __A,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_sqrtpd256_mask (__v4df __A,__v4df setzero,__mmask8 __U);
__m128d __builtin_ia32_sqrtpd128_mask (__v2df __A,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_sqrtpd128_mask (__v2df __A,__v2df setzero,__mmask8 __U);
__m256 __builtin_ia32_sqrtps256_mask (__v8sf __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_sqrtps256_mask (__v8sf __A, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_sqrtps128_mask (__v4sf __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_sqrtps128_mask (__v4sf __A, __v4sf setzero, __mmask8 __U);
__m256i __builtin_ia32_paddd256_mask (__v8si __A, __v8si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_paddd256_mask (__v8si __A, __v8si __B, __v8si setzero, __mmask8 __U);
__m256i __builtin_ia32_paddq256_mask (__v4di  __A, __v4di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_paddq256_mask (__v4di  __A, __v4di  __B, __v4di setzero, __mmask8 __U);
__m256i __builtin_ia32_psubd256_mask (__v8si __A, __v8si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_psubd256_mask (__v8si __A, __v8si __B, __v8si setzero, __mmask8 __U);
__m256i __builtin_ia32_psubq256_mask (__v4di  __A, __v4di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_psubq256_mask (__v4di  __A, __v4di  __B, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_paddd128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_paddd128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_paddq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_paddq128_mask (__v2di  __A, __v2di  __B, __v2di setzero, __mmask8 __U);
__m128i __builtin_ia32_psubd128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_psubd128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_psubq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_psubq128_mask (__v2di  __A, __v2di  __B, __v2di setzero, __mmask8 __U);
__m256 __builtin_ia32_getexpps256_mask (__v8sf __A, __v8sf setzero, __mmask8 minus_one);
__m256 __builtin_ia32_getexpps256_mask (__v8sf __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_getexpps256_mask (__v8sf __A, __v8sf setzero, __mmask8 __U);
__m256d __builtin_ia32_getexppd256_mask (__v4df __A,  __v4df setzero,  __mmask8 minus_one);
__m256d __builtin_ia32_getexppd256_mask (__v4df __A,  __v4df __W,  __mmask8 __U);
__m256d __builtin_ia32_getexppd256_mask (__v4df __A,  __v4df setzero,  __mmask8 __U);
__m128 __builtin_ia32_getexpps128_mask (__v4sf __A, __v4sf setzero, __mmask8 minus_one);
__m128 __builtin_ia32_getexpps128_mask (__v4sf __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_getexpps128_mask (__v4sf __A, __v4sf  setzero, __mmask8 __U);
__m128d __builtin_ia32_getexppd128_mask (__v2df __A,  __v2df setzero,  __mmask8 minus_one);
__m128d __builtin_ia32_getexppd128_mask (__v2df __A,  __v2df __W,  __mmask8 __U);
__m128d __builtin_ia32_getexppd128_mask (__v2df __A,  __v2df setzero,  __mmask8 __U);
__m256i __builtin_ia32_psrld256_mask (__v8si __A, __v4si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_psrld256_mask (__v8si __A, __v4si __B, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_psrld128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_psrld128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m256i __builtin_ia32_psrlq256_mask (__v4di  __A, __v2di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_psrlq256_mask (__v4di  __A, __v2di  __B, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_psrlq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_psrlq128_mask (__v2di  __A, __v2di  __B, __v2di setzero, __mmask8 __U);
__m256i __builtin_ia32_pandd256_mask (__v8si __A, __v8si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_pandd256_mask (__v8si __A, __v8si __B, __v8si setzero, __mmask8 __U);
__m256d __builtin_ia32_scalefpd256_mask (__v4df __A,  __v4df __B,  __v4df setzero,  __mmask8 minus_one);
__m256d __builtin_ia32_scalefpd256_mask (__v4df __A,  __v4df __B,  __v4df __W,  __mmask8 __U);
__m256d __builtin_ia32_scalefpd256_mask (__v4df __A,  __v4df __B,  __v4df setzero,  __mmask8 __U);
__m256 __builtin_ia32_scalefps256_mask (__v8sf __A, __v8sf __B, __v8sf setzero, __mmask8 minus_one);
__m256 __builtin_ia32_scalefps256_mask (__v8sf __A, __v8sf __B, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_scalefps256_mask (__v8sf __A, __v8sf __B, __v8sf setzero, __mmask8 __U);
__m128d __builtin_ia32_scalefpd128_mask (__v2df __A,  __v2df __B,  __v2df setzero,  __mmask8 minus_one);
__m128d __builtin_ia32_scalefpd128_mask (__v2df __A,  __v2df __B,  __v2df __W,  __mmask8 __U);
__m128d __builtin_ia32_scalefpd128_mask (__v2df __A,  __v2df __B,  __v2df setzero,  __mmask8 __U);
__m128 __builtin_ia32_scalefps128_mask (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 minus_one);
__m128 __builtin_ia32_scalefps128_mask (__v4sf __A, __v4sf __B, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_scalefps128_mask (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_mask (__v4df __A,  __v4df __B,  __v4df __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_mask3 (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_mask (__v2df __A,  __v2df __B,  __v2df __C,  __mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_mask3 (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_mask (__v8sf __A, __v8sf __B, __v8sf __C, __mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_mask3 (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_mask (__v4sf __A, __v4sf __B, __v4sf __C, __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_mask3 (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_mask (__v4df __A,  __v4df __B,  __v4df __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmsubpd256_mask3 (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_mask (__v2df __A,  __v2df __B,  __v2df __C,  __mmask8 __U);
__m128d __builtin_ia32_vfmsubpd128_mask3 (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_mask (__v8sf __A, __v8sf __B, __v8sf __C, __mmask8 __U);
__m256 __builtin_ia32_vfmsubps256_mask3 (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_mask (__v4sf __A, __v4sf __B, __v4sf __C, __mmask8 __U);
__m128 __builtin_ia32_vfmsubps128_mask3 (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmaddsubpd256_mask (__v4df __A,  __v4df __B,  __v4df __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmaddsubpd256_mask3 (__v4df __A, __v4df __B, __v4df __C, __mmask8  __U);
__m256d __builtin_ia32_vfmaddsubpd256_maskz (__v4df __A, __v4df __B, __v4df __C, __mmask8 __U);
__m128d __builtin_ia32_vfmaddsubpd128_mask (__v2df __A,  __v2df __B,  __v2df __C,  __mmask8 __U);
__m128d __builtin_ia32_vfmaddsubpd128_mask3 (__v2df __A, __v2df __B, __v2df __C, __mmask8 __U);
__m128d __builtin_ia32_vfmaddsubpd128_maskz (__v2df __A, __v2df __B, __v2df __C, __mmask8  __U);
__m256 __builtin_ia32_vfmaddsubps256_mask (__v8sf __A, __v8sf __B, __v8sf __C, __mmask8 __U);
__m256 __builtin_ia32_vfmaddsubps256_mask3 (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfmaddsubps256_maskz (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddsubps128_mask (__v4sf __A, __v4sf __B, __v4sf __C, __mmask8 __U);
__m128 __builtin_ia32_vfmaddsubps128_mask3 (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddsubps128_maskz (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmaddsubpd256_mask (__v4df __A,  __v4df __B,  __v4df __C,  __mmask8 __U);
__m256d __builtin_ia32_vfmsubaddpd256_mask3 (__v4df __A, __v4df __B, __v4df __C, __mmask8  __U);
__m256d __builtin_ia32_vfmaddsubpd256_maskz (__v4df __A, __v4df __B, __v4df __C, __mmask8 __U);
__m128d __builtin_ia32_vfmaddsubpd128_mask (__v2df __A,  __v2df __B,  __v2df __C,  __mmask8 __U);
__m128d __builtin_ia32_vfmsubaddpd128_mask3 (__v2df __A, __v2df __B, __v2df __C, __mmask8 __U);
__m128d __builtin_ia32_vfmaddsubpd128_maskz (__v2df __A, __v2df __B, __v2df __C, __mmask8 __U);
__m256 __builtin_ia32_vfmaddsubps256_mask (__v8sf __A, __v8sf __B, __v8sf __C, __mmask8 __U);
__m256 __builtin_ia32_vfmsubaddps256_mask3 (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfmaddsubps256_maskz (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddsubps128_mask (__v4sf __A, __v4sf __B, __v4sf __C, __mmask8 __U);
__m128 __builtin_ia32_vfmsubaddps128_mask3 (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddsubps128_maskz (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m256d __builtin_ia32_vfnmaddpd256_mask (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_mask3 (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m128d __builtin_ia32_vfnmaddpd128_mask (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_mask3 (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m256 __builtin_ia32_vfnmaddps256_mask (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_mask3 (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfnmaddps128_mask (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_mask3 (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m256d __builtin_ia32_vfnmsubpd256_mask (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m256d __builtin_ia32_vfnmsubpd256_mask3 (__v4df __A, __v4df __B, __v4df __C, __mmask8 __U);
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df __A,__v4df __B,__v4df __C,__mmask8 __U);
__m128d __builtin_ia32_vfnmsubpd128_mask (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m128d __builtin_ia32_vfnmsubpd128_mask3 (__v2df __A, __v2df __B, __v2df __C, __mmask8 __U);
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df __A,__v2df __B,__v2df __C,__mmask8 __U);
__m256 __builtin_ia32_vfnmsubps256_mask (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m256 __builtin_ia32_vfnmsubps256_mask3 (__v8sf __A,__v8sf __B,__v8sf __C,__mmask8 __U);
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf __A,  __v8sf __B,  __v8sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfnmsubps128_mask (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128 __builtin_ia32_vfnmsubps128_mask3 (__v4sf __A,__v4sf __B,__v4sf __C,__mmask8 __U);
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf __A,  __v4sf __B,  __v4sf __C,  __mmask8 __U);
__m128i __builtin_ia32_pandd128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_pandd128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m256i __builtin_ia32_pandnd256_mask (__v8si __A,__v8si __B,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_pandnd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_pandnd128_mask (__v4si __A,__v4si __B,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_pandnd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __U);
__m256i __builtin_ia32_pord256_mask (__v8si __A,__v8si __B,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_pord256_mask (__v8si __A,__v8si __B,__v8si  setzero,__mmask8 __U);
__m128i __builtin_ia32_pord128_mask (__v4si __A,__v4si __B,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_pord128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __U);
__m256i __builtin_ia32_pxord256_mask (__v8si __A, __v8si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_pxord256_mask (__v8si __A, __v8si __B, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_pxord128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_pxord128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m128 __builtin_ia32_cvtpd2ps_mask (__v2df __A,__v4sf __W,__mmask8 __U);
__m128 __builtin_ia32_cvtpd2ps_mask (__v2df __A,__v4sf setzero,__mmask8 __U);
__m128 __builtin_ia32_cvtpd2ps256_mask (__v4df __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_cvtpd2ps256_mask (__v4df __A, __v4sf setzero, __mmask8 __U);
__m256i __builtin_ia32_cvtps2dq256_mask (__v8sf __A,  __v8si __W,  __mmask8 __U);
__m256i __builtin_ia32_cvtps2dq256_mask (__v8sf __A,  __v8si setzero,  __mmask8 __U);
__m128i __builtin_ia32_cvtps2dq128_mask (__v4sf __A,  __v4si __W,  __mmask8 __U);
__m128i __builtin_ia32_cvtps2dq128_mask (__v4sf __A,  __v4si setzero,  __mmask8 __U);
__m256i __builtin_ia32_cvtps2udq256_mask (__v8sf __A,__v8si  setzero,__mmask8 minus_one);
__m256i __builtin_ia32_cvtps2udq256_mask (__v8sf __A,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_cvtps2udq256_mask (__v8sf __A,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_cvtps2udq128_mask (__v4sf __A,__v4si setzero,__mmask8 minus_one);
__m128i __builtin_ia32_cvtps2udq128_mask (__v4sf __A,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_cvtps2udq128_mask (__v4sf __A,__v4si setzero,__mmask8 __U);
__m256d __builtin_ia32_movddup256_mask (__v4df __A, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_movddup256_mask (__v4df __A, __v4df setzero, __mmask8 __U);
__m128d __builtin_ia32_movddup128_mask (__v2df __A, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_movddup128_mask (__v2df __A, __v2df setzero, __mmask8 __U);
__m256 __builtin_ia32_movshdup256_mask (__v8sf __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_movshdup256_mask (__v8sf __A, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_movshdup128_mask (__v4sf __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_movshdup128_mask (__v4sf __A, __v4sf setzero, __mmask8 __U);
__m256 __builtin_ia32_movsldup256_mask (__v8sf __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_movsldup256_mask (__v8sf __A, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_movsldup128_mask (__v4sf __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_movsldup128_mask (__v4sf __A, __v4sf setzero, __mmask8 __U);
__m128i __builtin_ia32_punpckhdq128_mask (__v4si __A,__v4si __B,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_punpckhdq128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __U);
__m256i __builtin_ia32_punpckhdq256_mask (__v8si __A,__v8si __B,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_punpckhdq256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_punpckhqdq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_punpckhqdq128_mask (__v2di  __A, __v2di  __B, __v2di  setzero, __mmask8 __U);
__m256i __builtin_ia32_punpckhqdq256_mask (__v4di  __A, __v4di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_punpckhqdq256_mask (__v4di  __A, __v4di  __B, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_punpckldq128_mask (__v4si __A,__v4si __B,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_punpckldq128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __U);
__m256i __builtin_ia32_punpckldq256_mask (__v8si __A,__v8si __B,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_punpckldq256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_punpcklqdq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_punpcklqdq128_mask (__v2di  __A, __v2di  __B, __v2di  setzero, __mmask8 __U);
__m256i __builtin_ia32_punpcklqdq256_mask (__v4di  __A, __v4di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_punpcklqdq256_mask (__v4di  __A, __v4di  __B, __v4di setzero, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si __A, __v4si __B, unsigned int zero, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpeqd128_mask (__v4si __A,  __v4si __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si __A, __v4si __B, unsigned int zero, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpeqd128_mask (__v4si __A,  __v4si __B,__mmask8  __U);
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si __A, __v8si __B, unsigned int zero, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpeqd256_mask (__v8si __A,  __v8si __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si __A, __v8si __B, unsigned int zero, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpeqd256_mask (__v8si __A,  __v8si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di  __A, __v2di  __B, unsigned int zero, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpeqq128_mask (__v2di  __A,  __v2di  __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di  __A, __v2di  __B, unsigned int zero, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpeqq128_mask (__v2di  __A,  __v2di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di  __A, __v4di  __B, unsigned int zero, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpeqq256_mask (__v4di  __A,  __v4di  __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di  __A, __v4di  __B, unsigned int zero,__mmask8  __U);
__mmask8 __builtin_ia32_pcmpeqq256_mask (__v4di  __A,  __v4di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si __A, __v4si __B, unsigned int six, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpgtd128_mask (__v4si __A,  __v4si __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si __A, __v4si __B, unsigned int six, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpgtd128_mask (__v4si __A,  __v4si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si __A, __v8si __B, unsigned int six, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpgtd256_mask (__v8si __A,  __v8si __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si __A, __v8si __B, unsigned int six, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpgtd256_mask (__v8si __A,  __v8si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di  __A, __v2di  __B, unsigned int six, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpgtq128_mask (__v2di  __A,  __v2di  __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di  __A, __v2di  __B, unsigned int six, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpgtq128_mask (__v2di  __A,  __v2di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di  __A, __v4di  __B, unsigned int six, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpgtq256_mask (__v4di  __A,  __v4di  __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di  __A, __v4di  __B, unsigned int six, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpgtq256_mask (__v4di  __A,  __v4di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestmd128 (__v4si __A, __v4si __B, __mmask8 minus_one);
__mmask8 __builtin_ia32_ptestmd128 (__v4si __A, __v4si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestmd256 (__v8si __A, __v8si __B, __mmask8 minus_one);
__mmask8 __builtin_ia32_ptestmd256 (__v8si __A, __v8si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestmq128 (__v2di  __A, __v2di  __B, __mmask8 minus_one);
__mmask8 __builtin_ia32_ptestmq128 (__v2di  __A, __v2di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestmq256 (__v4di  __A, __v4di  __B, __mmask8 minus_one);
__mmask8 __builtin_ia32_ptestmq256 (__v4di  __A, __v4di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestnmd128 (__v4si __A,__v4si __B,__mmask8 minus_one);
__mmask8 __builtin_ia32_ptestnmd128 (__v4si __A,__v4si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestnmd256 (__v8si __A,__v8si __B,__mmask8 minus_one);
__mmask8 __builtin_ia32_ptestnmd256 (__v8si __A,__v8si __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestnmq128 (__v2di  __A,__v2di  __B,__mmask8 minus_one);
__mmask8 __builtin_ia32_ptestnmq128 (__v2di  __A,__v2di  __B, __mmask8 __U);
__mmask8 __builtin_ia32_ptestnmq256 (__v4di  __A,__v4di  __B,__mmask8 minus_one);
__mmask8 __builtin_ia32_ptestnmq256 (__v4di  __A,__v4di  __B, __mmask8 __U);
__m256d __builtin_ia32_compressdf256_mask (__v4df __A, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_compressdf256_mask (__v4df __A, __v4df setzero, __mmask8 __U);
void  __builtin_ia32_compressstoredf256_mask (__v4df * __P, __v4df __A, __mmask8 __U);
__m128d __builtin_ia32_compressdf128_mask (__v2df __A, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_compressdf128_mask (__v2df __A, __v2df setzero, __mmask8 __U);
void  __builtin_ia32_compressstoredf128_mask (__v2df * __P, __v2df __A, __mmask8 __U);
__m256 __builtin_ia32_compresssf256_mask (__v8sf __A,__v8sf __W,__mmask8 __U);
__m256 __builtin_ia32_compresssf256_mask (__v8sf __A,__v8sf setzero,__mmask8 __U);
void  __builtin_ia32_compressstoresf256_mask (__v8sf * __P, __v8sf __A, __mmask8 __U);
__m128 __builtin_ia32_compresssf128_mask (__v4sf __A,__v4sf __W,__mmask8 __U);
__m128 __builtin_ia32_compresssf128_mask (__v4sf __A,__v4sf setzero,__mmask8 __U);
void  __builtin_ia32_compressstoresf128_mask (__v4sf * __P, __v4sf __A, __mmask8 __U);
__m256i __builtin_ia32_compressdi256_mask (__v4di  __A, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_compressdi256_mask (__v4di  __A, __v4di setzero, __mmask8 __U);
void  __builtin_ia32_compressstoredi256_mask (__v4di * __P, __v4di  __A, __mmask8 __U);
__m128i __builtin_ia32_compressdi128_mask (__v2di  __A, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_compressdi128_mask (__v2di  __A, __v2di  setzero, __mmask8 __U);
void  __builtin_ia32_compressstoredi128_mask (__v2di * __P, __v2di  __A, __mmask8 __U);
__m256i __builtin_ia32_compresssi256_mask (__v8si __A, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_compresssi256_mask (__v8si __A, __v8si setzero, __mmask8 __U);
void  __builtin_ia32_compressstoresi256_mask (__v8si * __P, __v8si __A, __mmask8 __U);
__m128i __builtin_ia32_compresssi128_mask (__v4si __A, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_compresssi128_mask (__v4si __A, __v4si setzero, __mmask8 __U);
void  __builtin_ia32_compressstoresi128_mask (__v4si *  __P, __v4si __A, __mmask8 __U);
__m256d __builtin_ia32_expanddf256_mask (__v4df __A,  __v4df __W,  __mmask8 __U);
__m256d __builtin_ia32_expanddf256_maskz (__v4df __A,__v4df setzero,__mmask8 __U);
__m256d __builtin_ia32_expandloaddf256_mask (__v4df * __P, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_expandloaddf256_maskz (__v4df * __P,__v4df setzero,__mmask8 __U);
__m128d __builtin_ia32_expanddf128_mask (__v2df __A,  __v2df __W,  __mmask8 __U);
__m128d __builtin_ia32_expanddf128_maskz (__v2df __A,__v2df setzero,__mmask8 __U);
__m128d __builtin_ia32_expandloaddf128_mask (__v2df * __P, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_expandloaddf128_maskz (__v2df * __P,__v2df setzero,__mmask8 __U);
__m256 __builtin_ia32_expandsf256_mask (__v8sf __A, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_expandsf256_maskz (__v8sf __A,  __v8sf setzero,  __mmask8 __U);
__m256 __builtin_ia32_expandloadsf256_mask (__v8sf * __P,  __v8sf __W,  __mmask8 __U);
__m256 __builtin_ia32_expandloadsf256_maskz (__v8sf * __P, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_expandsf128_mask (__v4sf __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_expandsf128_maskz (__v4sf __A,  __v4sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_expandloadsf128_mask (__v4sf * __P,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_expandloadsf128_maskz (__v4sf * __P, __v4sf setzero, __mmask8 __U);
__m256i __builtin_ia32_expanddi256_mask (__v4di  __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_expanddi256_maskz (__v4di  __A,__v4di setzero,__mmask8 __U);
__m256i __builtin_ia32_expandloaddi256_mask (__v4di * __P, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_expandloaddi256_maskz (__v4di * __P,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_expanddi128_mask (__v2di  __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_expanddi128_maskz (__v2di  __A,__v2di setzero,__mmask8 __U);
__m128i __builtin_ia32_expandloaddi128_mask (__v2di * __P, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_expandloaddi128_maskz (__v2di * __P,__v2di setzero,__mmask8 __U);
__m256i __builtin_ia32_expandsi256_mask (__v8si __A,  __v8si __W,  __mmask8 __U);
__m256i __builtin_ia32_expandsi256_maskz (__v8si __A,__v8si setzero,__mmask8 __U);
__m256i __builtin_ia32_expandloadsi256_mask (__v8si * __P, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_expandloadsi256_maskz (__v8si * __P,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_expandsi128_mask (__v4si __A,  __v4si __W,  __mmask8 __U);
__m128i __builtin_ia32_expandsi128_maskz (__v4si __A,__v4si setzero,__mmask8 __U);
__m128i __builtin_ia32_expandloadsi128_mask (__v4si *  __P, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_expandloadsi128_maskz (__v4si * __P,__v4si setzero,__mmask8 __U);
__m256d __builtin_ia32_vpermt2varpd256_mask (__v4di  __I, __v4df __A, __v4df __B, __mmask8 minus_one);
__m256d __builtin_ia32_vpermt2varpd256_mask (__v4di  __I, __v4df __A, __v4df __B, __mmask8 __U);
__m256d __builtin_ia32_vpermi2varpd256_mask (__v4df __A, __v4di  __I, __v4df __B, __mmask8 __U);
__m256d __builtin_ia32_vpermt2varpd256_maskz (__v4di  __I,__v4df __A,__v4df __B,__mmask8 __U);
__m256 __builtin_ia32_vpermt2varps256_mask (__v8si __I,  __v8sf __A,  __v8sf __B,  __mmask8 minus_one);
__m256 __builtin_ia32_vpermt2varps256_mask (__v8si __I,  __v8sf __A,  __v8sf __B,  __mmask8 __U);
__m256 __builtin_ia32_vpermi2varps256_mask (__v8sf __A,  __v8si __I,  __v8sf __B,  __mmask8 __U);
__m256 __builtin_ia32_vpermt2varps256_maskz (__v8si __I, __v8sf __A, __v8sf __B, __mmask8 __U);
__m128i __builtin_ia32_vpermt2varq128_mask (__v2di  __I,  __v2di  __A,  __v2di  __B,  __mmask8 minus_one);
__m128i __builtin_ia32_vpermt2varq128_mask (__v2di  __I,  __v2di  __A,  __v2di  __B,  __mmask8 __U);
__m128i __builtin_ia32_vpermi2varq128_mask (__v2di  __A,  __v2di  __I,  __v2di  __B,  __mmask8 __U);
__m128i __builtin_ia32_vpermt2varq128_maskz (__v2di  __I, __v2di  __A, __v2di  __B, __mmask8 __U);
__m128i __builtin_ia32_vpermt2vard128_mask (__v4si __I,  __v4si __A,  __v4si __B,  __mmask8 minus_one);
__m128i __builtin_ia32_vpermt2vard128_mask (__v4si __I,  __v4si __A,  __v4si __B,  __mmask8 __U);
__m128i __builtin_ia32_vpermi2vard128_mask (__v4si __A,  __v4si __I,  __v4si __B,  __mmask8 __U);
__m128i __builtin_ia32_vpermt2vard128_maskz (__v4si __I, __v4si __A, __v4si __B, __mmask8 __U);
__m256i __builtin_ia32_vpermt2varq256_mask (__v4di  __I,  __v4di  __A,  __v4di  __B,  __mmask8 minus_one);
__m256i __builtin_ia32_vpermt2varq256_mask (__v4di  __I,  __v4di  __A,  __v4di  __B,  __mmask8 __U);
__m256i __builtin_ia32_vpermi2varq256_mask (__v4di  __A,  __v4di  __I,  __v4di  __B,  __mmask8 __U);
__m256i __builtin_ia32_vpermt2varq256_maskz (__v4di  __I, __v4di  __A, __v4di  __B, __mmask8 __U);
__m256i __builtin_ia32_vpermt2vard256_mask (__v8si __I,  __v8si __A,  __v8si __B,  __mmask8 minus_one);
__m256i __builtin_ia32_vpermt2vard256_mask (__v8si __I,  __v8si __A,  __v8si __B,  __mmask8 __U);
__m256i __builtin_ia32_vpermi2vard256_mask (__v8si __A,  __v8si __I,  __v8si __B,  __mmask8 __U);
__m256i __builtin_ia32_vpermt2vard256_maskz (__v8si __I, __v8si __A, __v8si __B, __mmask8 __U);
__m128d __builtin_ia32_vpermt2varpd128_mask (__v2di  __I, __v2df __A, __v2df __B, __mmask8 minus_one);
__m128d __builtin_ia32_vpermt2varpd128_mask (__v2di  __I, __v2df __A, __v2df __B, __mmask8 __U);
__m128d __builtin_ia32_vpermi2varpd128_mask (__v2df __A, __v2di  __I, __v2df __B, __mmask8 __U);
__m128d __builtin_ia32_vpermt2varpd128_maskz (__v2di  __I,__v2df __A,__v2df __B,__mmask8 __U);
__m128 __builtin_ia32_vpermt2varps128_mask (__v4si __I,  __v4sf __A,  __v4sf __B,  __mmask8 minus_one);
__m128 __builtin_ia32_vpermt2varps128_mask (__v4si __I,  __v4sf __A,  __v4sf __B,  __mmask8 __U);
__m128 __builtin_ia32_vpermi2varps128_mask (__v4sf __A,  __v4si __I,  __v4sf __B,  __mmask8 __U);
__m128 __builtin_ia32_vpermt2varps128_maskz (__v4si __I, __v4sf __A, __v4sf __B, __mmask8 __U);
__m128i __builtin_ia32_psravq128_mask (__v2di  __X,__v2di  __Y,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_psravq128_mask (__v2di  __X,__v2di  __Y,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_psravq128_mask (__v2di  __X,__v2di  __Y,__v2di  setzero,__mmask8 __U);
__m256i __builtin_ia32_psllv8si_mask (__v8si __X, __v8si __Y, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_psllv8si_mask (__v8si __X, __v8si __Y, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_psllv4si_mask (__v4si __X, __v4si __Y, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_psllv4si_mask (__v4si __X, __v4si __Y, __v4si setzero, __mmask8 __U);
__m256i __builtin_ia32_psllv4di_mask (__v4di  __X, __v4di  __Y, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_psllv4di_mask (__v4di  __X, __v4di  __Y, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_psllv2di_mask (__v2di  __X, __v2di  __Y, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_psllv2di_mask (__v2di  __X, __v2di  __Y, __v2di  setzero, __mmask8 __U);
__m256i __builtin_ia32_psrav8si_mask (__v8si __X, __v8si __Y, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_psrav8si_mask (__v8si __X, __v8si __Y, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_psrav4si_mask (__v4si __X, __v4si __Y, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_psrav4si_mask (__v4si __X, __v4si __Y, __v4si setzero, __mmask8 __U);
__m256i __builtin_ia32_psrlv8si_mask (__v8si __X, __v8si __Y, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_psrlv8si_mask (__v8si __X, __v8si __Y, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_psrlv4si_mask (__v4si __X, __v4si __Y, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_psrlv4si_mask (__v4si __X, __v4si __Y, __v4si setzero, __mmask8 __U);
__m256i __builtin_ia32_psrlv4di_mask (__v4di  __X, __v4di  __Y, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_psrlv4di_mask (__v4di  __X, __v4di  __Y, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_psrlv2di_mask (__v2di  __X, __v2di  __Y, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_psrlv2di_mask (__v2di  __X, __v2di  __Y, __v2di  setzero, __mmask8 __U);
__m256i __builtin_ia32_prolvd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 minus_one);
__m256i __builtin_ia32_prolvd256_mask (__v8si __A,__v8si __B,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_prolvd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_prolvd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 minus_one);
__m128i __builtin_ia32_prolvd128_mask (__v4si __A,__v4si __B,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_prolvd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __U);
__m256i __builtin_ia32_prorvd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 minus_one);
__m256i __builtin_ia32_prorvd256_mask (__v8si __A,__v8si __B,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_prorvd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_prorvd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 minus_one);
__m128i __builtin_ia32_prorvd128_mask (__v4si __A,__v4si __B,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_prorvd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __U);
__m256i __builtin_ia32_prolvq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_prolvq256_mask (__v4di  __A,__v4di  __B,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_prolvq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_prolvq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_prolvq128_mask (__v2di  __A,__v2di  __B,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_prolvq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 __U);
__m256i __builtin_ia32_prorvq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_prorvq256_mask (__v4di  __A,__v4di  __B,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_prorvq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_prorvq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_prorvq128_mask (__v2di  __A,__v2di  __B,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_prorvq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 __U);
__m256i __builtin_ia32_psravq256_mask (__v4di  __X,__v4di  __Y,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_psravq256_mask (__v4di  __X,__v4di  __Y,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_psravq256_mask (__v4di  __X,__v4di  __Y,__v4di setzero,__mmask8 __U);
__m256i __builtin_ia32_pandq256_mask (__v4di  __A, __v4di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_pandq256_mask (__v4di  __A, __v4di  __B, __v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_pandq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_pandq128_mask (__v2di  __A, __v2di  __B, __v2di setzero,__mmask8  __U);
__m256i __builtin_ia32_pandnq256_mask (__v4di  __A,__v4di  __B,__v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_pandnq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_pandnq128_mask (__v2di  __A,__v2di  __B,__v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_pandnq128_mask (__v2di  __A,__v2di  __B,__v2di setzero,__mmask8 __U);
__m256i __builtin_ia32_porq256_mask (__v4di  __A,__v4di  __B,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_porq256_mask (__v4di  __A,__v4di  __B,__v4di  setzero,__mmask8 __U);
__m128i __builtin_ia32_porq128_mask (__v2di  __A,__v2di  __B,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_porq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 __U);
__m256i __builtin_ia32_pxorq256_mask (__v4di  __A, __v4di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_pxorq256_mask (__v4di  __A, __v4di  __B, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_pxorq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_pxorq128_mask (__v2di  __A, __v2di  __B, __v2di setzero, __mmask8 __U);
__m256d __builtin_ia32_maxpd256_mask (__v4df __A, __v4df __B, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_maxpd256_mask (__v4df __A, __v4df __B, __v4df setzero, __mmask8 __U);
__m256 __builtin_ia32_maxps256_mask (__v8sf __A,__v8sf __B,__v8sf __W,__mmask8 __U);
__m256 __builtin_ia32_maxps256_mask (__v8sf __A,__v8sf __B,__v8sf setzero,__mmask8 __U);
__m128 __builtin_ia32_divps_mask (__v4sf __A,  __v4sf __B,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_divps_mask (__v4sf __A,  __v4sf __B,  __v4sf setzero,  __mmask8 __U);
__m128d __builtin_ia32_divpd_mask (__v2df __A,__v2df __B,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_divpd_mask (__v2df __A,__v2df __B,__v2df setzero,__mmask8 __U);
__m256d __builtin_ia32_minpd256_mask (__v4df __A, __v4df __B, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_divpd256_mask (__v4df __A, __v4df __B, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_minpd256_mask (__v4df __A, __v4df __B, __v4df setzero, __mmask8 __U);
__m256 __builtin_ia32_minps256_mask (__v8sf __A,__v8sf __B,__v8sf __W,__mmask8 __U);
__m256d __builtin_ia32_divpd256_mask (__v4df __A, __v4df __B, __v4df setzero, __mmask8 __U);
__m256 __builtin_ia32_divps256_mask (__v8sf __A,__v8sf __B,__v8sf __W,__mmask8 __U);
__m256 __builtin_ia32_minps256_mask (__v8sf __A,__v8sf __B,__v8sf  setzero,__mmask8 __U);
__m256 __builtin_ia32_divps256_mask (__v8sf __A,__v8sf __B,__v8sf  setzero,__mmask8 __U);
__m128 __builtin_ia32_minps_mask (__v4sf __A,  __v4sf __B,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_mulps_mask (__v4sf __A,  __v4sf __B,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_minps_mask (__v4sf __A,  __v4sf __B,  __v4sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_mulps_mask (__v4sf __A,  __v4sf __B,  __v4sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_maxps_mask (__v4sf __A,  __v4sf __B,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_maxps_mask (__v4sf __A,  __v4sf __B,  __v4sf setzero,  __mmask8 __U);
__m128d __builtin_ia32_minpd_mask (__v2df __A,__v2df __B,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_minpd_mask (__v2df __A,__v2df __B,__v2df setzero,__mmask8 __U);
__m128d __builtin_ia32_maxpd_mask (__v2df __A,__v2df __B,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_maxpd_mask (__v2df __A,__v2df __B,__v2df setzero,__mmask8 __U);
__m128d __builtin_ia32_mulpd_mask (__v2df __A,__v2df __B,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_mulpd_mask (__v2df __A,__v2df __B,__v2df setzero,__mmask8 __U);
__m256 __builtin_ia32_mulps256_mask (__v8sf __A,__v8sf __B,__v8sf __W,__mmask8 __U);
__m256 __builtin_ia32_mulps256_mask (__v8sf __A,__v8sf __B,__v8sf setzero,__mmask8 __U);
__m256d __builtin_ia32_mulpd256_mask (__v4df __A, __v4df __B, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_mulpd256_mask (__v4df __A, __v4df __B, __v4df setzero, __mmask8 __U);
__m256i __builtin_ia32_pmaxsq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __M);
__m256i __builtin_ia32_pmaxsq256_mask (__v4di  __A,__v4di  __B,__v4di  __W, __mmask8 __M);
__m256i __builtin_ia32_pminsq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_pminsq256_mask (__v4di  __A,__v4di  __B,__v4di  __W, __mmask8 __M);
__m256i __builtin_ia32_pminsq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __M);
__m256i __builtin_ia32_pmaxuq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __M);
__m256i __builtin_ia32_pmaxsq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_pmaxuq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_pmaxuq256_mask (__v4di  __A,__v4di  __B,__v4di  __W, __mmask8 __M);
__m256i __builtin_ia32_pminuq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_pminuq256_mask (__v4di  __A,__v4di  __B,__v4di  __W, __mmask8 __M);
__m256i __builtin_ia32_pminuq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __M);
__m256i __builtin_ia32_pmaxsd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __M);
__m256i __builtin_ia32_pmaxsd256_mask (__v8si __A,__v8si __B,__v8si __W, __mmask8 __M);
__m256i __builtin_ia32_pminsd256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __M);
__m256i __builtin_ia32_pminsd256_mask (__v8si __A,__v8si __B,__v8si __W, __mmask8 __M);
__m256i __builtin_ia32_pmaxud256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __M);
__m256i __builtin_ia32_pmaxud256_mask (__v8si __A,__v8si __B,__v8si __W, __mmask8 __M);
__m256i __builtin_ia32_pminud256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __M);
__m256i __builtin_ia32_pminud256_mask (__v8si __A,__v8si __B,__v8si __W, __mmask8 __M);
__m128i __builtin_ia32_pmaxsq128_mask (__v2di  __A,__v2di  __B,__v2di setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxsq128_mask (__v2di  __A,__v2di  __B,__v2di  __W, __mmask8 __M);
__m128i __builtin_ia32_pminsq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_pminsq128_mask (__v2di  __A,__v2di  __B,__v2di  __W, __mmask8 __M);
__m128i __builtin_ia32_pminsq128_mask (__v2di  __A,__v2di  __B,__v2di setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxuq128_mask (__v2di  __A,__v2di  __B,__v2di setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxsq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_pmaxuq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_pmaxuq128_mask (__v2di  __A,__v2di  __B,__v2di  __W, __mmask8 __M);
__m128i __builtin_ia32_pminuq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_pminuq128_mask (__v2di  __A,__v2di  __B,__v2di  __W, __mmask8 __M);
__m128i __builtin_ia32_pminuq128_mask (__v2di  __A,__v2di  __B,__v2di setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxsd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxsd128_mask (__v4si __A,__v4si __B,__v4si __W, __mmask8 __M);
__m128i __builtin_ia32_pminsd128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pminsd128_mask (__v4si __A,__v4si __B,__v4si __W, __mmask8 __M);
__m128i __builtin_ia32_pmaxud128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxud128_mask (__v4si __A,__v4si __B,__v4si __W, __mmask8 __M);
__m128i __builtin_ia32_pminud128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pminud128_mask (__v4si __A,__v4si __B,__v4si __W, __mmask8 __M);
__m128i __builtin_ia32_broadcastmb128 (__mmask8 __A);
__m256i __builtin_ia32_broadcastmb256 (__mmask8 __A);
__m128i __builtin_ia32_broadcastmw128 (__mmask16 __A);
__m256i __builtin_ia32_broadcastmw256 (__mmask16 __A);
__m256i __builtin_ia32_vplzcntd_256_mask (__v8si __A,__v8si setzero,__mmask8 minus_one);
__m256i __builtin_ia32_vplzcntd_256_mask (__v8si __A,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_vplzcntd_256_mask (__v8si __A,__v8si setzero,__mmask8 __U);
__m256i __builtin_ia32_vplzcntq_256_mask (__v4di  __A,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_vplzcntq_256_mask (__v4di  __A,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_vplzcntq_256_mask (__v4di  __A,__v4di setzero,__mmask8 __U);
__m256i __builtin_ia32_vpconflictdi_256_mask (__v4di  __A,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_vpconflictdi_256_mask (__v4di  __A,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_vpconflictdi_256_mask (__v4di  __A,__v4di setzero,__mmask8 __U);
__m256i __builtin_ia32_vpconflictsi_256_mask (__v8si __A,__v8si setzero,__mmask8 minus_one);
__m256i __builtin_ia32_vpconflictsi_256_mask (__v8si __A,__v8si __W,__mmask8 __U);
__m256i __builtin_ia32_vpconflictsi_256_mask (__v8si __A,__v8si setzero,__mmask8 __U);
__m128i __builtin_ia32_vplzcntd_128_mask (__v4si __A,__v4si setzero,__mmask8 minus_one);
__m128i __builtin_ia32_vplzcntd_128_mask (__v4si __A,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_vplzcntd_128_mask (__v4si __A,__v4si setzero,__mmask8 __U);
__m128i __builtin_ia32_vplzcntq_128_mask (__v2di  __A,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_vplzcntq_128_mask (__v2di  __A,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_vplzcntq_128_mask (__v2di  __A,__v2di  setzero,__mmask8 __U);
__m128i __builtin_ia32_vpconflictdi_128_mask (__v2di  __A,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_vpconflictdi_128_mask (__v2di  __A,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_vpconflictdi_128_mask (__v2di  __A,__v2di  setzero,__mmask8 __U);
__m128i __builtin_ia32_vpconflictsi_128_mask (__v4si __A,__v4si setzero,__mmask8 minus_one);
__m128i __builtin_ia32_vpconflictsi_128_mask (__v4si __A,__v4si __W,__mmask8 __U);
__m128i __builtin_ia32_vpconflictsi_128_mask (__v4si __A,__v4si setzero,__mmask8 __U);
__m256d __builtin_ia32_unpcklpd256_mask (__v4df __A,  __v4df __B,  __v4df __W,  __mmask8 __U);
__m256d __builtin_ia32_unpcklpd256_mask (__v4df __A,  __v4df __B,  __v4df setzero,  __mmask8 __U);
__m128d __builtin_ia32_unpcklpd128_mask (__v2df __A,  __v2df __B,  __v2df __W,  __mmask8 __U);
__m128d __builtin_ia32_unpcklpd128_mask (__v2df __A,  __v2df __B,  __v2df setzero,  __mmask8 __U);
__m256 __builtin_ia32_unpcklps256_mask (__v8sf __A, __v8sf __B, __v8sf __W, __mmask8 __U);
__m256d __builtin_ia32_unpckhpd256_mask (__v4df __A,  __v4df __B,  __v4df __W,  __mmask8 __U);
__m256d __builtin_ia32_unpckhpd256_mask (__v4df __A,  __v4df __B,  __v4df setzero,  __mmask8 __U);
__m128d __builtin_ia32_unpckhpd128_mask (__v2df __A,  __v2df __B,  __v2df __W,  __mmask8 __U);
__m128d __builtin_ia32_unpckhpd128_mask (__v2df __A,  __v2df __B,  __v2df setzero,  __mmask8 __U);
__m256 __builtin_ia32_unpckhps256_mask (__v8sf __A, __v8sf __B, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_unpckhps256_mask (__v8sf __A, __v8sf __B, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_unpckhps128_mask (__v4sf __A, __v4sf __B, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_unpckhps128_mask (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 __U);
__m128 __builtin_ia32_vcvtph2ps_mask (__v8hi  __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_vcvtph2ps_mask (__v8hi  __A, __v4sf setzero, __mmask8 __U);
__m256 __builtin_ia32_unpcklps256_mask (__v8sf __A, __v8sf __B, __v8sf setzero, __mmask8 __U);
__m256 __builtin_ia32_vcvtph2ps256_mask (__v8hi  __A,  __v8sf __W,  __mmask8 __U);
__m256 __builtin_ia32_vcvtph2ps256_mask (__v8hi  __A,  __v8sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_unpcklps128_mask (__v4sf __A, __v4sf __B, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_unpcklps128_mask (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 __U);
__m256i __builtin_ia32_psrad256_mask (__v8si __A, __v4si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_psrad256_mask (__v8si __A, __v4si __B, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_psrad128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_psrad128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m256i __builtin_ia32_psraq256_mask (__v4di  __A, __v2di  __B, __v4di setzero, __mmask8 minus_one);
__m256i __builtin_ia32_psraq256_mask (__v4di  __A, __v2di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_psraq256_mask (__v4di  __A, __v2di  __B, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_psraq128_mask (__v2di  __A, __v2di  __B, __v2di  setzero, __mmask8 minus_one);
__m128i __builtin_ia32_psraq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_psraq128_mask (__v2di  __A, __v2di  __B, __v2di  setzero, __mmask8 __U);
__m128i __builtin_ia32_pslld128_mask (__v4si __A, __v4si __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_pslld128_mask (__v4si __A, __v4si __B, __v4si setzero, __mmask8 __U);
__m128i __builtin_ia32_psllq128_mask (__v2di  __A, __v2di  __B, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_psllq128_mask (__v2di  __A, __v2di  __B, __v2di  setzero, __mmask8 __U);
__m256i __builtin_ia32_pslld256_mask (__v8si __A, __v4si __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_pslld256_mask (__v8si __A, __v4si __B, __v8si setzero, __mmask8 __U);
__m256i __builtin_ia32_psllq256_mask (__v4di  __A, __v2di  __B, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_psllq256_mask (__v4di  __A, __v2di  __B, __v4di setzero, __mmask8 __U);
__m256 __builtin_ia32_permvarsf256_mask (__v8sf __Y,  __v8si __X,  __v8sf __W,  __mmask8 __U);
__m256 __builtin_ia32_permvarsf256_mask (__v8sf __Y,  __v8si __X,  __v8sf setzero,  __mmask8 __U);
__m256d __builtin_ia32_permvardf256_mask (__v4df __Y,__v4di  __X,__v4df setzero,__mmask8 minus_one);
__m256d __builtin_ia32_permvardf256_mask (__v4df __Y,__v4di  __X,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_permvardf256_mask (__v4df __Y,__v4di  __X,__v4df setzero,__mmask8 __U);
__m256d __builtin_ia32_vpermilvarpd256_mask (__v4df __A, __v4di  __C, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_vpermilvarpd256_mask (__v4df __A, __v4di  __C, __v4df setzero, __mmask8 __U);
__m256 __builtin_ia32_vpermilvarps256_mask (__v8sf __A,  __v8si __C,  __v8sf __W,  __mmask8 __U);
__m256 __builtin_ia32_vpermilvarps256_mask (__v8sf __A,  __v8si __C,  __v8sf setzero,  __mmask8 __U);
__m128d __builtin_ia32_vpermilvarpd_mask (__v2df __A,__v2di  __C,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_vpermilvarpd_mask (__v2df __A,__v2di  __C,__v2df setzero,__mmask8 __U);
__m128 __builtin_ia32_vpermilvarps_mask (__v4sf __A,  __v4si __C,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_vpermilvarps_mask (__v4sf __A,  __v4si __C,  __v4sf setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmulld256_mask (__v8si __A,__v8si __B,__v8si setzero,__mmask8 __M);
__m256i __builtin_ia32_permvardi256_mask (__v4di  __Y,__v4di  __X,__v4di setzero,__mmask8 __M);
__m256i __builtin_ia32_pmulld256_mask (__v8si __A,__v8si __B,__v8si __W, __mmask8 __M);

#if __cplusplus
__m128i __builtin_ia32_pmulld128_mask (__v4si __A,__v4si __B,__v4si setzero,__mmask8 __M);
__m128i __builtin_ia32_pmulld128_mask (__v4si __A,__v4si __B,__v4si __W, __mmask16 __M);
#endif

__m256i __builtin_ia32_pmuldq256_mask (__v8si __X,__v8si __Y,__v4di  __W, __mmask8 __M);
__m256i __builtin_ia32_pmuldq256_mask (__v8si __X,__v8si __Y,__v4di setzero,__mmask8 __M);
__m128i __builtin_ia32_pmuldq128_mask (__v4si __X,__v4si __Y,__v2di  __W, __mmask8 __M);
__m128i __builtin_ia32_pmuldq128_mask (__v4si __X,__v4si __Y,__v2di setzero,__mmask8 __M);
__m256i __builtin_ia32_permvardi256_mask (__v4di  __Y,__v4di  __X,__v4di  __W,__mmask8 __M);
__m256i __builtin_ia32_pmuludq256_mask (__v8si __X, __v8si __Y, __v4di  __W, __mmask8 __M);
__m256i __builtin_ia32_permvarsi256_mask (__v8si __Y,__v8si __X,__v8si setzero,__mmask8 __M);
__m256i __builtin_ia32_pmuludq256_mask (__v8si __X, __v8si __Y, __v4di setzero, __mmask8 __M);
__m128i __builtin_ia32_pmuludq128_mask (__v4si __X, __v4si __Y, __v2di  __W, __mmask8 __M);
__m128i __builtin_ia32_pmuludq128_mask (__v4si __X, __v4si __Y, __v2di setzero, __mmask8 __M);
__m256i __builtin_ia32_permvarsi256_mask (__v8si __Y,__v8si __X,__v8si __W,__mmask8 __M);

typedef short __v32hi ROSE_GNUATTR((__vector_size__ (64)));
typedef char __v64qi ROSE_GNUATTR((__vector_size__ (64)));
typedef unsigned long long __mmask64;

__m512i __builtin_ia32_movdquhi512_mask (__v32hi  __A,  __v32hi  __W,  __mmask32  __U);
__m512i __builtin_ia32_movdquhi512_mask (__v32hi  __A,  __v32hi  setzero,  __mmask32  __U);
__m512i __builtin_ia32_loaddquhi512_mask (__v32hi * __P,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_loaddquhi512_mask (__v32hi * __P,__v32hi  setzero,__mmask32  __U);
void __builtin_ia32_storedquhi512_mask (__v32hi * __P, __v32hi  __A, __mmask32  __U);
__m512i __builtin_ia32_movdquqi512_mask (__v64qi __A,  __v64qi __W,  __mmask64  __U);
__m512i __builtin_ia32_movdquqi512_mask (__v64qi __A,  __v64qi setzero,  __mmask64  __U);
__mmask32 __builtin_ia32_kunpcksi (__mmask32  __A,__mmask32  __B);
__mmask64 __builtin_ia32_kunpckdi (__mmask64  __A,__mmask64  __B);
__m512i __builtin_ia32_loaddquqi512_mask (__v64qi * __P,__v64qi __W,__mmask64  __U);
__m512i __builtin_ia32_loaddquqi512_mask (__v64qi * __P,__v64qi setzero,__mmask64  __U);
void  __builtin_ia32_storedquqi512_mask (__v64qi * __P, __v64qi __A, __mmask64  __U);
__m512i __builtin_ia32_psadbw512 (__v64qi __A,  __v64qi __B);
__m256i __builtin_ia32_pmovwb512_mask (__v32hi  __A,__v32qi setzero,__mmask32  minus_one);

#if __cplusplus
__m256i __builtin_ia32_pmovwb512_mask (__v32hi  __A,__v32qi  __O, __mmask8 __M);
__m256i __builtin_ia32_pmovwb512_mask (__v32hi  __A,__v32qi  setzero,__mmask8 __M);
#endif

__m256i __builtin_ia32_pmovswb512_mask (__v32hi  __A, __v32qi  setzero, __mmask32  minus_one);

#if __cplusplus
__m256i __builtin_ia32_pmovswb512_mask (__v32hi  __A, __v32qi __O, __mmask8 __M);
__m256i __builtin_ia32_pmovswb512_mask (__v32hi  __A, __v32qi  setzero, __mmask8 __M);
#endif

__m256i __builtin_ia32_pmovuswb512_mask (__v32hi  __A,  __v32qi  setzero,  __mmask32  minus_one);

#if __cplusplus
__m256i __builtin_ia32_pmovuswb512_mask (__v32hi  __A,  __v32qi  __O,  __mmask8 __M);
__m256i __builtin_ia32_pmovuswb512_mask (__v32hi  __A,  __v32qi  setzero,  __mmask8 __M);
#endif

__m512i __builtin_ia32_pbroadcastb512_mask (__v16qi  __A,  __v64qi undefined,  __mmask64  minus_one);
__m512i __builtin_ia32_pbroadcastb512_mask (__v16qi  __A,  __v64qi __O,  __mmask64 __M);
__m512i __builtin_ia32_pbroadcastb512_mask (__v16qi  __A,  __v64qi setzero,  __mmask64 __M);
__m512i __builtin_ia32_pbroadcastb512_gpr_mask (char __A,  __v64qi __O,  __mmask8 __M);
__m512i __builtin_ia32_pbroadcastb512_gpr_mask (char __A,  __v64qi setzero,  __mmask8 __M);

#if __cplusplus
__m512i __builtin_ia32_pbroadcastw512_mask (__v8hi  __A,  __v32hi undefined,  __mmask32 minus_one);
__m512i __builtin_ia32_pbroadcastw512_mask (__v8hi  __A,  __v32hi  __O,  __mmask8 __M);
__m512i __builtin_ia32_pbroadcastw512_mask (__v8hi  __A,  __v32hi setzero,  __mmask8 __M);
#endif

__m512i __builtin_ia32_pbroadcastw512_gpr_mask (short __A,  __v32hi  __O,  __mmask8 __M);
__m512i __builtin_ia32_pbroadcastw512_gpr_mask (short __A,  __v32hi setzero,  __mmask8 __M);
__m512i __builtin_ia32_pmulhrsw512_mask (__v32hi  __A,  __v32hi  __B,  __v32hi setzero,  __mmask32  minus_one);
__m512i __builtin_ia32_pmulhrsw512_mask (__v32hi  __A,  __v32hi  __B,  __v32hi  __W,  __mmask32  __U);
__m512i __builtin_ia32_pmulhrsw512_mask (__v32hi  __A,  __v32hi  __B,  __v32hi setzero,  __mmask32  __U);
__m512i __builtin_ia32_pmulhw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_pmulhw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_pmulhw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_pmulhuw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_pmulhuw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_pmulhuw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_pmullw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_pmullw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_pmovsxbw512_mask (__v32qi  __A,  __v32hi  setzero,  __mmask32  minus_one);
__m512i __builtin_ia32_pmovsxbw512_mask (__v32qi  __A,  __v32hi  __W,  __mmask32  __U);
__m512i __builtin_ia32_pmovsxbw512_mask (__v32qi  __A,  __v32hi  setzero,  __mmask32  __U);
__m512i __builtin_ia32_pmovzxbw512_mask (__v32qi  __A,  __v32hi  setzero,  __mmask32  minus_one);
__m512i __builtin_ia32_pmovzxbw512_mask (__v32qi  __A,  __v32hi  __W,  __mmask32  __U);
__m512i __builtin_ia32_pmovzxbw512_mask (__v32qi  __A,  __v32hi  setzero,  __mmask32  __U);
__m512i __builtin_ia32_permvarhi512_mask (__v32hi  __B,__v32hi  __A,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_permvarhi512_mask (__v32hi  __B,__v32hi  __A,__v32hi setzero,__mmask32  __M);
__m512i __builtin_ia32_permvarhi512_mask (__v32hi  __B,__v32hi  __A,__v32hi  __W,__mmask32  __M);
__m512i __builtin_ia32_vpermt2varhi512_mask (__v32hi  __I, __v32hi  __A, __v32hi  __B, __mmask32  minus_one);
__m512i __builtin_ia32_vpermt2varhi512_mask (__v32hi  __I, __v32hi  __A, __v32hi  __B, __mmask32 __U);
__m512i __builtin_ia32_vpermi2varhi512_mask (__v32hi  __A, __v32hi  __I, __v32hi  __B, __mmask32 __U);
__m512i __builtin_ia32_vpermt2varhi512_maskz (__v32hi  __I,__v32hi  __A,__v32hi  __B,__mmask32 __U);
__m512i __builtin_ia32_pavgb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  minus_one);
__m512i __builtin_ia32_pavgb512_mask (__v64qi __A, __v64qi __B, __v64qi __W, __mmask64  __U);
__m512i __builtin_ia32_pavgb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  __U);
__m512i __builtin_ia32_paddb512_mask (__v64qi __A, __v64qi __B, __v64qi __W, __mmask64  __U);
__m512i __builtin_ia32_paddb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  __U);
__m512i __builtin_ia32_psubb512_mask (__v64qi __A, __v64qi __B, __v64qi __W, __mmask64  __U);
__m512i __builtin_ia32_psubb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  __U);
__m512i __builtin_ia32_pavgw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_pavgw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_pavgw512_mask (__v32hi  __A, __v32hi  __B, __v32hi setzero, __mmask32  __U);
__m512i __builtin_ia32_psubsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_psubsb512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __U);
__m512i __builtin_ia32_psubsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __U);
__m512i __builtin_ia32_psubusb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  minus_one);
__m512i __builtin_ia32_psubusb512_mask (__v64qi __A, __v64qi __B, __v64qi __W, __mmask64  __U);
__m512i __builtin_ia32_psubusb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  __U);
__m512i __builtin_ia32_paddsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_paddsb512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __U);
__m512i __builtin_ia32_paddsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __U);
__m512i __builtin_ia32_paddusb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  minus_one);
__m512i __builtin_ia32_paddusb512_mask (__v64qi __A, __v64qi __B, __v64qi __W, __mmask64  __U);
__m512i __builtin_ia32_paddusb512_mask (__v64qi __A, __v64qi __B, __v64qi setzero, __mmask64  __U);
__m512i __builtin_ia32_psubw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_psubw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_psubsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_psubsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_psubsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_psubusw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_psubusw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_psubusw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_paddw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_paddw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_paddsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_paddsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_paddsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_paddusw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_paddusw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_paddusw512_mask (__v32hi  __A, __v32hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_psrlw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_psrlw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_psrlw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_packsswb512_mask (__v32hi  __A,  __v32hi  __B,  __v64qi setzero,  __mmask64  minus_one);
__m512i __builtin_ia32_psllw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_psllw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_psllw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_pmaddubsw512_mask (__v64qi __X,__v64qi __Y,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_pmaddubsw512_mask (__v64qi __X,__v64qi __Y,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_pmaddubsw512_mask (__v64qi __X,__v64qi __Y,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_pmaddwd512_mask (__v32hi  __A, __v32hi  __B, __v16si setzero, __mmask16  minus_one);
__m512i __builtin_ia32_pmaddwd512_mask (__v32hi  __A, __v32hi  __B, __v16si __W, __mmask16  __U);
__m512i __builtin_ia32_pmaddwd512_mask (__v32hi  __A, __v32hi  __B, __v16si setzero, __mmask16  __U);
__m512i __builtin_ia32_punpckhbw512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_punpckhbw512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __U);
__m512i __builtin_ia32_punpckhbw512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __U);
__m512i __builtin_ia32_punpckhwd512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_punpckhwd512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_punpckhwd512_mask (__v32hi  __A,__v32hi  __B,__v32hi setzero,__mmask32  __U);
__m512i __builtin_ia32_punpcklbw512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_punpcklbw512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __U);
__m512i __builtin_ia32_punpcklbw512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __U);
__m512i __builtin_ia32_punpcklwd512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_punpcklwd512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_punpcklwd512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __A,  __v64qi __B, unsigned int zero,  __mmask64  minus_one);
__mmask64 __builtin_ia32_pcmpeqb512_mask (__v64qi __A,__v64qi __B,__mmask64  minus_one);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __A,  __v64qi __B, unsigned int zero, __mmask64 __U);
__mmask64 __builtin_ia32_pcmpeqb512_mask (__v64qi __A,__v64qi __B, __mmask64  __U);

__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __A,  __v32hi  __B, unsigned int zero,  __mmask32  minus_one);

__mmask32 __builtin_ia32_pcmpeqw512_mask (__v32hi  __A,__v32hi  __B,__mmask32  minus_one);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __A,  __v32hi  __B, unsigned int zero,__mmask64 __U);
#endif

__mmask32 __builtin_ia32_pcmpeqw512_mask (__v32hi  __A,__v32hi  __B,__mmask32 __U);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __A,  __v64qi __B, unsigned int six,  __mmask64  minus_one);
__mmask64 __builtin_ia32_pcmpgtb512_mask (__v64qi __A,__v64qi __B,__mmask64  minus_one);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __A,  __v64qi __B, unsigned int six, __mmask64 __U);
__mmask64 __builtin_ia32_pcmpgtb512_mask (__v64qi __A,__v64qi __B, __mmask64  __U);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __A,  __v32hi  __B, unsigned int six,  __mmask32  minus_one);
#endif

__mmask32 __builtin_ia32_pcmpgtw512_mask (__v32hi  __A,__v32hi  __B,__mmask32  minus_one);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __A,  __v32hi  __B, unsigned int six,__mmask64 __U);
#endif

__mmask32 __builtin_ia32_pcmpgtw512_mask (__v32hi  __A,__v32hi  __B,__mmask32 __U);
__mmask64 __builtin_ia32_cvtb2mask512 (__v64qi __A);
__mmask32 __builtin_ia32_cvtw2mask512 (__v32hi  __A);
__m512i __builtin_ia32_cvtmask2b512 (__mmask64 __A);
__m512i __builtin_ia32_cvtmask2w512 (__mmask32 __A);
__mmask64 __builtin_ia32_ptestmb512 (__v64qi __A,__v64qi __B,__mmask64  minus_one);
__mmask64 __builtin_ia32_ptestmb512 (__v64qi __A,__v64qi __B, __mmask64 __U);
__mmask32 __builtin_ia32_ptestmw512 (__v32hi  __A,__v32hi  __B,__mmask32  minus_one);
__mmask32 __builtin_ia32_ptestmw512 (__v32hi  __A,__v32hi  __B, __mmask32 __U);
__mmask64 __builtin_ia32_ptestnmb512 (__v64qi __A, __v64qi __B, __mmask64  minus_one);
__mmask64 __builtin_ia32_ptestnmb512 (__v64qi __A, __v64qi __B, __mmask64 __U);
__mmask32 __builtin_ia32_ptestnmw512 (__v32hi  __A, __v32hi  __B, __mmask32  minus_one);
__mmask32 __builtin_ia32_ptestnmw512 (__v32hi  __A, __v32hi  __B,  __mmask32 __U);
__m512i __builtin_ia32_pshufb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_pshufb512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __U);
__m512i __builtin_ia32_pshufb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __U);
__m512i __builtin_ia32_pminuw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_pminuw512_mask (__v32hi  __A,__v32hi  __B,__v32hi setzero,__mmask32  __M);
__m512i __builtin_ia32_pminuw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __M);
__m512i __builtin_ia32_pminsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_pminsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi setzero,__mmask32  __M);
__m512i __builtin_ia32_pminsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __M);
__m512i __builtin_ia32_pmaxub512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_pmaxub512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __M);
__m512i __builtin_ia32_pmaxub512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __M);
__m512i __builtin_ia32_pmaxsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_pmaxsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __M);
__m512i __builtin_ia32_pmaxsb512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __M);
__m512i __builtin_ia32_pminub512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_pminub512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __M);
__m512i __builtin_ia32_pminub512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64  __M);
__m512i __builtin_ia32_pminsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  minus_one);
__m512i __builtin_ia32_pminsb512_mask (__v64qi __A,__v64qi __B,__v64qi setzero,__mmask64  __M);
__m512i __builtin_ia32_pminsb512_mask (__v64qi __A,__v64qi __B,__v64qi __W,__mmask64 __M);
__m512i __builtin_ia32_pmaxsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_pmaxsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi setzero,__mmask32  __M);
__m512i __builtin_ia32_pmaxsw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __M);
__m512i __builtin_ia32_pmaxuw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_pmaxuw512_mask (__v32hi  __A,__v32hi  __B,__v32hi setzero,__mmask32  __M);
__m512i __builtin_ia32_pmaxuw512_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __M);
__m512i __builtin_ia32_psraw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_psraw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_psraw512_mask (__v32hi  __A, __v8hi  __B, __v32hi  setzero, __mmask32  __U);
__m512i __builtin_ia32_psrav32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_psrav32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_psrav32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_psrlv32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_psrlv32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_psrlv32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);
__m512i __builtin_ia32_psllv32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  minus_one);
__m512i __builtin_ia32_psllv32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  __W,__mmask32  __U);
__m512i __builtin_ia32_psllv32hi_mask (__v32hi  __A,__v32hi  __B,__v32hi  setzero,__mmask32  __U);

#if __cplusplus
__m512i __builtin_ia32_packsswb512_mask (__v32hi  __A,  __v32hi  __B,  __v64qi __W,  __mmask64  __M);
__m512i __builtin_ia32_packsswb512_mask (__v32hi  __A,  __v32hi  __B,  __v64qi setzero,  __mmask8 __M);
#endif

__m512i __builtin_ia32_packuswb512_mask (__v32hi  __A,  __v32hi  __B,  __v64qi setzero,  __mmask64  minus_one);
__m512i __builtin_ia32_packuswb512_mask (__v32hi  __A,  __v32hi  __B,  __v64qi __W,  __mmask64  __M);
__m512i __builtin_ia32_packuswb512_mask (__v32hi  __A,  __v32hi  __B,  __v64qi setzero,  __mmask64  __M);
__m512i __builtin_ia32_pabsb512_mask (__v64qi __A, __v64qi setzero, __mmask64  minus_one);
__m512i __builtin_ia32_pabsb512_mask (__v64qi __A, __v64qi __W, __mmask64  __U);
__m512i __builtin_ia32_pabsb512_mask (__v64qi __A, __v64qi setzero, __mmask64  __U);
__m512i __builtin_ia32_pabsw512_mask (__v32hi  __A, __v32hi  setzero, __mmask32  minus_one);
__m512i __builtin_ia32_pabsw512_mask (__v32hi  __A, __v32hi  __W, __mmask32  __U);
__m512i __builtin_ia32_pabsw512_mask (__v32hi  __A, __v32hi  setzero, __mmask32  __U);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64  __M);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64  __M);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64  __M);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64  __M);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32  __M);
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32  __M);
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32  __M);
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32  __M);
#endif

__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64  __M);
__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64  __M);
__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64  __M);
__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64  __M);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32  __M);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32  __M);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32  __M);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32  __M);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64 one);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64 one);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64 one);
__mmask64 __builtin_ia32_ucmpb512_mask (__v64qi __X, __v64qi __Y, unsigned int value, __mmask64 one);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32 one);
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32 one);
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32 one);
__mmask32 __builtin_ia32_ucmpw512_mask (__v32hi  __X, __v32hi  __Y, unsigned int value, __mmask32 one);
#endif

__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64 one);
__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64 one);
__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64 one);
__mmask64 __builtin_ia32_cmpb512_mask (__v64qi __X,__v64qi __Y, unsigned int value,__mmask64 one);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32 one);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32 one);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32 one);
__mmask32 __builtin_ia32_cmpw512_mask (__v32hi  __X,__v32hi  __Y, unsigned int value,__mmask32 one);
__m512d __builtin_ia32_broadcastf64x2_512_mask (__v2df __A,  __v8df __O, __mmask8 __M);
__m512d __builtin_ia32_broadcastf64x2_512_mask (__v2df __A,  __v8df setzero,  __mmask8 __M);
__m512i __builtin_ia32_broadcasti64x2_512_mask (__v2di  __A, __v8di  undefined,  __mmask8 minus_one);
__m512i __builtin_ia32_broadcasti64x2_512_mask (__v2di __A,  __v8di __O, __mmask8 __M);
__m512i __builtin_ia32_broadcasti64x2_512_mask (__v2di __A,  __v8di  setzero,  __mmask8 __M);

__m512 __builtin_ia32_broadcastf32x2_512_mask (__v4sf __A, __v16sf undefined, __mmask16  minus_one);

#if __cplusplus
__m512 __builtin_ia32_broadcastf32x2_512_mask (__v4sf __A, __v16sf __O, __mmask8 __M);
__m512 __builtin_ia32_broadcastf32x2_512_mask (__v4sf __A, __v16sf  setzero, __mmask8 __M);
#endif

__m512i __builtin_ia32_broadcasti32x2_512_mask (__v4si __A,  __v16si undefined,  __mmask16  minus_one);
__m512i __builtin_ia32_broadcasti32x2_512_mask (__v4si __A,  __v16si __O, __mmask16 __M);
__m512i __builtin_ia32_broadcasti32x2_512_mask (__v4si __A,  __v16si  setzero,  __mmask16 __M);
__m512 __builtin_ia32_broadcastf32x8_512_mask (__v8sf __A, __v16sf  setzero, __mmask16  minus_one);
__m512 __builtin_ia32_broadcastf32x8_512_mask (__v8sf __A, __v16sf __O, __mmask16 __M);
__m512 __builtin_ia32_broadcastf32x8_512_mask (__v8sf __A, __v16sf  setzero, __mmask16 __M);
__m512i __builtin_ia32_broadcasti32x8_512_mask (__v8si  __A,  __v16si undefined,  __mmask16  minus_one);
__m512i __builtin_ia32_broadcasti32x8_512_mask (__v8si __A,  __v16si __O,  __mmask16 __M);
__m512i __builtin_ia32_broadcasti32x8_512_mask (__v8si __A,  __v16si  setzero,  __mmask16 __M);
__m512i __builtin_ia32_pmullq512_mask (__v8di  __A,__v8di  __B,__v8di  __W,__mmask8 __U);
__m512i __builtin_ia32_pmullq512_mask (__v8di  __A,__v8di  __B,__v8di  setzero,__mmask8 __U);
__m512d __builtin_ia32_xorpd512_mask (__v8df __A, __v8df __B, __v8df setzero, __mmask8 minus_one);
__m512d __builtin_ia32_xorpd512_mask (__v8df __A, __v8df __B, __v8df __W, __mmask8 __U);
__m512d __builtin_ia32_xorpd512_mask (__v8df __A, __v8df __B, __v8df setzero, __mmask8 __U);
__m512 __builtin_ia32_xorps512_mask (__v16sf  __A,__v16sf  __B,__v16sf setzero,__mmask16  minus_one);
__m512 __builtin_ia32_xorps512_mask (__v16sf  __A,__v16sf  __B,__v16sf  __W,__mmask16  __U);
__m512 __builtin_ia32_xorps512_mask (__v16sf  __A,__v16sf  __B,__v16sf setzero,__mmask16  __U);
__m512d __builtin_ia32_orpd512_mask (__v8df __A,__v8df __B,__v8df  setzero,__mmask8 minus_one);
__m512d __builtin_ia32_orpd512_mask (__v8df __A,__v8df __B,__v8df __W,__mmask8 __U);
__m512d __builtin_ia32_orpd512_mask (__v8df __A,__v8df __B,__v8df  setzero,__mmask8 __U);
__m512 __builtin_ia32_orps512_mask (__v16sf  __A, __v16sf  __B, __v16sf  setzero, __mmask16  minus_one);
__m512 __builtin_ia32_orps512_mask (__v16sf  __A, __v16sf  __B, __v16sf  __W, __mmask16  __U);
__m512 __builtin_ia32_orps512_mask (__v16sf  __A, __v16sf  __B, __v16sf  setzero, __mmask16  __U);
__m512d __builtin_ia32_andpd512_mask (__v8df __A, __v8df __B, __v8df setzero, __mmask8 minus_one);
__m512d __builtin_ia32_andpd512_mask (__v8df __A, __v8df __B, __v8df __W, __mmask8 __U);
__m512d __builtin_ia32_andpd512_mask (__v8df __A, __v8df __B, __v8df setzero, __mmask8 __U);
__m512 __builtin_ia32_andps512_mask (__v16sf  __A,__v16sf  __B,__v16sf setzero,__mmask16  minus_one);
__m512 __builtin_ia32_andps512_mask (__v16sf  __A,__v16sf  __B,__v16sf  __W,__mmask16  __U);
__m512 __builtin_ia32_andps512_mask (__v16sf  __A,__v16sf  __B,__v16sf setzero,__mmask16  __U);
__m512d __builtin_ia32_andnpd512_mask (__v8df __A,__v8df __B,__v8df setzero,__mmask8 minus_one);
__m512d __builtin_ia32_andnpd512_mask (__v8df __A,__v8df __B,__v8df __W,__mmask8 __U);
__m512d __builtin_ia32_andnpd512_mask (__v8df __A,__v8df __B,__v8df setzero,__mmask8 __U);
__m512 __builtin_ia32_andnps512_mask (__v16sf __A, __v16sf  __B, __v16sf  setzero, __mmask16  minus_one);
__m512 __builtin_ia32_andnps512_mask (__v16sf __A, __v16sf  __B, __v16sf  __W, __mmask16  __U);
__m512 __builtin_ia32_andnps512_mask (__v16sf __A, __v16sf  __B, __v16sf  setzero, __mmask16  __U);
__mmask16 __builtin_ia32_cvtd2mask512 (__v16si __A);
__mmask8 __builtin_ia32_cvtq2mask512 (__v8di  __A);
__m512i __builtin_ia32_cvtmask2d512 (__mmask16 __A);
__m512i __builtin_ia32_cvtmask2q512 (__mmask8 __A);
__m512i __builtin_ia32_cvttpd2qq512_mask (__v8df __A,__v8di  setzero,__mmask8 minus_one,unsigned int hex);
__m512i __builtin_ia32_cvttpd2qq512_mask (__v8df __A,__v8di  __W,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvttpd2qq512_mask (__v8df __A,__v8di  setzero,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvttpd2uqq512_mask (__v8df __A, __v8di  setzero, __mmask8 minus_one, unsigned int hex);
__m512i __builtin_ia32_cvttpd2uqq512_mask (__v8df __A, __v8di  __W, __mmask8 __U, unsigned int hex);
__m512i __builtin_ia32_cvttpd2uqq512_mask (__v8df __A, __v8di  setzero, __mmask8 __U, unsigned int hex);
__m512i __builtin_ia32_cvttps2qq512_mask (__v8sf __A,__v8di  setzero,__mmask8 minus_one,unsigned int hex);
__m512i __builtin_ia32_cvttps2qq512_mask (__v8sf __A,__v8di  __W,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvttps2qq512_mask (__v8sf __A,__v8di  setzero,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvttps2uqq512_mask (__v8sf __A, __v8di  setzero, __mmask8 minus_one, unsigned int hex);
__m512i __builtin_ia32_cvttps2uqq512_mask (__v8sf __A, __v8di  __W, __mmask8 __U, unsigned int hex);
__m512i __builtin_ia32_cvttps2uqq512_mask (__v8sf __A, __v8di  setzero, __mmask8 __U, unsigned int hex);
__m512i __builtin_ia32_cvtpd2qq512_mask (__v8df __A,  __v8di  setzero,  __mmask8 minus_one,  unsigned int hex);
__m512i __builtin_ia32_cvtpd2qq512_mask (__v8df __A,  __v8di  __W,  __mmask8 __U,  unsigned int hex);
__m512i __builtin_ia32_cvtpd2qq512_mask (__v8df __A,  __v8di  setzero,  __mmask8 __U,  unsigned int hex);
__m512i __builtin_ia32_cvtpd2uqq512_mask (__v8df __A,__v8di  setzero,__mmask8 minus_one,unsigned int hex);
__m512i __builtin_ia32_cvtpd2uqq512_mask (__v8df __A,__v8di  __W,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvtpd2uqq512_mask (__v8df __A,__v8di  setzero,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvtps2qq512_mask (__v8sf __A,  __v8di  setzero,  __mmask8 minus_one,  unsigned int hex);
__m512i __builtin_ia32_cvtps2qq512_mask (__v8sf __A,  __v8di  __W,  __mmask8 __U,  unsigned int hex);
__m512i __builtin_ia32_cvtps2qq512_mask (__v8sf __A,  __v8di  setzero,  __mmask8 __U,  unsigned int hex);
__m512i __builtin_ia32_cvtps2uqq512_mask (__v8sf __A,__v8di  setzero,__mmask8 minus_one,unsigned int hex);
__m512i __builtin_ia32_cvtps2uqq512_mask (__v8sf __A,__v8di  __W,__mmask8 __U,unsigned int hex);
__m512i __builtin_ia32_cvtps2uqq512_mask (__v8sf __A,__v8di  setzero,__mmask8 __U,unsigned int hex);
__m256 __builtin_ia32_cvtqq2ps512_mask (__v8di  __A, __v8sf setzero, __mmask8 minus_one, unsigned int hex);
__m256 __builtin_ia32_cvtqq2ps512_mask (__v8di  __A, __v8sf __W, __mmask8 __U, unsigned int hex);
__m256 __builtin_ia32_cvtqq2ps512_mask (__v8di  __A, __v8sf setzero, __mmask8 __U, unsigned int hex);
__m256 __builtin_ia32_cvtuqq2ps512_mask (__v8di  __A,  __v8sf setzero,  __mmask8 minus_one,  unsigned int hex);
__m256 __builtin_ia32_cvtuqq2ps512_mask (__v8di  __A,  __v8sf __W,  __mmask8 __U,  unsigned int hex);
__m256 __builtin_ia32_cvtuqq2ps512_mask (__v8di  __A,  __v8sf setzero,  __mmask8 __U,  unsigned int hex);
__m512d __builtin_ia32_cvtqq2pd512_mask (__v8di  __A,  __v8df setzero,  __mmask8 minus_one,  unsigned int hex);
__m512d __builtin_ia32_cvtqq2pd512_mask (__v8di  __A,  __v8df __W,  __mmask8 __U,  unsigned int hex);
__m512d __builtin_ia32_cvtqq2pd512_mask (__v8di  __A,  __v8df setzero,  __mmask8 __U,  unsigned int hex);
__m512d __builtin_ia32_cvtuqq2pd512_mask (__v8di  __A,__v8df setzero,__mmask8 minus_one,unsigned int hex);
__m512d __builtin_ia32_cvtuqq2pd512_mask (__v8di  __A,__v8df __W,__mmask8 __U,unsigned int hex);
__m512d __builtin_ia32_cvtuqq2pd512_mask (__v8di  __A,__v8df  setzero,__mmask8 __U,unsigned int hex);
__m256i __builtin_ia32_movdquqi256_mask (__v32qi  __A,  __v32qi  __W,  __mmask32  __U);
__m256i __builtin_ia32_movdquqi256_mask (__v32qi  __A,  __v32qi  setzero,  __mmask32  __U);
__m128i __builtin_ia32_movdquqi128_mask (__v16qi  __A,  __v16qi  __W,  __mmask16  __U);
__m128i __builtin_ia32_movdquqi128_mask (__v16qi  __A,  __v16qi setzero,  __mmask16  __U);
void __builtin_ia32_storedquqi256_mask (__v32qi * __P, __v32qi  __A, __mmask32  __U);
void __builtin_ia32_storedquqi128_mask (__v16qi * __P, __v16qi  __A, __mmask16  __U);
__m256i __builtin_ia32_loaddquhi256_mask (__v16hi * __P,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_loaddquhi256_mask (__v16hi * __P,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_loaddquhi128_mask (__v8hi * __P,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_loaddquhi128_mask (__v8hi * __P,__v8hi setzero,__mmask8 __U);
__m256i __builtin_ia32_movdquhi256_mask (__v16hi  __A,  __v16hi  __W,  __mmask16  __U);
__m256i __builtin_ia32_movdquhi256_mask (__v16hi  __A,  __v16hi  setzero,  __mmask16  __U);
__m128i __builtin_ia32_movdquhi128_mask (__v8hi  __A,  __v8hi  __W,  __mmask8 __U);
__m128i __builtin_ia32_movdquhi128_mask (__v8hi  __A,  __v8hi setzero,  __mmask8 __U);
__m256i __builtin_ia32_loaddquqi256_mask (__v32qi * __P,__v32qi  __W,__mmask32  __U);
__m256i __builtin_ia32_loaddquqi256_mask (__v32qi * __P,__v32qi  setzero,__mmask32  __U);
__m128i __builtin_ia32_loaddquqi128_mask (__v16qi * __P,__v16qi  __W,__mmask16  __U);
__m128i __builtin_ia32_loaddquqi128_mask (__v16qi * __P,__v16qi setzero,__mmask16  __U);

__m128i __builtin_ia32_pmovwb256_mask (__v16hi  __A,__v16qi undefined,__mmask16  minus_one);

#if __cplusplus
__m128i __builtin_ia32_pmovwb256_mask (__v16hi  __A,__v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovwb256_mask (__v16hi  __A,__v16qi setzero,__mmask8 __M);
#endif

__m128i __builtin_ia32_pmovswb128_mask (__v8hi  __A, __v16qi undefined, __mmask8 minus_one);
__m128i __builtin_ia32_pmovswb128_mask (__v8hi  __A, __v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovswb128_mask (__v8hi  __A, __v16qi setzero, __mmask8 __M);
__m128i __builtin_ia32_pmovswb256_mask (__v16hi  __A, __v16qi undefined, __mmask16  minus_one);
__m128i __builtin_ia32_pmovswb256_mask (__v16hi  __A, __v16qi  __O, __mmask16 __M);
__m128i __builtin_ia32_pmovswb256_mask (__v16hi  __A, __v16qi setzero, __mmask16 __M);
__m128i __builtin_ia32_pmovuswb128_mask (__v8hi  __A,  __v16qi undefined,  __mmask8 minus_one);
__m128i __builtin_ia32_pmovuswb128_mask (__v8hi  __A,  __v16qi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pmovuswb128_mask (__v8hi  __A,  __v16qi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pmovuswb256_mask (__v16hi  __A,  __v16qi undefined,  __mmask16  minus_one);
__m128i __builtin_ia32_pmovuswb256_mask (__v16hi  __A,  __v16qi  __O,  __mmask16 __M);
__m128i __builtin_ia32_pmovuswb256_mask (__v16hi  __A,  __v16qi setzero,  __mmask16 __M);
__m256i __builtin_ia32_pbroadcastb256_mask (__v16qi  __A,  __v32qi  __O,  __mmask32 __M);
__m256i __builtin_ia32_pbroadcastb256_mask (__v16qi  __A,  __v32qi  setzero,  __mmask32 __M);
__m256i __builtin_ia32_pbroadcastb256_gpr_mask (char __A,  __v32qi  __O,  __mmask32 __M);
__m256i __builtin_ia32_pbroadcastb256_gpr_mask (char __A,  __v32qi  setzero,  __mmask32 __M);
__m128i __builtin_ia32_pbroadcastb128_mask (__v16qi  __A,  __v16qi  __O,  __mmask16 __M);
__m128i __builtin_ia32_pbroadcastb128_mask (__v16qi  __A,  __v16qi setzero,  __mmask16 __M);
__m128i __builtin_ia32_pbroadcastb128_gpr_mask (char __A,  __v16qi  __O,  __mmask16 __M);
__m128i __builtin_ia32_pbroadcastb128_gpr_mask (char __A,  __v16qi setzero,  __mmask16 __M);
__m256i __builtin_ia32_pbroadcastw256_mask (__v8hi  __A,  __v16hi  __O,  __mmask16 __M);
__m256i __builtin_ia32_pbroadcastw256_mask (__v8hi  __A,  __v16hi  setzero,  __mmask16 __M);
__m256i __builtin_ia32_pbroadcastw256_gpr_mask (short __A,  __v16hi  __O,  __mmask16 __M);
__m256i __builtin_ia32_pbroadcastw256_gpr_mask (short __A,  __v16hi  setzero,  __mmask16 __M);
__m128i __builtin_ia32_pbroadcastw128_mask (__v8hi  __A,  __v8hi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastw128_mask (__v8hi  __A,  __v8hi setzero,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastw128_gpr_mask (short __A,  __v8hi  __O,  __mmask8 __M);
__m128i __builtin_ia32_pbroadcastw128_gpr_mask (short __A,  __v8hi setzero,  __mmask8 __M);
__m256i __builtin_ia32_permvarhi256_mask (__v16hi  __B,__v16hi  __A,__v16hi  setzero,__mmask16  minus_one);
__m256i __builtin_ia32_permvarhi256_mask (__v16hi  __B,__v16hi  __A,__v16hi  setzero,__mmask16  __M);
__m256i __builtin_ia32_permvarhi256_mask (__v16hi  __B,__v16hi  __A,__v16hi  __W,__mmask16  __M);
__m128i __builtin_ia32_permvarhi128_mask (__v8hi  __B,__v8hi  __A,__v8hi setzero,__mmask8 minus_one);
__m128i __builtin_ia32_permvarhi128_mask (__v8hi  __B,__v8hi  __A,__v8hi setzero,__mmask8 __M);
__m128i __builtin_ia32_permvarhi128_mask (__v8hi  __B,__v8hi  __A,__v8hi  __W,__mmask8 __M);
__m256i __builtin_ia32_vpermt2varhi256_mask (__v16hi  __I, __v16hi  __A, __v16hi  __B, __mmask16  minus_one);
__m256i __builtin_ia32_vpermt2varhi256_mask (__v16hi  __I, __v16hi  __A, __v16hi  __B, __mmask16 __U);
__m256i __builtin_ia32_vpermi2varhi256_mask (__v16hi  __A, __v16hi  __I, __v16hi  __B, __mmask16 __U);
__m256i __builtin_ia32_vpermt2varhi256_maskz (__v16hi  __I,__v16hi  __A,__v16hi  __B,__mmask16 __U);
__m128i __builtin_ia32_vpermt2varhi128_mask (__v8hi  __I, __v8hi  __A, __v8hi  __B, __mmask8 minus_one);
__m128i __builtin_ia32_vpermt2varhi128_mask (__v8hi  __I, __v8hi  __A, __v8hi  __B, __mmask8 __U);
__m128i __builtin_ia32_vpermi2varhi128_mask (__v8hi  __A, __v8hi  __I, __v8hi  __B, __mmask8 __U);
__m128i __builtin_ia32_vpermt2varhi128_maskz (__v8hi  __I,__v8hi  __A,__v8hi  __B,__mmask8 __U);
__m256i __builtin_ia32_pmaddubsw256_mask (__v32qi  __X,__v32qi  __Y,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_pmaddubsw256_mask (__v32qi  __X,__v32qi  __Y,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_pmaddubsw128_mask (__v16qi  __X,__v16qi  __Y,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_pmaddubsw128_mask (__v16qi  __X,__v16qi  __Y,__v8hi setzero,__mmask8 __U);
__m256i __builtin_ia32_pmaddwd256_mask (__v16hi  __A, __v16hi  __B, __v8si __W, __mmask8 __U);
__m256i __builtin_ia32_pmaddwd256_mask (__v16hi  __A, __v16hi  __B, __v8si setzero, __mmask8 __U);
__m128i __builtin_ia32_pmaddwd128_mask (__v8hi  __A, __v8hi  __B, __v4si __W, __mmask8 __U);
__m128i __builtin_ia32_pmaddwd128_mask (__v8hi  __A, __v8hi  __B, __v4si setzero, __mmask8 __U);
__mmask16 __builtin_ia32_cvtb2mask128 (__v16qi  __A);
__mmask32 __builtin_ia32_cvtb2mask256 (__v32qi  __A);
__mmask8 __builtin_ia32_cvtw2mask128 (__v8hi  __A);
__mmask16 __builtin_ia32_cvtw2mask256 (__v16hi  __A);
__m128i __builtin_ia32_cvtmask2b128 (__mmask16 __A);
__m256i __builtin_ia32_cvtmask2b256 (__mmask32 __A);
__m128i __builtin_ia32_cvtmask2w128 (__mmask8 __A);
__m256i __builtin_ia32_cvtmask2w256 (__mmask16 __A);
__mmask16 __builtin_ia32_ptestmb128 (__v16qi  __A,__v16qi  __B,__mmask16  minus_one);
__mmask16 __builtin_ia32_ptestmb128 (__v16qi  __A,__v16qi  __B, __mmask16 __U);
__mmask32 __builtin_ia32_ptestmb256 (__v32qi  __A,__v32qi  __B,__mmask32  minus_one);
__mmask32 __builtin_ia32_ptestmb256 (__v32qi  __A,__v32qi  __B, __mmask32 __U);
__mmask8 __builtin_ia32_ptestmw128 (__v8hi  __A, __v8hi  __B, __mmask8 minus_one);
__mmask8 __builtin_ia32_ptestmw128 (__v8hi  __A, __v8hi  __B, __mmask8 __U);
__mmask16 __builtin_ia32_ptestmw256 (__v16hi  __A,__v16hi  __B,__mmask16  minus_one);
__mmask16 __builtin_ia32_ptestmw256 (__v16hi  __A,__v16hi  __B, __mmask16 __U);
__m256i __builtin_ia32_pminuw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __M);
__m256i __builtin_ia32_pminuw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __M);
__m128i __builtin_ia32_pminuw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  setzero,__mmask8 __M);
__m128i __builtin_ia32_pminuw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __M);
__m256i __builtin_ia32_pminsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __M);
__m256i __builtin_ia32_pminsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __M);
__m256i __builtin_ia32_pmaxub256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __M);
__m256i __builtin_ia32_pmaxub256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __M);
__m128i __builtin_ia32_pmaxub128_mask (__v16qi  __A,__v16qi  __B,__v16qi  setzero,__mmask16  __M);
__m128i __builtin_ia32_pmaxub128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __M);
__m256i __builtin_ia32_pmaxsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __M);
__m256i __builtin_ia32_pmaxsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __M);
__m128i __builtin_ia32_pmaxsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  setzero,__mmask16  __M);
__m128i __builtin_ia32_pmaxsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __M);
__m256i __builtin_ia32_pminub256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __M);
__m256i __builtin_ia32_pminub256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __M);
__m128i __builtin_ia32_pminub128_mask (__v16qi  __A,__v16qi  __B,__v16qi  setzero,__mmask16  __M);
__m128i __builtin_ia32_pminub128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __M);
__m256i __builtin_ia32_pminsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __M);
__m256i __builtin_ia32_pminsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __M);
__m128i __builtin_ia32_pminsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  setzero,__mmask16  __M);
__m128i __builtin_ia32_pminsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __M);
__m256i __builtin_ia32_pmaxsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __M);
__m256i __builtin_ia32_pmaxsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __M);
__m128i __builtin_ia32_pmaxsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __M);
__m256i __builtin_ia32_pmaxuw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __M);
__m256i __builtin_ia32_pmaxuw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __M);
__m128i __builtin_ia32_pmaxuw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  setzero,__mmask8 __M);
__m128i __builtin_ia32_pmaxuw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __M);
__m128i __builtin_ia32_pminsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  setzero,__mmask8 __M);
__m128i __builtin_ia32_pminsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __M);

__mmask32 __builtin_ia32_cmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask32 one);

#if __cplusplus
__mmask32 __builtin_ia32_cmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask32 one);
__mmask32 __builtin_ia32_cmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask32 one);
__mmask32 __builtin_ia32_cmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask32 one);
#endif

__mmask16 __builtin_ia32_cmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask16 one);

#if __cplusplus
__mmask16 __builtin_ia32_cmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask16 one);
__mmask16 __builtin_ia32_cmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask16 one);
__mmask16 __builtin_ia32_cmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask16 one);
#endif

__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16 one);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16 one);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16 one);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16 one);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8  one);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8  one);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8  one);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8  one);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16 one);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16 one);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16 one);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16 one);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8  one);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8  one);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8  one);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8  one);
__m256i __builtin_ia32_pmulhrsw256_mask (__v16hi  __X,  __v16hi  __Y,  __v16hi  __W,  __mmask16  __U);
__m256i __builtin_ia32_pmulhrsw256_mask (__v16hi  __X,  __v16hi  __Y,  __v16hi  setzero,  __mmask16  __U);
__m256i __builtin_ia32_pmulhuw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_pmulhuw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  setzero, __mmask16  __U);
__m256i __builtin_ia32_pmulhw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_pmulhw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_pmulhw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_pmulhw128_mask (__v8hi  __A,__v8hi  __B,__v8hi setzero,__mmask8 __U);
__m128i __builtin_ia32_pmulhuw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_pmulhuw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m128i __builtin_ia32_pmulhrsw128_mask (__v8hi  __X,  __v8hi  __Y,  __v8hi  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmulhrsw128_mask (__v8hi  __X,  __v8hi  __Y,  __v8hi setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmullw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_pmullw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_pmullw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_pmullw128_mask (__v8hi  __A,__v8hi  __B,__v8hi setzero,__mmask8 __U);
__m256i __builtin_ia32_pmovsxbw256_mask (__v16qi  __A,  __v16hi  __W,  __mmask16  __U);
__m256i __builtin_ia32_pmovsxbw256_mask (__v16qi  __A,  __v16hi  setzero,  __mmask16  __U);
__m128i __builtin_ia32_pmovsxbw128_mask (__v16qi  __A,  __v8hi  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovsxbw128_mask (__v16qi  __A,  __v8hi setzero,  __mmask8 __U);
__m256i __builtin_ia32_pmovzxbw256_mask (__v16qi  __A,  __v16hi  __W,  __mmask16  __U);
__m256i __builtin_ia32_pmovzxbw256_mask (__v16qi  __A,  __v16hi  setzero,  __mmask16  __U);
__m128i __builtin_ia32_pmovzxbw128_mask (__v16qi  __A,  __v8hi  __W,  __mmask8 __U);
__m128i __builtin_ia32_pmovzxbw128_mask (__v16qi  __A,  __v8hi setzero,  __mmask8 __U);
__m256i __builtin_ia32_pavgb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  __W, __mmask32  __U);
__m256i __builtin_ia32_pavgb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  setzero, __mmask32  __U);
__m128i __builtin_ia32_pavgb128_mask (__v16qi  __A, __v16qi  __B, __v16qi  __W, __mmask16  __U);
__m128i __builtin_ia32_pavgb128_mask (__v16qi  __A, __v16qi  __B, __v16qi setzero, __mmask16  __U);
__m256i __builtin_ia32_pavgw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_pavgw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  setzero, __mmask16  __U);
__m128i __builtin_ia32_pavgw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_pavgw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_paddb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  __W, __mmask32  __U);
__m256i __builtin_ia32_paddb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  setzero, __mmask32  __U);
__m256i __builtin_ia32_paddw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_paddw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  setzero, __mmask16  __U);
__m256i __builtin_ia32_paddsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __U);
__m256i __builtin_ia32_paddsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __U);
__m256i __builtin_ia32_paddsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_paddsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m256i __builtin_ia32_paddusb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  __W, __mmask32  __U);
__m256i __builtin_ia32_paddusb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  setzero, __mmask32  __U);
__m256i __builtin_ia32_paddusw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_paddusw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  setzero, __mmask16  __U);
__m256i __builtin_ia32_psubb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  __W, __mmask32  __U);
__m256i __builtin_ia32_psubb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  setzero, __mmask32  __U);
__m256i __builtin_ia32_psubw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_psubw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  setzero, __mmask16  __U);
__m256i __builtin_ia32_psubsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __U);
__m256i __builtin_ia32_psubsb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __U);
__m256i __builtin_ia32_psubsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_psubsw256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m256i __builtin_ia32_psubusb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  __W, __mmask32  __U);
__m256i __builtin_ia32_psubusb256_mask (__v32qi  __A, __v32qi  __B, __v32qi  setzero, __mmask32  __U);
__m256i __builtin_ia32_psubusw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_psubusw256_mask (__v16hi  __A, __v16hi  __B, __v16hi  setzero, __mmask16  __U);
__m128i __builtin_ia32_paddb128_mask (__v16qi  __A, __v16qi  __B, __v16qi  __W, __mmask16  __U);
__m128i __builtin_ia32_paddb128_mask (__v16qi  __A, __v16qi  __B, __v16qi setzero, __mmask16  __U);
__m128i __builtin_ia32_paddw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_paddw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_punpckhbw256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __U);
__m256i __builtin_ia32_punpckhbw256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __U);
__m128i __builtin_ia32_punpckhbw128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __U);
__m128i __builtin_ia32_punpckhbw128_mask (__v16qi  __A,__v16qi  __B,__v16qi setzero,__mmask16  __U);
__m256i __builtin_ia32_punpckhwd256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_punpckhwd256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_punpckhwd128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_punpckhwd128_mask (__v8hi  __A,__v8hi  __B,__v8hi setzero,__mmask8 __U);
__m256i __builtin_ia32_punpcklbw256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __U);
__m256i __builtin_ia32_punpcklbw256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __U);
__m128i __builtin_ia32_punpcklbw128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __U);
__m128i __builtin_ia32_punpcklbw128_mask (__v16qi  __A,__v16qi  __B,__v16qi setzero,__mmask16  __U);
__m256i __builtin_ia32_punpcklwd256_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_punpcklwd256_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_punpcklwd128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_punpcklwd128_mask (__v8hi  __A,__v8hi  __B,__v8hi setzero,__mmask8 __U);
__mmask16 __builtin_ia32_pcmpeqb128_mask (__v16qi  __A,__v16qi  __B,__mmask16  minus_one);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __A,  __v16qi  __B, unsigned int zero,  __mmask16  minus_one);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __A,  __v16qi  __B, unsigned int zero,  __mmask16 __U);
__mmask16 __builtin_ia32_pcmpeqb128_mask (__v16qi  __A,__v16qi  __B,__mmask16 __U);

__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __A,  __v32qi  __B, unsigned int zero,  __mmask32  minus_one);

__mmask32 __builtin_ia32_pcmpeqb256_mask (__v32qi  __A,__v32qi  __B,__mmask32  minus_one);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __A,  __v32qi  __B, unsigned int zero,  __mmask32 __U);
#endif

__mmask32 __builtin_ia32_pcmpeqb256_mask (__v32qi  __A,__v32qi  __B,__mmask32 __U);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __A, __v8hi  __B, unsigned int zero, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpeqw128_mask (__v8hi  __A,  __v8hi  __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __A, __v8hi  __B, unsigned int zero, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpeqw128_mask (__v8hi  __A,  __v8hi  __B, __mmask8 __U);
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __A,  __v16hi  __B, unsigned int zero,  __mmask16  minus_one);
__mmask16 __builtin_ia32_pcmpeqw256_mask (__v16hi  __A,__v16hi  __B,__mmask16  minus_one);

#if __cplusplus
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __A,  __v16hi  __B, unsigned int zero,__mmask16 __U);
#endif

__mmask16 __builtin_ia32_pcmpeqw256_mask (__v16hi  __A,__v16hi  __B,__mmask16 __U);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __A,  __v16qi  __B, unsigned int six,  __mmask16  minus_one);
__mmask16 __builtin_ia32_pcmpgtb128_mask (__v16qi  __A,__v16qi  __B,__mmask16  minus_one);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __A,  __v16qi  __B, unsigned int six,  __mmask16 __U);
__mmask16 __builtin_ia32_pcmpgtb128_mask (__v16qi  __A,__v16qi  __B,__mmask16 __U);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __A,  __v32qi  __B, unsigned int six,  __mmask32  minus_one);
#endif

__mmask32 __builtin_ia32_pcmpgtb256_mask (__v32qi  __A,__v32qi  __B,__mmask32  minus_one);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __A,  __v32qi  __B, unsigned int six,  __mmask32 __U);
#endif

__mmask32 __builtin_ia32_pcmpgtb256_mask (__v32qi  __A,__v32qi  __B,__mmask32 __U);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __A, __v8hi  __B, unsigned int six, __mmask8 minus_one);
__mmask8 __builtin_ia32_pcmpgtw128_mask (__v8hi  __A,  __v8hi  __B,  __mmask8 minus_one);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __A, __v8hi  __B, unsigned int six, __mmask8 __U);
__mmask8 __builtin_ia32_pcmpgtw128_mask (__v8hi  __A,  __v8hi  __B, __mmask8 __U);

#if __cplusplus
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __A,  __v16hi  __B, unsigned int six,  __mmask16  minus_one);
#endif

__mmask16 __builtin_ia32_pcmpgtw256_mask (__v16hi  __A,__v16hi  __B,__mmask16  minus_one);

#if __cplusplus
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __A,  __v16hi  __B, unsigned int six,  __mmask16 __U);
#endif

__mmask16 __builtin_ia32_pcmpgtw256_mask (__v16hi  __A,__v16hi  __B,__mmask16 __U);
__mmask16 __builtin_ia32_ptestnmb128 (__v16qi  __A, __v16qi  __B, __mmask16  minus_one);
__mmask16 __builtin_ia32_ptestnmb128 (__v16qi  __A, __v16qi  __B, __mmask16 __U);
__mmask32 __builtin_ia32_ptestnmb256 (__v32qi  __A, __v32qi  __B, __mmask32  minus_one);
__mmask32 __builtin_ia32_ptestnmb256 (__v32qi  __A, __v32qi  __B, __mmask32 __U);
__mmask8 __builtin_ia32_ptestnmw128 (__v8hi  __A,__v8hi  __B,__mmask8 minus_one);
__mmask8 __builtin_ia32_ptestnmw128 (__v8hi  __A,__v8hi  __B, __mmask8 __U);
__mmask16 __builtin_ia32_ptestnmw256 (__v16hi  __A, __v16hi  __B, __mmask16  minus_one);
__mmask16 __builtin_ia32_ptestnmw256 (__v16hi  __A, __v16hi  __B, __mmask16 __U);
__m256i __builtin_ia32_pshufb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  __W,__mmask32  __U);
__m256i __builtin_ia32_pshufb256_mask (__v32qi  __A,__v32qi  __B,__v32qi  setzero,__mmask32  __U);
__m128i __builtin_ia32_pshufb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __U);
__m128i __builtin_ia32_pshufb128_mask (__v16qi  __A,__v16qi  __B,__v16qi setzero,__mmask16  __U);
__m256i __builtin_ia32_packsswb256_mask (__v16hi  __A,  __v16hi  __B,  __v32qi  setzero,  __mmask32 __M);
__m256i __builtin_ia32_packsswb256_mask (__v16hi  __A,  __v16hi  __B,  __v32qi  __W,  __mmask32 __M);
__m128i __builtin_ia32_packsswb128_mask (__v8hi  __A,  __v8hi  __B,  __v16qi setzero,  __mmask16 __M);
__m128i __builtin_ia32_packsswb128_mask (__v8hi  __A,  __v8hi  __B,  __v16qi  __W,  __mmask16 __M);
__m256i __builtin_ia32_packuswb256_mask (__v16hi  __A,  __v16hi  __B,  __v32qi  setzero,  __mmask32 __M);
__m256i __builtin_ia32_packuswb256_mask (__v16hi  __A,  __v16hi  __B,  __v32qi  __W,  __mmask32 __M);
__m128i __builtin_ia32_packuswb128_mask (__v8hi  __A,  __v8hi  __B,  __v16qi setzero,  __mmask16 __M);
__m128i __builtin_ia32_packuswb128_mask (__v8hi  __A,  __v8hi  __B,  __v16qi  __W,  __mmask16 __M);
__m256i __builtin_ia32_pabsb256_mask (__v32qi  __A, __v32qi  __W, __mmask32  __U);
__m256i __builtin_ia32_pabsb256_mask (__v32qi  __A, __v32qi  setzero, __mmask32  __U);
__m128i __builtin_ia32_pabsb128_mask (__v16qi  __A, __v16qi  __W, __mmask16  __U);
__m128i __builtin_ia32_pabsb128_mask (__v16qi  __A, __v16qi setzero, __mmask16  __U);
__m256i __builtin_ia32_pabsw256_mask (__v16hi  __A, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_pabsw256_mask (__v16hi  __A, __v16hi  setzero, __mmask16  __U);
__m128i __builtin_ia32_pabsw128_mask (__v8hi  __A, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_pabsw128_mask (__v8hi  __A, __v8hi setzero, __mmask8 __U);

#if __cplusplus
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask32 one);
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask32 one);
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask32 one);
__mmask32 __builtin_ia32_ucmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask32 one);
#endif

#if __cplusplus
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask16 one);
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask16 one);
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask16 one);
__mmask16 __builtin_ia32_ucmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask16 one);
#endif

void  __builtin_ia32_storedquhi256_mask (__v16hi * __P, __v16hi  __A, __mmask16  __U);
void  __builtin_ia32_storedquhi128_mask (__v8hi * __P, __v8hi  __A, __mmask8 __U);
__m128i __builtin_ia32_paddsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_psubsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __U);
__m128i __builtin_ia32_psubsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi setzero,__mmask16  __U);
__m128i __builtin_ia32_psubsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi  __W,__mmask8 __U);
__m128i __builtin_ia32_psubsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi setzero,__mmask8 __U);
__m128i __builtin_ia32_psubusb128_mask (__v16qi  __A, __v16qi  __B, __v16qi  __W, __mmask16  __U);
__m128i __builtin_ia32_psubusb128_mask (__v16qi  __A, __v16qi  __B, __v16qi setzero, __mmask16  __U);
__m128i __builtin_ia32_psubusw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psubusw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_psrlw256_mask (__v16hi  __A, __v8hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_psrlw256_mask (__v16hi  __A, __v8hi  __B, __v16hi  setzero, __mmask16  __U);
__m128i __builtin_ia32_psrlw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psrlw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_psraw256_mask (__v16hi  __A, __v8hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_psraw256_mask (__v16hi  __A, __v8hi  __B, __v16hi  setzero, __mmask16  __U);
__m128i __builtin_ia32_psraw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psraw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m128i __builtin_ia32_paddsw128_mask (__v8hi  __A,__v8hi  __B,__v8hi setzero,__mmask8 __U);
__m128i __builtin_ia32_paddusb128_mask (__v16qi  __A, __v16qi  __B, __v16qi  __W, __mmask16  __U);
__m128i __builtin_ia32_paddusb128_mask (__v16qi  __A, __v16qi  __B, __v16qi setzero, __mmask16  __U);
__m128i __builtin_ia32_paddusw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_paddusw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m128i __builtin_ia32_psubb128_mask (__v16qi  __A, __v16qi  __B, __v16qi  __W, __mmask16  __U);
__m128i __builtin_ia32_psubb128_mask (__v16qi  __A, __v16qi  __B, __v16qi setzero, __mmask16  __U);
__m128i __builtin_ia32_psubw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psubw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m128i __builtin_ia32_paddsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi  __W,__mmask16  __U);
__m128i __builtin_ia32_paddsb128_mask (__v16qi  __A,__v16qi  __B,__v16qi setzero,__mmask16  __U);
__m128i __builtin_ia32_pmovwb128_mask (__v8hi  __A,__v16qi undefined,__mmask8 minus_one);
__m128i __builtin_ia32_pmovwb128_mask (__v8hi  __A,__v16qi  __O, __mmask8 __M);
__m128i __builtin_ia32_pmovwb128_mask (__v8hi  __A,__v16qi setzero,__mmask8 __M);
__m256i __builtin_ia32_psrav16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  minus_one);
__m256i __builtin_ia32_psrav16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_psrav16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_psrav8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 minus_one);
__m128i __builtin_ia32_psrav8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psrav8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_psrlv16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  minus_one);
__m256i __builtin_ia32_psrlv16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_psrlv16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_psrlv8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 minus_one);
__m128i __builtin_ia32_psrlv8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psrlv8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_psllv16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  minus_one);
__m256i __builtin_ia32_psllv16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  __W,__mmask16  __U);
__m256i __builtin_ia32_psllv16hi_mask (__v16hi  __A,__v16hi  __B,__v16hi  setzero,__mmask16  __U);
__m128i __builtin_ia32_psllv8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 minus_one);
__m128i __builtin_ia32_psllv8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psllv8hi_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m128i __builtin_ia32_psllw128_mask (__v8hi  __A, __v8hi  __B, __v8hi  __W, __mmask8 __U);
__m128i __builtin_ia32_psllw128_mask (__v8hi  __A, __v8hi  __B, __v8hi setzero, __mmask8 __U);
__m256i __builtin_ia32_psllw256_mask (__v16hi  __A, __v8hi  __B, __v16hi  __W, __mmask16  __U);
__m256i __builtin_ia32_psllw256_mask (__v16hi  __A, __v8hi  __B, __v16hi  setzero, __mmask16  __U);
__m256i __builtin_ia32_packusdw256_mask (__v8si __A,  __v8si __B,  __v16hi  setzero,  __mmask16 __M);
__m256i __builtin_ia32_packusdw256_mask (__v8si __A,  __v8si __B,  __v16hi  __W,  __mmask16 __M);
__m128i __builtin_ia32_packusdw128_mask (__v4si __A,  __v4si __B,  __v8hi setzero,  __mmask8 __M);
__m128i __builtin_ia32_packusdw128_mask (__v4si __A,  __v4si __B,  __v8hi  __W, __mmask8 __M);
__m256i __builtin_ia32_packssdw256_mask (__v8si __A,  __v8si __B,  __v16hi  setzero,  __mmask16 __M);
__m256i __builtin_ia32_packssdw256_mask (__v8si __A,  __v8si __B,  __v16hi  __W,  __mmask16 __M);

__m128i __builtin_ia32_packssdw128_mask (__v4si __A,  __v4si __B,  __v8hi setzero,  __mmask8 __M);

#if __cplusplus
__m128i __builtin_ia32_packssdw128_mask (__v4si __A,  __v4si __B,  __v8hi  __W, __mmask16 __M);
#endif

__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16  __M);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16  __M);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16  __M);
__mmask16 __builtin_ia32_ucmpb128_mask (__v16qi  __X, __v16qi  __Y, unsigned int value, __mmask16  __M);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpw128_mask (__v8hi  __X,__v8hi  __Y, unsigned int value,__mmask8 __M);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16  __M);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16  __M);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16  __M);
__mmask16 __builtin_ia32_cmpb128_mask (__v16qi  __X,__v16qi  __Y, unsigned int value,__mmask16  __M);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpw128_mask (__v8hi  __X, __v8hi  __Y, unsigned int value, __mmask8 __M);

#if __cplusplus
__mmask8 __builtin_ia32_ucmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpb256_mask (__v32qi  __X,__v32qi  __Y, unsigned int value,__mmask8 __M);
#endif

#if __cplusplus
__mmask8 __builtin_ia32_ucmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask8 __M);
__mmask8 __builtin_ia32_ucmpw256_mask (__v16hi  __X,__v16hi  __Y, unsigned int value,__mmask8 __M);
#endif

#if __cplusplus
__mmask8 __builtin_ia32_cmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpb256_mask (__v32qi  __X, __v32qi  __Y, unsigned int value, __mmask8 __M);
#endif

#if __cplusplus
__mmask8 __builtin_ia32_cmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask8 __M);
__mmask8 __builtin_ia32_cmpw256_mask (__v16hi  __X, __v16hi  __Y, unsigned int value, __mmask8 __M);
#endif

__m256i __builtin_ia32_cvttpd2qq256_mask (__v4df __A,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_cvttpd2qq256_mask (__v4df __A,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_cvttpd2qq256_mask (__v4df __A,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_cvttpd2qq128_mask (__v2df __A,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_cvttpd2qq128_mask (__v2df __A,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_cvttpd2qq128_mask (__v2df __A,__v2di setzero,__mmask8 __U);
__m256i __builtin_ia32_cvttpd2uqq256_mask (__v4df __A, __v4di setzero, __mmask8 minus_one);
__m256i __builtin_ia32_cvttpd2uqq256_mask (__v4df __A, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_cvttpd2uqq256_mask (__v4df __A, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2uqq128_mask (__v2df __A, __v2di  setzero, __mmask8 minus_one);
__m128i __builtin_ia32_cvttpd2uqq128_mask (__v2df __A, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_cvttpd2uqq128_mask (__v2df __A, __v2di setzero, __mmask8 __U);
__m256i __builtin_ia32_cvtpd2qq256_mask (__v4df __A,  __v4di setzero,  __mmask8 minus_one);
__m256i __builtin_ia32_cvtpd2qq256_mask (__v4df __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_cvtpd2qq256_mask (__v4df __A,  __v4di setzero,  __mmask8 __U);
__m128i __builtin_ia32_cvtpd2qq128_mask (__v2df __A,  __v2di  setzero,  __mmask8 minus_one);
__m128i __builtin_ia32_cvtpd2qq128_mask (__v2df __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_cvtpd2qq128_mask (__v2df __A,  __v2di setzero,  __mmask8 __U);
__m256i __builtin_ia32_cvtpd2uqq256_mask (__v4df __A,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_cvtpd2uqq256_mask (__v4df __A,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_cvtpd2uqq256_mask (__v4df __A,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_cvtpd2uqq128_mask (__v2df __A,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_cvtpd2uqq128_mask (__v2df __A,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_cvtpd2uqq128_mask (__v2df __A,__v2di setzero,__mmask8 __U);
__m256i __builtin_ia32_cvttps2qq256_mask (__v4sf __A,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_cvttps2qq256_mask (__v4sf __A,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_cvttps2qq256_mask (__v4sf __A,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_cvttps2qq128_mask (__v4sf __A,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_cvttps2qq128_mask (__v4sf __A,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_cvttps2qq128_mask (__v4sf __A,__v2di  setzero,__mmask8 __U);
__m256i __builtin_ia32_cvttps2uqq256_mask (__v4sf __A, __v4di setzero, __mmask8 minus_one);
__m256i __builtin_ia32_cvttps2uqq256_mask (__v4sf __A, __v4di  __W, __mmask8 __U);
__m256i __builtin_ia32_cvttps2uqq256_mask (__v4sf __A, __v4di setzero, __mmask8 __U);
__m128i __builtin_ia32_cvttps2uqq128_mask (__v4sf __A, __v2di  setzero, __mmask8 minus_one);
__m128i __builtin_ia32_cvttps2uqq128_mask (__v4sf __A, __v2di  __W, __mmask8 __U);
__m128i __builtin_ia32_cvttps2uqq128_mask (__v4sf __A, __v2di  setzero, __mmask8 __U);
__m256d __builtin_ia32_broadcastf64x2_256_mask (__v2df __A,__v4df undefined,  __mmask8 minus_one);
__m256d __builtin_ia32_broadcastf64x2_256_mask (__v2df __A,  __v4df __O, __mmask8 __M);
__m256d __builtin_ia32_broadcastf64x2_256_mask (__v2df __A,  __v4df setzero,  __mmask8 __M);
__m256i __builtin_ia32_broadcasti64x2_256_mask (__v2di  __A,__v4di  setzero,  __mmask8 minus_one);
__m256i __builtin_ia32_broadcasti64x2_256_mask (__v2di  __A,  __v4di  __O, __mmask8 __M);
__m256i __builtin_ia32_broadcasti64x2_256_mask (__v2di  __A,  __v4di setzero,  __mmask8 __M);
__m256 __builtin_ia32_broadcastf32x2_256_mask (__v4sf __A, __v8sf undefined, __mmask8 minus_one);
__m256 __builtin_ia32_broadcastf32x2_256_mask (__v4sf __A, __v8sf __O, __mmask8 __M);
__m256 __builtin_ia32_broadcastf32x2_256_mask (__v4sf __A, __v8sf setzero, __mmask8 __M);
__m256i __builtin_ia32_broadcasti32x2_256_mask (__v4si  __A, __v8si setzero,  __mmask8 minus_one);
__m256i __builtin_ia32_broadcasti32x2_256_mask (__v4si __A,  __v8si __O, __mmask8 __M);
__m256i __builtin_ia32_broadcasti32x2_256_mask (__v4si __A,  __v8si setzero,  __mmask8 __M);
__m128i __builtin_ia32_broadcasti32x2_128_mask (__v4si __A, __v4si undefined,  __mmask8 minus_one);
__m128i __builtin_ia32_broadcasti32x2_128_mask (__v4si __A,  __v4si __O, __mmask8 __M);
__m128i __builtin_ia32_broadcasti32x2_128_mask (__v4si __A,  __v4si setzero,  __mmask8 __M);
__m256i __builtin_ia32_pmullq256_mask (__v4di  __A,__v4di  __B,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_pmullq256_mask (__v4di  __A,__v4di  __B,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_pmullq128_mask (__v2di  __A,__v2di  __B,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_pmullq128_mask (__v2di  __A,__v2di  __B,__v2di  setzero,__mmask8 __U);
__m256d __builtin_ia32_andnpd256_mask (__v4df __A,__v4df __B,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_andnpd256_mask (__v4df __A,__v4df __B,__v4df setzero,__mmask8 __U);
__m128d __builtin_ia32_andnpd128_mask (__v2df __A,__v2df __B,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_andnpd128_mask (__v2df __A,__v2df __B,__v2df setzero,__mmask8 __U);
__m256 __builtin_ia32_andnps256_mask (__v8sf __A, __v8sf __B, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_andnps256_mask (__v8sf __A, __v8sf __B, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_andnps128_mask (__v4sf __A, __v4sf __B, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_andnps128_mask (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 __U);
__m256i __builtin_ia32_cvtps2qq256_mask (__v4sf __A,  __v4di setzero,  __mmask8 minus_one);
__m256i __builtin_ia32_cvtps2qq256_mask (__v4sf __A,  __v4di  __W,  __mmask8 __U);
__m256i __builtin_ia32_cvtps2qq256_mask (__v4sf __A,  __v4di setzero,  __mmask8 __U);
__m128i __builtin_ia32_cvtps2qq128_mask (__v4sf __A,  __v2di  setzero,  __mmask8 minus_one);
__m128i __builtin_ia32_cvtps2qq128_mask (__v4sf __A,  __v2di  __W,  __mmask8 __U);
__m128i __builtin_ia32_cvtps2qq128_mask (__v4sf __A,  __v2di  setzero,  __mmask8 __U);
__m256i __builtin_ia32_cvtps2uqq256_mask (__v4sf __A,__v4di setzero,__mmask8 minus_one);
__m256i __builtin_ia32_cvtps2uqq256_mask (__v4sf __A,__v4di  __W,__mmask8 __U);
__m256i __builtin_ia32_cvtps2uqq256_mask (__v4sf __A,__v4di setzero,__mmask8 __U);
__m128i __builtin_ia32_cvtps2uqq128_mask (__v4sf __A,__v2di  setzero,__mmask8 minus_one);
__m128i __builtin_ia32_cvtps2uqq128_mask (__v4sf __A,__v2di  __W,__mmask8 __U);
__m128i __builtin_ia32_cvtps2uqq128_mask (__v4sf __A,__v2di  setzero,__mmask8 __U);
__m128 __builtin_ia32_cvtqq2ps256_mask (__v4di  __A, __v4sf setzero, __mmask8 minus_one);
__m128 __builtin_ia32_cvtqq2ps256_mask (__v4di  __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_cvtqq2ps256_mask (__v4di  __A, __v4sf setzero, __mmask8 __U);
__m128 __builtin_ia32_cvtqq2ps128_mask (__v2di  __A, __v4sf setzero, __mmask8 minus_one);
__m128 __builtin_ia32_cvtqq2ps128_mask (__v2di  __A, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_cvtqq2ps128_mask (__v2di  __A, __v4sf setzero, __mmask8 __U);
__m128 __builtin_ia32_cvtuqq2ps256_mask (__v4di  __A,  __v4sf setzero,  __mmask8 minus_one);
__m128 __builtin_ia32_cvtuqq2ps256_mask (__v4di  __A,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_cvtuqq2ps256_mask (__v4di  __A,  __v4sf setzero,  __mmask8 __U);
__m128 __builtin_ia32_cvtuqq2ps128_mask (__v2di  __A,  __v4sf setzero,  __mmask8 minus_one);
__m128 __builtin_ia32_cvtuqq2ps128_mask (__v2di  __A,  __v4sf __W,  __mmask8 __U);
__m128 __builtin_ia32_cvtuqq2ps128_mask (__v2di  __A,  __v4sf setzero,  __mmask8 __U);
__m256d __builtin_ia32_cvtqq2pd256_mask (__v4di  __A,  __v4df setzero,  __mmask8 minus_one);
__m256d __builtin_ia32_cvtqq2pd256_mask (__v4di  __A,  __v4df __W,  __mmask8 __U);
__m256d __builtin_ia32_cvtqq2pd256_mask (__v4di  __A,  __v4df setzero,  __mmask8 __U);
__m128d __builtin_ia32_cvtqq2pd128_mask (__v2di  __A,  __v2df setzero,  __mmask8 minus_one);
__m128d __builtin_ia32_cvtqq2pd128_mask (__v2di  __A,  __v2df __W,  __mmask8 __U);
__m128d __builtin_ia32_cvtqq2pd128_mask (__v2di  __A,  __v2df setzero,  __mmask8 __U);
__m256d __builtin_ia32_cvtuqq2pd256_mask (__v4di  __A,__v4df setzero,__mmask8 minus_one);
__m256d __builtin_ia32_cvtuqq2pd256_mask (__v4di  __A,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_cvtuqq2pd256_mask (__v4di  __A,__v4df setzero,__mmask8 __U);
__m256d __builtin_ia32_andpd256_mask (__v4df __A, __v4df __B, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_andpd256_mask (__v4df __A, __v4df __B, __v4df setzero, __mmask8 __U);
__m128d __builtin_ia32_andpd128_mask (__v2df __A, __v2df __B, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_andpd128_mask (__v2df __A, __v2df __B, __v2df setzero, __mmask8 __U);
__m256 __builtin_ia32_andps256_mask (__v8sf __A,__v8sf __B,__v8sf __W,__mmask8 __U);
__m256 __builtin_ia32_andps256_mask (__v8sf __A,__v8sf __B,__v8sf setzero,__mmask8 __U);
__m128 __builtin_ia32_andps128_mask (__v4sf __A,__v4sf __B,__v4sf __W,__mmask8 __U);
__m128 __builtin_ia32_andps128_mask (__v4sf __A,__v4sf __B,__v4sf setzero,__mmask8 __U);
__m128d __builtin_ia32_cvtuqq2pd128_mask (__v2di  __A,__v2df setzero,__mmask8 minus_one);
__m128d __builtin_ia32_cvtuqq2pd128_mask (__v2di  __A,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_cvtuqq2pd128_mask (__v2di  __A,__v2df setzero,__mmask8 __U);
__m256d __builtin_ia32_xorpd256_mask (__v4df __A, __v4df __B, __v4df __W, __mmask8 __U);
__m256d __builtin_ia32_xorpd256_mask (__v4df __A, __v4df __B, __v4df setzero, __mmask8 __U);
__m128d __builtin_ia32_xorpd128_mask (__v2df __A, __v2df __B, __v2df __W, __mmask8 __U);
__m128d __builtin_ia32_xorpd128_mask (__v2df __A, __v2df __B, __v2df setzero, __mmask8 __U);
__m256 __builtin_ia32_xorps256_mask (__v8sf __A,__v8sf __B,__v8sf __W,__mmask8 __U);
__m256 __builtin_ia32_xorps256_mask (__v8sf __A,__v8sf __B,__v8sf setzero,__mmask8 __U);
__m128 __builtin_ia32_xorps128_mask (__v4sf __A,__v4sf __B,__v4sf __W,__mmask8 __U);
__m128 __builtin_ia32_xorps128_mask (__v4sf __A,__v4sf __B,__v4sf setzero,__mmask8 __U);
__m256d __builtin_ia32_orpd256_mask (__v4df __A,__v4df __B,__v4df __W,__mmask8 __U);
__m256d __builtin_ia32_orpd256_mask (__v4df __A,__v4df __B,__v4df setzero,__mmask8 __U);
__m128d __builtin_ia32_orpd128_mask (__v2df __A,__v2df __B,__v2df __W,__mmask8 __U);
__m128d __builtin_ia32_orpd128_mask (__v2df __A,__v2df __B,__v2df setzero,__mmask8 __U);
__m256 __builtin_ia32_orps256_mask (__v8sf __A, __v8sf __B, __v8sf __W, __mmask8 __U);
__m256 __builtin_ia32_orps256_mask (__v8sf __A, __v8sf __B, __v8sf setzero, __mmask8 __U);
__m128 __builtin_ia32_orps128_mask (__v4sf __A, __v4sf __B, __v4sf __W, __mmask8 __U);
__m128 __builtin_ia32_orps128_mask (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 __U);
__m128i __builtin_ia32_cvtmask2d128 (__mmask8 __A);
__m256i __builtin_ia32_cvtmask2d256 (__mmask8 __A);
__m128i __builtin_ia32_cvtmask2q128 (__mmask8 __A);
__m256i __builtin_ia32_cvtmask2q256 (__mmask8 __A);
__mmask8 __builtin_ia32_cvtd2mask128 (__v4si __A);
__mmask8 __builtin_ia32_cvtd2mask256 (__v8si __A);
__mmask8 __builtin_ia32_cvtq2mask128 (__v2di  __A);
__mmask8 __builtin_ia32_cvtq2mask256 (__v4di  __A);

// #pragma GCC pop_options
// #pragma GCC push_options
// #pragma GCC target("avx512ifma")
__m512i __builtin_ia32_vpmadd52luq512_mask (__v8di  __X,  __v8di  __Y,  __v8di  __Z,  __mmask8  one);
__m512i __builtin_ia32_vpmadd52huq512_mask (__v8di  __X,  __v8di  __Y,  __v8di  __Z,  __mmask8  one);
__m512i __builtin_ia32_vpmadd52luq512_mask (__v8di  __W,  __v8di  __X,  __v8di  __Y,  __mmask8 __M);
__m512i __builtin_ia32_vpmadd52huq512_mask (__v8di  __W,  __v8di  __X,  __v8di  __Y,  __mmask8 __M);
__m512i __builtin_ia32_vpmadd52luq512_maskz (__v8di  __X, __v8di  __Y, __v8di  __Z, __mmask8 __M);
__m512i __builtin_ia32_vpmadd52huq512_maskz (__v8di  __X, __v8di  __Y, __v8di  __Z, __mmask8 __M);

// #pragma GCC pop_options
// #pragma GCC push_options
// #pragma GCC target("avx512ifma,avx512vl")
__m128i __builtin_ia32_vpmadd52luq128_mask (__v2di  __X,  __v2di  __Y,  __v2di  __Z,  __mmask8  one);
__m128i __builtin_ia32_vpmadd52huq128_mask (__v2di  __X,  __v2di  __Y,  __v2di  __Z,  __mmask8  one);
__m256i __builtin_ia32_vpmadd52luq256_mask (__v4di  __X,  __v4di  __Y,  __v4di  __Z,  __mmask8  one);
__m256i __builtin_ia32_vpmadd52huq256_mask (__v4di  __X,  __v4di  __Y,  __v4di  __Z,  __mmask8  one);
__m128i __builtin_ia32_vpmadd52luq128_mask (__v2di  __W,  __v2di  __X,  __v2di  __Y,  __mmask8 __M);
__m128i __builtin_ia32_vpmadd52huq128_mask (__v2di  __W,  __v2di  __X,  __v2di  __Y,  __mmask8 __M);
__m256i __builtin_ia32_vpmadd52luq256_mask (__v4di  __W,  __v4di  __X,  __v4di  __Y,  __mmask8 __M);
__m256i __builtin_ia32_vpmadd52huq256_mask (__v4di  __W,  __v4di  __X,  __v4di  __Y,  __mmask8 __M);
__m128i __builtin_ia32_vpmadd52luq128_maskz (__v2di  __X, __v2di  __Y, __v2di  __Z, __mmask8 __M);
__m128i __builtin_ia32_vpmadd52huq128_maskz (__v2di  __X, __v2di  __Y, __v2di  __Z, __mmask8 __M);
__m256i __builtin_ia32_vpmadd52luq256_maskz (__v4di  __X, __v4di  __Y, __v4di  __Z, __mmask8 __M);
__m256i __builtin_ia32_vpmadd52huq256_maskz (__v4di  __X, __v4di  __Y, __v4di  __Z, __mmask8 __M);

// #pragma GCC pop_option
// #pragma GCC push_options
// #pragma GCC target("avx512vbmi")
__m512i __builtin_ia32_vpmultishiftqb512_mask (__v64qi __X, __v64qi __Y, __v64qi __W, __mmask64  __M);
__m512i __builtin_ia32_vpmultishiftqb512_mask (__v64qi __X, __v64qi __Y, __v64qi setzero, __mmask64  __M);
__m512i __builtin_ia32_vpmultishiftqb512_mask (__v64qi __X, __v64qi __Y, __v64qi  undefined, __mmask64  minus_one);
__m512i __builtin_ia32_permvarqi512_mask (__v64qi __B,__v64qi __A,__v64qi  undefined,__mmask64  minus_one);
__m512i __builtin_ia32_permvarqi512_mask (__v64qi __B,__v64qi __A,__v64qi  setzero,__mmask64  __M);
__m512i __builtin_ia32_permvarqi512_mask (__v64qi __B,__v64qi __A,__v64qi __W,__mmask64  __M);
__m512i __builtin_ia32_vpermt2varqi512_mask (__v64qi __I, __v64qi __A, __v64qi __B, __mmask64  minus_one);
__m512i __builtin_ia32_vpermt2varqi512_mask (__v64qi __I, __v64qi __A, __v64qi __B, __mmask64 __U);
__m512i __builtin_ia32_vpermi2varqi512_mask (__v64qi __A, __v64qi __I, __v64qi __B, __mmask64 __U);
__m512i __builtin_ia32_vpermt2varqi512_maskz (__v64qi __I,__v64qi __A,__v64qi __B,__mmask64 __U);
__m256i __builtin_ia32_vpmultishiftqb256_mask (__v32qi  __X, __v32qi  __Y, __v32qi  __W, __mmask32  __M);
__m256i __builtin_ia32_vpmultishiftqb256_mask (__v32qi  __X, __v32qi  __Y, __v32qi  setzero, __mmask32  __M);
__m256i __builtin_ia32_vpmultishiftqb256_mask (__v32qi  __X, __v32qi  __Y, __v32qi undefined, __mmask32  minus_one);
__m128i __builtin_ia32_vpmultishiftqb128_mask (__v16qi  __X, __v16qi  __Y, __v16qi  __W, __mmask16  __M);
__m128i __builtin_ia32_vpmultishiftqb128_mask (__v16qi  __X, __v16qi  __Y, __v16qi  setzero, __mmask16  __M);
__m128i __builtin_ia32_vpmultishiftqb128_mask (__v16qi  __X, __v16qi  __Y, __v16qi undefined, __mmask16  minus_one);
__m256i __builtin_ia32_permvarqi256_mask (__v32qi  __B,__v32qi  __A,__v32qi  undefined,__mmask32  minus_one);
__m256i __builtin_ia32_permvarqi256_mask (__v32qi  __B,__v32qi  __A,__v32qi  setzero,__mmask32  __M);
__m256i __builtin_ia32_permvarqi256_mask (__v32qi  __B,__v32qi  __A,__v32qi  __W,__mmask32  __M);
__m128i __builtin_ia32_permvarqi128_mask (__v16qi  __B,__v16qi  __A,__v16qi undefined,__mmask16  minus_one);
__m128i __builtin_ia32_permvarqi128_mask (__v16qi  __B,__v16qi  __A,__v16qi setzero,__mmask16  __M);
__m128i __builtin_ia32_permvarqi128_mask (__v16qi  __B,__v16qi  __A,__v16qi  __W,__mmask16  __M);
__m256i __builtin_ia32_vpermt2varqi256_mask (__v32qi  __I, __v32qi  __A, __v32qi  __B, __mmask32  minus_one);
__m256i __builtin_ia32_vpermt2varqi256_mask (__v32qi  __I, __v32qi  __A, __v32qi  __B, __mmask32 __U);
__m256i __builtin_ia32_vpermi2varqi256_mask (__v32qi  __A, __v32qi  __I, __v32qi  __B, __mmask32 __U);
__m256i __builtin_ia32_vpermt2varqi256_maskz (__v32qi __I, __v32qi __A, __v32qi __B, __mmask32  __U);
__m128i __builtin_ia32_vpermt2varqi128_mask (__v16qi  __I, __v16qi  __A, __v16qi  __B, __mmask16  minus_one);
__m128i __builtin_ia32_vpermt2varqi128_mask (__v16qi  __I, __v16qi  __A, __v16qi  __B, __mmask16 __U);
__m128i __builtin_ia32_vpermi2varqi128_mask (__v16qi  __A, __v16qi  __I, __v16qi  __B, __mmask16 __U);
__m128i __builtin_ia32_vpermt2varqi128_maskz (__v16qi  __I,__v16qi  __A,__v16qi __B,__mmask16 __U);

// #pragma GCC pop_options
// #pragma GCC push_options
// #pragma GCC target("sha")
__m128i __builtin_ia32_sha1msg1 (__v4si __A, __v4si __B);
__m128i __builtin_ia32_sha1msg2 (__v4si __A, __v4si __B);
__m128i __builtin_ia32_sha1nexte (__v4si __A, __v4si __B);
__m128i __builtin_ia32_sha256msg1 (__v4si __A, __v4si __B);
__m128i __builtin_ia32_sha256msg2 (__v4si __A, __v4si __B);
__m128i __builtin_ia32_sha256rnds2 (__v4si __A, __v4si __B,  __v4si __C);

unsigned char __builtin_ia32_sbb_u32 (unsigned char __CF, unsigned int __Y, unsigned int __X, unsigned int * __P);
unsigned char __builtin_ia32_sbb_u64 (unsigned char __CF, unsigned long long __Y, unsigned long long __X, unsigned long long * __P);
void __builtin_ia32_clwb (void * __A);
void __builtin_ia32_pcommit ();
void __builtin_ia32_clflushopt (void* __A);
void __builtin_ia32_xsaves (void* __P, long long __M);
void __builtin_ia32_xrstors (void* __P, long long __M);
void __builtin_ia32_xrstors64 (void* __P, long long __M);
void __builtin_ia32_xsaves64 (void* __P, long long __M);
void __builtin_ia32_xsavec (void* __P, long long __M);
void __builtin_ia32_xsavec64 (void* __P, long long __M);

#undef __mmask32


// DQ (3/11/2017): Added more builtins specfic to Clang 3.8 support.
// #error "Put new builtins here for Clang"
// __m256d __builtin_ia32_undef256();
__m256  __builtin_ia32_undef256();


// DQ (3/12/2017): Make this for C++ only.
#ifdef __cplusplus
__m256i __builtin_ia32_movntdqa256(const __v4di* __V);
#endif

__m128  __builtin_ia32_vfmsubps(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfmsubpd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfmsubss(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfmsubsd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfnmaddps(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfnmaddpd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfnmaddss(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfnmaddsd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfnmsubps(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfnmsubpd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfnmsubss(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfnmsubsd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfmaddsubps(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfmaddsubpd(__m128d __A, __m128d __B, __m128d __C);
__m128  __builtin_ia32_vfmsubaddps(__m128 __A, __m128 __B, __m128 __C);
__m128d __builtin_ia32_vfmsubaddpd(__m128d __A, __m128d __B, __m128d __C);
__m256  __builtin_ia32_vfmaddps256(__m256 __A, __m256 __B, __m256 __C);
__m256d __builtin_ia32_vfmaddpd256(__m256d __A, __m256d __B, __m256d __C);
__m256  __builtin_ia32_vfmsubps256(__m256 __A, __m256 __B, __m256 __C);
__m256d __builtin_ia32_vfmsubpd256(__m256d __A, __m256d __B, __m256d __C);
__m256  __builtin_ia32_vfnmaddps256(__m256 __A, __m256 __B, __m256 __C);
__m256d __builtin_ia32_vfnmaddpd256(__m256d __A, __m256d __B, __m256d __C);
__m256  __builtin_ia32_vfnmsubps256(__m256 __A, __m256 __B, __m256 __C);
__m256d __builtin_ia32_vfnmsubpd256(__m256d __A, __m256d __B, __m256d __C);
__m256  __builtin_ia32_vfmaddsubps256(__m256 __A, __m256 __B, __m256 __C);
__m256d __builtin_ia32_vfmaddsubpd256(__m256d __A, __m256d __B, __m256d __C);
__m256  __builtin_ia32_vfmsubaddps256(__m256 __A, __m256 __B, __m256 __C);
__m256d __builtin_ia32_vfmsubaddpd256(__m256d __A, __m256d __B, __m256d __C);


typedef double __v8df ROSE_GNUATTR((__vector_size__(64)));
typedef float __v16sf ROSE_GNUATTR((__vector_size__(64)));
typedef long long __v8di ROSE_GNUATTR((__vector_size__(64)));
typedef int __v16si ROSE_GNUATTR((__vector_size__(64)));

typedef float __m512 ROSE_GNUATTR((__vector_size__(64)));
typedef double __m512d ROSE_GNUATTR((__vector_size__(64)));
typedef long long __m512i ROSE_GNUATTR((__vector_size__(64)));

typedef unsigned char __mmask8;
typedef unsigned short __mmask16;

__m512 __builtin_ia32_undef512();

__v4sf _mm_setzero_ps();
__m128 __builtin_ia32_maxss_round (__v4sf __A,__v4sf __B, __v4sf __W, __mmask8 __U, unsigned int direction);

__v2df _mm_setzero_pd();
__m128d __builtin_ia32_maxsd_round (__v2df __A, __v2df __B, __v2df __W, __mmask8 __U, unsigned int direction);

// __v16si _mm512_setzero_si512 ();
__m512i __builtin_ia32_pmaxsd512_mask (__v16si __A, __v16si __B, __v16si setzero, __mmask16 minus_one);
__m512i __builtin_ia32_pmaxud512_mask (__v16si __A, __v16si __B, __v16si setzero, __mmask16 minus_one);
__m512i __builtin_ia32_pmaxsq512_mask (__v8di __A, __v8di __B, __v8di setzero, __mmask8 minus_one);
__m128  __builtin_ia32_minss_round    (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 __U, unsigned int direction);
__m128d __builtin_ia32_minsd_round    (__v2df __A, __v2df __B, __v2df __W, __mmask8 __U, unsigned int direction);

// DQ (3/12/2017): Make this for C++ only.
#ifdef __cplusplus
__m128  __builtin_ia32_rsqrt14ss (__v4sf __A,__v4sf __B, __v4sf setzero, __mmask8 minus_one);
__m128d __builtin_ia32_rsqrt14sd (__v2df __A, __v2df __B, __v2df setzero, __mmask8 minus_one);
__m128  __builtin_ia32_rcp14ss (__v4sf __A, __v4sf __B, __v4sf setzero, __mmask8 minus_one);
__m128d __builtin_ia32_rcp14sd (__v2df __A, __v2df __B, __v2df setzero, __mmask8 minus_one);
#endif

#endif





#if 0
// DQ (1/22/2017): these are the remaining builtins to fixup for GNU 5.1 (then we cna focus on whatever might be missing from GNU 6.1).
// Since I have to go on travel and leave rediculusly early, I need to save these for when I get back.
__m128i __builtin_ia32_cvttps2dq128_mask (__v4sf, int );
__m256i __builtin_ia32_cvttps2udq256_mask (__v8sf, int );
__m128i __builtin_ia32_cvttps2udq128_mask (__v4sf, int );
__m128i __builtin_ia32_cvttpd2dq256_mask (__v4df, int );
__m128i __builtin_ia32_cvttpd2dq128_mask (__v2df, int );
__m128i __builtin_ia32_cvttpd2udq256_mask (__v4df, int );
__m128i __builtin_ia32_cvttpd2udq128_mask (__v2df, int );
__m128i __builtin_ia32_cvtpd2dq256_mask (__v4df, int );
__m128i __builtin_ia32_cvtpd2dq128_mask (__v2df, int );
__m256d __builtin_ia32_cvtdq2pd256_mask (__v4si, int );
__m128d __builtin_ia32_cvtdq2pd128_mask (__v4si, int );
__m256d __builtin_ia32_cvtudq2pd256_mask (__v4si, int );
__m128d __builtin_ia32_cvtudq2pd128_mask (__v4si, int );
__m256 __builtin_ia32_cvtdq2ps256_mask (__v8si, int );
__m128 __builtin_ia32_cvtdq2ps128_mask (__v4si, int );
__m256 __builtin_ia32_cvtudq2ps256_mask (__v8si, int );
__m128 __builtin_ia32_cvtudq2ps128_mask (__v4si, int );
__m256d __builtin_ia32_cvtps2pd256_mask (__v4sf, int );
__m128d __builtin_ia32_cvtps2pd128_mask (__v4sf, int );
__m128i __builtin_ia32_pmovdb128_mask (__v4si, int );
void    __builtin_ia32_pmovdb128mem_mask (__v16qi *, __v4si, __m128i, __mmask8);
__m128i __builtin_ia32_pmovdb128_mask (__v4si, int );
__m128i __builtin_ia32_pmovdb256_mask (__v8si, int );
void    __builtin_ia32_pmovdb256mem_mask (__v16qi *, __v8si, __mmask8);
__m128i __builtin_ia32_pmovdb256_mask (__v8si, int );
__m128i __builtin_ia32_pmovsdb128_mask (__v4si, int );
void    __builtin_ia32_pmovsdb128mem_mask (__v16qi *, __v4si, __mmask8);
__m128i __builtin_ia32_pmovsdb128_mask (__v4si, int );
__m128i __builtin_ia32_pmovsdb256_mask (__v8si, int );
void    __builtin_ia32_pmovsdb256mem_mask (__v16qi *, __v8si, __mmask8);
__m128i __builtin_ia32_pmovsdb256_mask (__v8si, int );
__m128i __builtin_ia32_pmovusdb128_mask (__v4si, int );
void    __builtin_ia32_pmovusdb128mem_mask (__v16qi *, __v4si, __mmask8);
__m128i __builtin_ia32_pmovusdb128_mask (__v4si, int );
__m128i __builtin_ia32_pmovusdb256_mask (__v8si, int );
void    __builtin_ia32_pmovusdb256mem_mask (__v16qi*, __v8si, __mmask8);
__m128i __builtin_ia32_pmovusdb256_mask (__v8si, int );
__m128i __builtin_ia32_pmovdw128_mask (__v4si, int );
void    __builtin_ia32_pmovdw128mem_mask (__v8hi *, __v4si, __mmask8);
__m128i __builtin_ia32_pmovdw128_mask (__v4si, int );
__m128i __builtin_ia32_pmovdw256_mask (__v8si, int );
void    __builtin_ia32_pmovdw256mem_mask (__v8hi *, __v8si, __mmask8);
__m128i __builtin_ia32_pmovdw256_mask (__v8si, int );
__m128i __builtin_ia32_pmovsdw128_mask (__v4si, int );
void    __builtin_ia32_pmovsdw128mem_mask (__v8hi *, __v4si, __mmask8);
__m128i __builtin_ia32_pmovsdw128_mask (__v4si, int );
__m128i __builtin_ia32_pmovsdw256_mask (__v8si, int );
void    __builtin_ia32_pmovsdw256mem_mask (__v8hi *, __v8si, __mmask8);
__m128i __builtin_ia32_pmovsdw256_mask (__v8si, int );
__m128i __builtin_ia32_pmovusdw128_mask (__v4si, int );
void    __builtin_ia32_pmovusdw128mem_mask (__v8hi *, __v4si, __mmask8);
__m128i __builtin_ia32_pmovusdw128_mask (__v4si, int );
__m128i __builtin_ia32_pmovusdw256_mask (__v8si, int );
void    __builtin_ia32_pmovusdw256mem_mask (__v8hi *, __v8si, __mmask8);
__m128i __builtin_ia32_pmovusdw256_mask (__v8si, int );
__m128i __builtin_ia32_pmovqb128_mask (__v2di, int );
void    __builtin_ia32_pmovqb128mem_mask (__v16qi *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovqb128_mask (__v2di, int );
__m128i __builtin_ia32_pmovqb256_mask (__v4di, int );
void    __builtin_ia32_pmovqb256mem_mask (__v16qi *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovqb256_mask (__v4di, int );
__m128i __builtin_ia32_pmovsqb128_mask (__v2di, int );
void    __builtin_ia32_pmovsqb128mem_mask (__v16qi *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovsqb128_mask (__v2di, int );
__m128i __builtin_ia32_pmovsqb256_mask (__v4di, int );
void    __builtin_ia32_pmovsqb256mem_mask (__v16qi *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovsqb256_mask (__v4di, int );
__m128i __builtin_ia32_pmovusqb128_mask (__v2di, int );
void    __builtin_ia32_pmovusqb128mem_mask (__v16qi *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovusqb128_mask (__v2di, int );
__m128i __builtin_ia32_pmovusqb256_mask (__v4di, int );
void    __builtin_ia32_pmovusqb256mem_mask (__v16qi *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovusqb256_mask (__v4di, int );
__m128i __builtin_ia32_pmovqw128_mask (__v2di, int );
void    __builtin_ia32_pmovqw128mem_mask (__v8hi *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovqw128_mask (__v2di, int );
__m128i __builtin_ia32_pmovqw256_mask (__v4di, int );
void    __builtin_ia32_pmovqw256mem_mask (__v8hi *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovqw256_mask (__v4di, int );
__m128i __builtin_ia32_pmovsqw128_mask (__v2di, int );
void    __builtin_ia32_pmovsqw128mem_mask (__v8hi *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovsqw128_mask (__v2di, int );
__m128i __builtin_ia32_pmovsqw256_mask (__v4di, int );
void    __builtin_ia32_pmovsqw256mem_mask (__v8hi *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovsqw256_mask (__v4di, int );
__m128i __builtin_ia32_pmovusqw128_mask (__v2di, int );
void    __builtin_ia32_pmovusqw128mem_mask (__v8hi *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovusqw128_mask (__v2di, int );
__m128i __builtin_ia32_pmovusqw256_mask (__v4di, int );
void    __builtin_ia32_pmovusqw256mem_mask (__v8hi *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovusqw256_mask (__v4di, int );
__m128i __builtin_ia32_pmovqd128_mask (__v2di, int );
void    __builtin_ia32_pmovqd128mem_mask (__v4si *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovqd128_mask (__v2di, int );
__m128i __builtin_ia32_pmovqd256_mask (__v4di, int );
void    __builtin_ia32_pmovqd256mem_mask (__v4si *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovqd256_mask (__v4di, int );
__m128i __builtin_ia32_pmovsqd128_mask (__v2di, int );
void    __builtin_ia32_pmovsqd128mem_mask (__v4si *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovsqd128_mask (__v2di, int );
__m128i __builtin_ia32_pmovsqd256_mask (__v4di, int );
void    __builtin_ia32_pmovsqd256mem_mask (__v4si *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovsqd256_mask (__v4di, int );
__m128i __builtin_ia32_pmovusqd128_mask (__v2di, int );
void    __builtin_ia32_pmovusqd128mem_mask (__v4si *, __v2di, __mmask8);
__m128i __builtin_ia32_pmovusqd128_mask (__v2di, int );
__m128i __builtin_ia32_pmovusqd256_mask (__v4di, int );
void    __builtin_ia32_pmovusqd256mem_mask (__v4si *, __v4di, __mmask8);
__m128i __builtin_ia32_pmovusqd256_mask (__v4di, int );
__m256 __builtin_ia32_broadcastss256_mask (__v4sf, int );
__m128 __builtin_ia32_broadcastss128_mask (__v4sf, int );
__m256d __builtin_ia32_broadcastsd256_mask (__v2df, int );
__m256i __builtin_ia32_pbroadcastd256_mask (__v4si, int );
__m256i __builtin_ia32_pbroadcastd256_gpr_mask (int, __v8si, int );
// __m256i __builtin_ia32_pbroadcastd256_gpr_mask (int, int );
__m128i __builtin_ia32_pbroadcastd128_mask (__v4si, int );
__m128i __builtin_ia32_pbroadcastd128_gpr_mask (__v4si, __v4si, int );
// __m128i __builtin_ia32_pbroadcastd128_gpr_mask (int, int );
// __m256i __builtin_ia32_pbroadcastq256_mask (__v2di, int );
__m256i __builtin_ia32_pbroadcastq256_gpr_mask (long long, __v4di, int );
// __m256i __builtin_ia32_pbroadcastq256_gpr_mask (__A, int );
__m128i __builtin_ia32_pbroadcastq128_mask (__v2di, int );
__m128i __builtin_ia32_pbroadcastq128_gpr_mask (long long, __v2di, int );
// __m128i __builtin_ia32_pbroadcastq128_gpr_mask (__A, int );
__m256 __builtin_ia32_broadcastf32x4_256_mask (__v4sf, int );
__m256i __builtin_ia32_broadcasti32x4_256_mask (__v4si,  int );
__m256i __builtin_ia32_pmovsxbd256_mask (__v16qi, int );
__m128i __builtin_ia32_pmovsxbd128_mask (__v16qi, int );
__m256i __builtin_ia32_pmovsxbq256_mask (__v16qi, int );
__m128i __builtin_ia32_pmovsxbq128_mask (__v16qi, int );
__m256i __builtin_ia32_pmovsxwd256_mask (__v8hi, int );
__m128i __builtin_ia32_pmovsxwd128_mask (__v8hi, int );
__m256i __builtin_ia32_pmovsxwq256_mask (__v8hi, int );
__m128i __builtin_ia32_pmovsxwq128_mask (__v8hi, int );
__m256i __builtin_ia32_pmovsxdq256_mask (__v4si, int );
__m128i __builtin_ia32_pmovsxdq128_mask (__v4si, int );
__m256i __builtin_ia32_pmovzxbd256_mask (__v16qi, int );
__m128i __builtin_ia32_pmovzxbd128_mask (__v16qi, int );
__m256i __builtin_ia32_pmovzxbq256_mask (__v16qi, int );
__m128i __builtin_ia32_pmovzxbq128_mask (__v16qi, int );
__m256i __builtin_ia32_pmovzxwd256_mask (__v8hi, int );
__m128i __builtin_ia32_pmovzxwd128_mask (__v8hi, int );
__m256i __builtin_ia32_pmovzxwq256_mask (__v8hi, int );
__m128i __builtin_ia32_pmovzxwq128_mask (__v8hi, int );
__m256i __builtin_ia32_pmovzxdq256_mask (__v4si, int );
__m128i __builtin_ia32_pmovzxdq128_mask (__v4si, int );
__m256d __builtin_ia32_rcp14pd256_mask (__v4df, int );
__m128d __builtin_ia32_rcp14pd128_mask (__v2df, int );
__m256 __builtin_ia32_rcp14ps256_mask (__v8sf, int );
__m128 __builtin_ia32_rcp14ps128_mask (__v4sf, int );
__m256d __builtin_ia32_rsqrt14pd256_mask (__v4df, int );
__m128d __builtin_ia32_rsqrt14pd128_mask (__v2df, int );
__m256 __builtin_ia32_rsqrt14ps256_mask (__v8sf, int );
__m128 __builtin_ia32_rsqrt14ps128_mask (__v4sf, int );
__m256d __builtin_ia32_sqrtpd256_mask (__v4df, int );
__m128d __builtin_ia32_sqrtpd128_mask (__v2df, int );
__m256 __builtin_ia32_sqrtps256_mask (__v8sf, int );
__m128 __builtin_ia32_sqrtps128_mask (__v4sf, int );
__m256i __builtin_ia32_paddd256_mask (__v8si, int );
__m256i __builtin_ia32_paddq256_mask (__v4di, int );
__m256i __builtin_ia32_psubd256_mask (__v8si, int );
__m256i __builtin_ia32_psubq256_mask (__v4di, int );
__m128i __builtin_ia32_paddd128_mask (__v4si, int );
__m128i __builtin_ia32_paddq128_mask (__v2di, int );
__m128i __builtin_ia32_psubd128_mask (__v4si, int );
__m128i __builtin_ia32_psubq128_mask (__v2di, int );
__m256 __builtin_ia32_getexpps256_mask (__v8sf, int );
__m256d __builtin_ia32_getexppd256_mask (__v4df, int );
__m128 __builtin_ia32_getexpps128_mask (__v4sf, int );
__m128d __builtin_ia32_getexppd128_mask (__v2df, int );
__m256i __builtin_ia32_psrld256_mask (__v8si, int );
__m128i __builtin_ia32_psrld128_mask (__v4si, int );
__m256i __builtin_ia32_psrlq256_mask (__v4di , int );
__m128i __builtin_ia32_psrlq128_mask (__v2di , int );
__m256i __builtin_ia32_pandd256_mask (__v8si , int );
__m256d __builtin_ia32_scalefpd256_mask (__v4df , int );
__m256 __builtin_ia32_scalefps256_mask (__v8sf , int );
__m128d __builtin_ia32_scalefpd128_mask (__v2df , int );
__m128 __builtin_ia32_scalefps128_mask (__v4sf , int );
__m256d __builtin_ia32_vfmaddpd256_mask (__v4df , int );
__m256d __builtin_ia32_vfmaddpd256_mask3 (__v4df , int );
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df , int );
__m128d __builtin_ia32_vfmaddpd128_mask (__v2df , int );
__m128d __builtin_ia32_vfmaddpd128_mask3 (__v2df , int );
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df , int );
__m256 __builtin_ia32_vfmaddps256_mask (__v8sf , int );
__m256 __builtin_ia32_vfmaddps256_mask3 (__v8sf , int );
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf , int );
__m128 __builtin_ia32_vfmaddps128_mask (__v4sf , int );
__m128 __builtin_ia32_vfmaddps128_mask3 (__v4sf , int );
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf , int );
__m256d __builtin_ia32_vfmaddpd256_mask (__v4df , int );
__m256d __builtin_ia32_vfmsubpd256_mask3 (__v4df , int );
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df , int );
__m128d __builtin_ia32_vfmaddpd128_mask (__v2df , int );
__m128d __builtin_ia32_vfmsubpd128_mask3 (__v2df , int );
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df , int );
__m256 __builtin_ia32_vfmaddps256_mask (__v8sf , int );
__m256 __builtin_ia32_vfmsubps256_mask3 (__v8sf , int );
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf , int );
__m128 __builtin_ia32_vfmaddps128_mask (__v4sf , int );
__m128 __builtin_ia32_vfmsubps128_mask3 (__v4sf , int );
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf , int );
__m256d __builtin_ia32_vfmaddsubpd256_mask (__v4df , int );
__m256d __builtin_ia32_vfmaddsubpd256_mask3 (__v4df , int );
__m256d __builtin_ia32_vfmaddsubpd256_maskz (__v4df , int );
__m128d __builtin_ia32_vfmaddsubpd128_mask (__v2df , int );
__m128d __builtin_ia32_vfmaddsubpd128_mask3 (__v2df , int );
__m128d __builtin_ia32_vfmaddsubpd128_maskz (__v2df , int );
__m256 __builtin_ia32_vfmaddsubps256_mask (__v8sf , int );
__m256 __builtin_ia32_vfmaddsubps256_mask3 (__v8sf , int );
__m256 __builtin_ia32_vfmaddsubps256_maskz (__v8sf , int );
__m128 __builtin_ia32_vfmaddsubps128_mask (__v4sf , int );
__m128 __builtin_ia32_vfmaddsubps128_mask3 (__v4sf , int );
__m128 __builtin_ia32_vfmaddsubps128_maskz (__v4sf , int );
__m256d __builtin_ia32_vfmaddsubpd256_mask (__v4df , int );
__m256d __builtin_ia32_vfmsubaddpd256_mask3 (__v4df , int );
__m256d __builtin_ia32_vfmaddsubpd256_maskz (__v4df , int );
__m128d __builtin_ia32_vfmaddsubpd128_mask (__v2df , int );
__m128d __builtin_ia32_vfmsubaddpd128_mask3 (__v2df , int );
__m128d __builtin_ia32_vfmaddsubpd128_maskz (__v2df , int );
__m256 __builtin_ia32_vfmaddsubps256_mask (__v8sf , int );
__m256 __builtin_ia32_vfmsubaddps256_mask3 (__v8sf , int );
__m256 __builtin_ia32_vfmaddsubps256_maskz (__v8sf , int );
__m128 __builtin_ia32_vfmaddsubps128_mask (__v4sf , int );
__m128 __builtin_ia32_vfmsubaddps128_mask3 (__v4sf , int );
__m128 __builtin_ia32_vfmaddsubps128_maskz (__v4sf , int );
__m256d __builtin_ia32_vfnmaddpd256_mask (__v4df , int );
__m256d __builtin_ia32_vfmaddpd256_mask3 (__v4df , int );
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df , int );
__m128d __builtin_ia32_vfnmaddpd128_mask (__v2df , int );
__m128d __builtin_ia32_vfmaddpd128_mask3 (__v2df , int );
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df , int );
__m256 __builtin_ia32_vfnmaddps256_mask (__v8sf , int );
__m256 __builtin_ia32_vfmaddps256_mask3 (__v8sf , int );
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf , int );
__m128 __builtin_ia32_vfnmaddps128_mask (__v4sf , int );
__m128 __builtin_ia32_vfmaddps128_mask3 (__v4sf , int );
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf , int );
__m256d __builtin_ia32_vfnmsubpd256_mask (__v4df , int );
__m256d __builtin_ia32_vfnmsubpd256_mask3 (__v4df , int );
__m256d __builtin_ia32_vfmaddpd256_maskz (__v4df , int );
__m128d __builtin_ia32_vfnmsubpd128_mask (__v2df , int );
__m128d __builtin_ia32_vfnmsubpd128_mask3 (__v2df , int );
__m128d __builtin_ia32_vfmaddpd128_maskz (__v2df , int );
__m256 __builtin_ia32_vfnmsubps256_mask (__v8sf , int );
__m256 __builtin_ia32_vfnmsubps256_mask3 (__v8sf , int );
__m256 __builtin_ia32_vfmaddps256_maskz (__v8sf , int );
__m128 __builtin_ia32_vfnmsubps128_mask (__v4sf , int );
__m128 __builtin_ia32_vfnmsubps128_mask3 (__v4sf , int );
__m128 __builtin_ia32_vfmaddps128_maskz (__v4sf , int );
__m128i __builtin_ia32_pandd128_mask (__v4si , int );
__m256i __builtin_ia32_pandnd256_mask (__v8si , int );
__m128i __builtin_ia32_pandnd128_mask (__v4si , int );
__m256i __builtin_ia32_pord256_mask (__v8si , int );
__m128i __builtin_ia32_pord128_mask (__v4si , int );
__m256i __builtin_ia32_pxord256_mask (__v8si , int );
__m128i __builtin_ia32_pxord128_mask (__v4si , int );
__m128 __builtin_ia32_cvtpd2ps_mask (__v2df , int );
__m128 __builtin_ia32_cvtpd2ps256_mask (__v4df , int );
__m256i __builtin_ia32_cvtps2dq256_mask (__v8sf , int );
__m128i __builtin_ia32_cvtps2dq128_mask (__v4sf , int );
__m256i __builtin_ia32_cvtps2udq256_mask (__v8sf , int );
__m128i __builtin_ia32_cvtps2udq128_mask (__v4sf , int );
__m256d __builtin_ia32_movddup256_mask (__v4df , int );
__m128d __builtin_ia32_movddup128_mask (__v2df , int );
__m256 __builtin_ia32_movshdup256_mask (__v8sf , int );
__m128 __builtin_ia32_movshdup128_mask (__v4sf , int );
__m256 __builtin_ia32_movsldup256_mask (__v8sf , int );
__m128 __builtin_ia32_movsldup128_mask (__v4sf , int );
__m128i __builtin_ia32_punpckhdq128_mask (__v4si , int );
__m256i __builtin_ia32_punpckhdq256_mask (__v8si , int );
__m128i __builtin_ia32_punpckhqdq128_mask (__v2di , int );
__m256i __builtin_ia32_punpckhqdq256_mask (__v4di , int );
__m128i __builtin_ia32_punpckldq128_mask (__v4si , int );
__m256i __builtin_ia32_punpckldq256_mask (__v8si , int );
__m128i __builtin_ia32_punpcklqdq128_mask (__v2di , int );
__m256i __builtin_ia32_punpcklqdq256_mask (__v4di , int );
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si , int );
__mmask8 __builtin_ia32_pcmpeqd128_mask (__v4si , int );
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si , int );
__mmask8 __builtin_ia32_pcmpeqd128_mask (__v4si , int );
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si , int );
__mmask8 __builtin_ia32_pcmpeqd256_mask (__v8si , int );
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si , int );
__mmask8 __builtin_ia32_pcmpeqd256_mask (__v8si , int );
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di , int );
__mmask8 __builtin_ia32_pcmpeqq128_mask (__v2di , int );
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di , int );
__mmask8 __builtin_ia32_pcmpeqq128_mask (__v2di , int );
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di , int );
__mmask8 __builtin_ia32_pcmpeqq256_mask (__v4di , int );
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di , int );
__mmask8 __builtin_ia32_pcmpeqq256_mask (__v4di , int );
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si , int );
__mmask8 __builtin_ia32_pcmpgtd128_mask (__v4si , int );
__mmask8 __builtin_ia32_ucmpd128_mask (__v4si , int );
__mmask8 __builtin_ia32_pcmpgtd128_mask (__v4si , int );
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si , int );
__mmask8 __builtin_ia32_pcmpgtd256_mask (__v8si , int );
__mmask8 __builtin_ia32_ucmpd256_mask (__v8si , int );
__mmask8 __builtin_ia32_pcmpgtd256_mask (__v8si , int );
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di , int );
__mmask8 __builtin_ia32_pcmpgtq128_mask (__v2di , int );
__mmask8 __builtin_ia32_ucmpq128_mask (__v2di , int );
__mmask8 __builtin_ia32_pcmpgtq128_mask (__v2di , int );
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di , int );
__mmask8 __builtin_ia32_pcmpgtq256_mask (__v4di , int );
__mmask8 __builtin_ia32_ucmpq256_mask (__v4di , int );
__mmask8 __builtin_ia32_pcmpgtq256_mask (__v4di , int );
__mmask8 __builtin_ia32_ptestmd128 (__v4si , int );
__mmask8 __builtin_ia32_ptestmd256 (__v8si , int );
__mmask8 __builtin_ia32_ptestmq128 (__v2di , int );
__mmask8 __builtin_ia32_ptestmq256 (__v4di , int );
__mmask8 __builtin_ia32_ptestnmd128 (__v4si , int );
__mmask8 __builtin_ia32_ptestnmd256 (__v8si , int );
__mmask8 __builtin_ia32_ptestnmq128 (__v2di , int );
__mmask8 __builtin_ia32_ptestnmq256 (__v4di , int );
__m256d __builtin_ia32_compressdf256_mask (__v4df , int );
void    __builtin_ia32_compressstoredf256_mask (__v4df *, int );
__m128d __builtin_ia32_compressdf128_mask (__v2df , int );
void    __builtin_ia32_compressstoredf128_mask (__v2df *, int );
__m256 __builtin_ia32_compresssf256_mask (__v8sf , int );
void    __builtin_ia32_compressstoresf256_mask (__v8sf *, int );
__m128 __builtin_ia32_compresssf128_mask (__v4sf , int );
void    __builtin_ia32_compressstoresf128_mask (__v4sf *, int );
__m256i __builtin_ia32_compressdi256_mask (__v4di , int );
void    __builtin_ia32_compressstoredi256_mask (__v4di *, int );
__m128i __builtin_ia32_compressdi128_mask (__v2di , int );
void    __builtin_ia32_compressstoredi128_mask (__v2di *, int );
__m256i __builtin_ia32_compresssi256_mask (__v8si , int );
void    __builtin_ia32_compressstoresi256_mask (__v8si *, int );
__m128i __builtin_ia32_compresssi128_mask (__v4si , int );
void    __builtin_ia32_compressstoresi128_mask (__v4si *, int );
__m256d __builtin_ia32_expanddf256_mask (__v4df , int );
__m256d __builtin_ia32_expanddf256_maskz (__v4df , int );
__m256d __builtin_ia32_expandloaddf256_mask (__v4df *, int );
__m256d __builtin_ia32_expandloaddf256_maskz (__v4df *, int );
__m128d __builtin_ia32_expanddf128_mask (__v2df , int );
__m128d __builtin_ia32_expanddf128_maskz (__v2df , int );
__m128d __builtin_ia32_expandloaddf128_mask (__v2df *, int );
__m128d __builtin_ia32_expandloaddf128_maskz (__v2df *, int );
__m256 __builtin_ia32_expandsf256_mask (__v8sf , int );
__m256 __builtin_ia32_expandsf256_maskz (__v8sf , int );
__m256 __builtin_ia32_expandloadsf256_mask (__v8sf *, int );
__m256 __builtin_ia32_expandloadsf256_maskz (__v8sf *, int );
__m128 __builtin_ia32_expandsf128_mask (__v4sf , int );
__m128 __builtin_ia32_expandsf128_maskz (__v4sf , int );
__m128 __builtin_ia32_expandloadsf128_mask (__v4sf *, int );
__m128 __builtin_ia32_expandloadsf128_maskz (__v4sf *, int );
__m256i __builtin_ia32_expanddi256_mask (__v4di , int );
__m256i __builtin_ia32_expanddi256_maskz (__v4di , int );
__m256i __builtin_ia32_expandloaddi256_mask (__v4di *, int );
__m256i __builtin_ia32_expandloaddi256_maskz (__v4di *, int );
__m128i __builtin_ia32_expanddi128_mask (__v2di , int );
__m128i __builtin_ia32_expanddi128_maskz (__v2di , int );
__m128i __builtin_ia32_expandloaddi128_mask (__v2di *, int );
__m128i __builtin_ia32_expandloaddi128_maskz (__v2di *, int );
__m256i __builtin_ia32_expandsi256_mask (__v8si , int );
__m256i __builtin_ia32_expandsi256_maskz (__v8si , int );
__m256i __builtin_ia32_expandloadsi256_mask (__v8si *, int );
__m256i __builtin_ia32_expandloadsi256_maskz (__v8si *, int );
__m128i __builtin_ia32_expandsi128_mask (__v4si , int );
__m128i __builtin_ia32_expandsi128_maskz (__v4si , int );
__m128i __builtin_ia32_expandloadsi128_mask (__v4si *, int );
__m128i __builtin_ia32_expandloadsi128_maskz (__v4si *, int );
__m256d __builtin_ia32_vpermt2varpd256_mask (__v4di, int );
__m256d __builtin_ia32_vpermi2varpd256_mask (__v4df , int );
__m256d __builtin_ia32_vpermt2varpd256_maskz (__v4di, int );
__m256 __builtin_ia32_vpermt2varps256_mask (__v8si __I, int );
__m256 __builtin_ia32_vpermi2varps256_mask (__v8sf , int );
__m256 __builtin_ia32_vpermt2varps256_maskz (__v8si, int );
__m128i __builtin_ia32_vpermt2varq128_mask (__v2di, int );
__m128i __builtin_ia32_vpermi2varq128_mask (__v2di , int );
__m128i __builtin_ia32_vpermt2varq128_maskz (__v2di, int );
__m128i __builtin_ia32_vpermt2vard128_mask (__v4si, int );
__m128i __builtin_ia32_vpermi2vard128_mask (__v4si , int );
__m128i __builtin_ia32_vpermt2vard128_maskz (__v4si, int );
__m256i __builtin_ia32_vpermt2varq256_mask (__v4di, int );
__m256i __builtin_ia32_vpermi2varq256_mask (__v4di , int );
__m256i __builtin_ia32_vpermt2varq256_maskz (__v4di, int );
__m256i __builtin_ia32_vpermt2vard256_mask (__v8si, int );
__m256i __builtin_ia32_vpermi2vard256_mask (__v8si , int );
__m256i __builtin_ia32_vpermt2vard256_maskz (__v8si, int );
__m128d __builtin_ia32_vpermt2varpd128_mask (__v2di, int );
__m128d __builtin_ia32_vpermi2varpd128_mask (__v2df , int );
__m128d __builtin_ia32_vpermt2varpd128_maskz (__v2di, int );
__m128 __builtin_ia32_vpermt2varps128_mask (__v4si, int );
__m128 __builtin_ia32_vpermi2varps128_mask (__v4sf , int );
__m128 __builtin_ia32_vpermt2varps128_maskz (__v4si, int );
__m128i __builtin_ia32_psravq128_mask (__v2di, int );
__m256i __builtin_ia32_psllv8si_mask (__v8si, int );
__m128i __builtin_ia32_psllv4si_mask (__v4si, int );
__m256i __builtin_ia32_psllv4di_mask (__v4di, int );
__m128i __builtin_ia32_psllv2di_mask (__v2di, int );
__m256i __builtin_ia32_psrav8si_mask (__v8si, int );
__m128i __builtin_ia32_psrav4si_mask (__v4si, int );
__m256i __builtin_ia32_psrlv8si_mask (__v8si, int );
__m128i __builtin_ia32_psrlv4si_mask (__v4si, int );
__m256i __builtin_ia32_psrlv4di_mask (__v4di, int );
__m128i __builtin_ia32_psrlv2di_mask (__v2di, int );
__m256i __builtin_ia32_prolvd256_mask (__v8si , int );
__m128i __builtin_ia32_prolvd128_mask (__v4si , int );
__m256i __builtin_ia32_prorvd256_mask (__v8si , int );
__m128i __builtin_ia32_prorvd128_mask (__v4si , int );
__m256i __builtin_ia32_prolvq256_mask (__v4di , int );
__m128i __builtin_ia32_prolvq128_mask (__v2di , int );
__m256i __builtin_ia32_prorvq256_mask (__v4di , int );
__m128i __builtin_ia32_prorvq128_mask (__v2di , int );
__m256i __builtin_ia32_psravq256_mask (__v4di, int );
__m256i __builtin_ia32_pandq256_mask (__v4di , int );
__m128i __builtin_ia32_pandq128_mask (__v2di , int );
__m256i __builtin_ia32_pandnq256_mask (__v4di , int );
__m128i __builtin_ia32_pandnq128_mask (__v2di , int );
__m256i __builtin_ia32_porq256_mask (__v4di , int );
__m128i __builtin_ia32_porq128_mask (__v2di , int );
__m256i __builtin_ia32_pxorq256_mask (__v4di , int );
__m128i __builtin_ia32_pxorq128_mask (__v2di , int );
__m256d __builtin_ia32_maxpd256_mask (__v4df , int );
__m256 __builtin_ia32_maxps256_mask (__v8sf , int );
__m128 __builtin_ia32_divps_mask (__v4sf , int );
__m128d __builtin_ia32_divpd_mask (__v2df , int );
__m256d __builtin_ia32_minpd256_mask (__v4df , int );
__m256d __builtin_ia32_divpd256_mask (__v4df , int );
__m256d __builtin_ia32_minpd256_mask (__v4df , int );
__m256 __builtin_ia32_minps256_mask (__v8sf , int );
__m256d __builtin_ia32_divpd256_mask (__v4df , int );
__m256 __builtin_ia32_divps256_mask (__v8sf , int );
__m256 __builtin_ia32_minps256_mask (__v8sf , int );
__m256 __builtin_ia32_divps256_mask (__v8sf , int );
__m128 __builtin_ia32_minps_mask (__v4sf , int );
__m128 __builtin_ia32_mulps_mask (__v4sf , int );
__m128 __builtin_ia32_minps_mask (__v4sf , int );
__m128 __builtin_ia32_mulps_mask (__v4sf , int );
__m128 __builtin_ia32_maxps_mask (__v4sf , int );
__m128d __builtin_ia32_minpd_mask (__v2df , int );
__m128d __builtin_ia32_maxpd_mask (__v2df , int );
__m128d __builtin_ia32_mulpd_mask (__v2df , int );
__m256 __builtin_ia32_mulps256_mask (__v8sf , int );
__m256d __builtin_ia32_mulpd256_mask (__v4df , int );
__m256i __builtin_ia32_pmaxsq256_mask (__v4di , int );
__m256i __builtin_ia32_pminsq256_mask (__v4di , int );
__m256i __builtin_ia32_pmaxuq256_mask (__v4di , int );
__m256i __builtin_ia32_pmaxsq256_mask (__v4di , int );
__m256i __builtin_ia32_pmaxuq256_mask (__v4di , int );
__m256i __builtin_ia32_pminuq256_mask (__v4di , int );
__m256i __builtin_ia32_pmaxsd256_mask (__v8si , int );
__m256i __builtin_ia32_pminsd256_mask (__v8si , int );
__m256i __builtin_ia32_pmaxud256_mask (__v8si , int );
__m256i __builtin_ia32_pminud256_mask (__v8si , int );
__m128i __builtin_ia32_pmaxsq128_mask (__v2di , int );
__m128i __builtin_ia32_pminsq128_mask (__v2di , int );
__m128i __builtin_ia32_pmaxuq128_mask (__v2di , int );
__m128i __builtin_ia32_pmaxsq128_mask (__v2di , int );
__m128i __builtin_ia32_pmaxuq128_mask (__v2di , int );
__m128i __builtin_ia32_pminuq128_mask (__v2di , int );
__m128i __builtin_ia32_pmaxsd128_mask (__v4si , int );
__m128i __builtin_ia32_pminsd128_mask (__v4si , int );
__m128i __builtin_ia32_pmaxud128_mask (__v4si , int );
__m128i __builtin_ia32_pminud128_mask (__v4si , int );
__m128i __builtin_ia32_broadcastmb128 ();
__m256i __builtin_ia32_broadcastmb256 ();
__m128i __builtin_ia32_broadcastmw128 ();
__m256i __builtin_ia32_broadcastmw256 ();
__m256i __builtin_ia32_vplzcntd_256_mask (__v8si , int );
__m256i __builtin_ia32_vplzcntq_256_mask (__v4di , int );
__m256i __builtin_ia32_vpconflictdi_256_mask (__v4di , int );
__m256i __builtin_ia32_vpconflictsi_256_mask (__v8si , int );
__m128i __builtin_ia32_vplzcntd_128_mask (__v4si , int );
__m128i __builtin_ia32_vplzcntq_128_mask (__v2di , int );
__m128i __builtin_ia32_vpconflictdi_128_mask (__v2di , int );
__m128i __builtin_ia32_vpconflictsi_128_mask (__v4si , int );
__m256d __builtin_ia32_unpcklpd256_mask (__v4df , int );
__m128d __builtin_ia32_unpcklpd128_mask (__v2df , int );
__m256 __builtin_ia32_unpcklps256_mask (__v8sf , int );
__m256d __builtin_ia32_unpckhpd256_mask (__v4df , int );
__m128d __builtin_ia32_unpckhpd128_mask (__v2df , int );
__m256 __builtin_ia32_unpckhps256_mask (__v8sf , int );
__m128 __builtin_ia32_unpckhps128_mask (__v4sf , int );
__m128 __builtin_ia32_vcvtph2ps_mask (__v8hi , int );
__m256 __builtin_ia32_unpcklps256_mask (__v8sf , int );
__m256 __builtin_ia32_vcvtph2ps256_mask (__v8hi , int );
__m128 __builtin_ia32_unpcklps128_mask (__v4sf , int );
__m256i __builtin_ia32_psrad256_mask (__v8si , int );
__m128i __builtin_ia32_psrad128_mask (__v4si , int );
__m256i __builtin_ia32_psraq256_mask (__v4di , int );
__m128i __builtin_ia32_psraq128_mask (__v2di , int );
__m128i __builtin_ia32_pslld128_mask (__v4si , int );
__m128i __builtin_ia32_psllq128_mask (__v2di , int );
__m256i __builtin_ia32_pslld256_mask (__v8si , int );
__m256i __builtin_ia32_psllq256_mask (__v4di , int );
__m256 __builtin_ia32_permvarsf256_mask (__v8sf, int );
__m256d __builtin_ia32_permvardf256_mask (__v4df, int );
__m256d __builtin_ia32_vpermilvarpd256_mask (__v4df , int );
__m256 __builtin_ia32_vpermilvarps256_mask (__v8sf , int );
__m128d __builtin_ia32_vpermilvarpd_mask (__v2df , int );
__m128 __builtin_ia32_vpermilvarps_mask (__v4sf , int );
__m256i __builtin_ia32_pmulld256_mask (__v8si , int );
__m256i __builtin_ia32_permvardi256_mask (__v4di, int );
__m256i __builtin_ia32_pmulld256_mask (__v8si , int );
__m128i __builtin_ia32_pmulld128_mask (__v4si , int );
__m256i __builtin_ia32_pmuldq256_mask (__v8si, int );
__m128i __builtin_ia32_pmuldq128_mask (__v4si, int );
__m256i __builtin_ia32_permvardi256_mask (__v4di, int );
__m256i __builtin_ia32_pmuludq256_mask (__v8si, int );
__m256i __builtin_ia32_permvarsi256_mask (__v8si, int );
__m256i __builtin_ia32_pmuludq256_mask (__v8si, int );
__m128i __builtin_ia32_pmuludq128_mask (__v4si, int );
__m256i __builtin_ia32_permvarsi256_mask (__v8si, int );
__m512i __builtin_ia32_movdquhi512_mask (__v32hi , int );
__m512i __builtin_ia32_loaddquhi512_mask (__v32hi *, int );
void    __builtin_ia32_storedquhi512_mask (__v32hi *, int );
__m512i __builtin_ia32_movdquqi512_mask (__v64qi , int );
__mmask32 __builtin_ia32_kunpcksi (__mmask32, int );
__mmask64 __builtin_ia32_kunpckdi (__mmask64, int );
__m512i __builtin_ia32_loaddquqi512_mask (__v64qi *, int );
void    __builtin_ia32_storedquqi512_mask (__v64qi *, int );
__m512i __builtin_ia32_psadbw512 (__v64qi , int );
__m256i __builtin_ia32_pmovwb512_mask (__v32hi , int );
__m256i __builtin_ia32_pmovswb512_mask (__v32hi , int );
__m256i __builtin_ia32_pmovuswb512_mask (__v32hi , int );
__m512i __builtin_ia32_pbroadcastb512_mask (__v16qi , int );
__m512i __builtin_ia32_pbroadcastb512_gpr_mask (char, int );
__m512i __builtin_ia32_pbroadcastw512_mask (__v8hi , int );
__m512i __builtin_ia32_pbroadcastw512_gpr_mask (short, int );
__m512i __builtin_ia32_pmulhrsw512_mask (__v32hi , int );
__m512i __builtin_ia32_pmulhw512_mask (__v32hi , int );
__m512i __builtin_ia32_pmulhuw512_mask (__v32hi , int );
__m512i __builtin_ia32_pmullw512_mask (__v32hi , int );
__m512i __builtin_ia32_pmovsxbw512_mask (__v32qi, int );
__m512i __builtin_ia32_pmovzxbw512_mask (__v32qi, int );
__m512i __builtin_ia32_permvarhi512_mask (__v32hi, int );
__m512i __builtin_ia32_vpermt2varhi512_mask (__v32hi, int );
__m512i __builtin_ia32_vpermi2varhi512_mask (__v32hi , int );
__m512i __builtin_ia32_vpermt2varhi512_maskz (__v32hi, int );
__m512i __builtin_ia32_pavgb512_mask (__v64qi , int );
__m512i __builtin_ia32_paddb512_mask (__v64qi , int );
__m512i __builtin_ia32_psubb512_mask (__v64qi , int );
__m512i __builtin_ia32_pavgw512_mask (__v32hi , int );
__m512i __builtin_ia32_psubsb512_mask (__v64qi , int );
__m512i __builtin_ia32_psubusb512_mask (__v64qi , int );
__m512i __builtin_ia32_paddsb512_mask (__v64qi, int );


void __builtin_ia32_pmovdb128mem_mask (__v16qi *, __v4si, __mmask8);
__m128i __builtin_ia32_pbroadcastd128_gpr_mask (int, __v4si, int );
__m128i __builtin_ia32_pbroadcastd128_gpr_mask (int, int);
__m256i __builtin_ia32_pbroadcastq256_mask (__v2di, int );

__m128 __builtin_ia32_extractf32x4_mask (__v16sf, int, __v4sf, __mmask8);
__m128i __builtin_ia32_extracti32x4_mask (__v16si, int, __v4si, __mmask8);
__m256d __builtin_ia32_extractf64x4_mask (__v8df, int, __v4df, __mmask8);
__m256d __builtin_ia32_extractf64x4_mask (__v8df, int, __v4df, __mmask8);
__m256d __builtin_ia32_extractf64x4_mask (__v8df, int, __v4df, __mmask8);
#endif
#endif


#if 0
/* From: mmintrin.h */
void __builtin_ia32_emms();
int __builtin_ia32_vec_init_v2si(int,int);
int __builtin_ia32_vec_ext_v2si(int,int);
int __builtin_ia32_packsswb(short,short);
int __builtin_ia32_packssdw(int,int);
int __builtin_ia32_packuswb(short,short);
int __builtin_ia32_punpckhbw(short,short);
int __builtin_ia32_punpckhwd(short,short);
int __builtin_ia32_punpckhdq(int,int);
int __builtin_ia32_punpcklbw(char,char);
int __builtin_ia32_punpcklwd(short,short);
int __builtin_ia32_punpckldq(int,int);
int __builtin_ia32_paddb(char,char);
int __builtin_ia32_paddw(int,int);
int __builtin_ia32_paddd(short,short);
int __builtin_ia32_paddq(long long,long long);
int __builtin_ia32_paddsb(char,char);
int __builtin_ia32_paddsw(int,int);
int __builtin_ia32_paddusb(char,char);
int __builtin_ia32_paddusw(int,int);
int __builtin_ia32_psubb(char,char);
int __builtin_ia32_psubw(int,int);
int __builtin_ia32_psubd(int,int);
int __builtin_ia32_psubq(long long,long long);
int __builtin_ia32_psubsb(char,char);
int __builtin_ia32_psubsw(short,short);
int __builtin_ia32_psubusb(char,char);
int __builtin_ia32_psubusw(int,int);
int __builtin_ia32_pmaddwd(short,short);
int __builtin_ia32_pmulhw(short,short);
int __builtin_ia32_pmullw(short,short);
int __builtin_ia32_psllw(short,long long);
int __builtin_ia32_pslld(int,long long);
int __builtin_ia32_psllq(long long, long long);
int __builtin_ia32_psraw(short, long long);
int __builtin_ia32_psrad(short, long long);
int __builtin_ia32_psrlw(short,long long);
int __builtin_ia32_psrld(short, long long);
int __builtin_ia32_psrlq(long long, long long);
int __builtin_ia32_pand(int,int);
int __builtin_ia32_pandn(int,int);
int __builtin_ia32_por(int,int);
int __builtin_ia32_pxor(int,int);
int __builtin_ia32_pcmpeqb(char,char);
int __builtin_ia32_pcmpgtb(char,char);
int __builtin_ia32_pcmpeqw(short,short);
int __builtin_ia32_pcmpgtw(short,short);
int __builtin_ia32_pcmpeqd(int,int);
int __builtin_ia32_pcmpgtd(int,int);
int __builtin_ia32_vec_init_v2si(int,int);
int __builtin_ia32_vec_init_v4hi(short,short,short,short);
int __builtin_ia32_vec_init_v8qi(char,char,char,char,char,char,char,char);

/* From: xmmintrin.h */
int __builtin_ia32_addss(float,float);
int __builtin_ia32_subss(float,float);
int __builtin_ia32_mulss(float,float);
int __builtin_ia32_divss(float,float);
int __builtin_ia32_sqrtss(float);
int __builtin_ia32_rcpss(float);
int __builtin_ia32_rsqrtss(float);
int __builtin_ia32_minss(float,float);
int __builtin_ia32_maxss(float,float);
int __builtin_ia32_addps(float,float);
int __builtin_ia32_subps(float,float);
int __builtin_ia32_mulps(float,float);
int __builtin_ia32_divps(float,float);
int __builtin_ia32_sqrtps(float);
int __builtin_ia32_rcpps(float);
int __builtin_ia32_rsqrtps(float);
int __builtin_ia32_minps(float,float);
int __builtin_ia32_maxps(float,float);
int __builtin_ia32_andps(float,float);
int __builtin_ia32_andnps(float,float);
int __builtin_ia32_orps(float,float);
int __builtin_ia32_xorps(float,float);
int __builtin_ia32_cmpeqss(float,float);
int __builtin_ia32_cmpltss(float,float);
int __builtin_ia32_cmpless(float,float);
// int __builtin_ia32_cmpltss(float,float);
int __builtin_ia32_movss(float,float);
int __builtin_ia32_cmpless(float,float);
int __builtin_ia32_cmpneqss(float,float);
int __builtin_ia32_cmpnltss(float,float);
int __builtin_ia32_cmpnless(float,float);
int __builtin_ia32_cmpordss(float,float);
int __builtin_ia32_cmpunordss(float,float);
int __builtin_ia32_cmpeqps(float,float);
int __builtin_ia32_cmpltps(float,float);
int __builtin_ia32_cmpleps(float,float);
int __builtin_ia32_cmpgtps(float,float);
int __builtin_ia32_cmpgeps(float,float);
int __builtin_ia32_cmpneqps(float,float);
int __builtin_ia32_cmpnltps(float,float);
int __builtin_ia32_cmpnleps(float,float);
int __builtin_ia32_cmpngtps(float,float);
int __builtin_ia32_cmpngeps(float,float);
int __builtin_ia32_cmpordps(float,float);
int __builtin_ia32_cmpunordps(float,float);
int __builtin_ia32_comieq(float,float);
int __builtin_ia32_comilt(float,float);
int __builtin_ia32_comile(float,float);
int __builtin_ia32_comigt(float,float);
int __builtin_ia32_comige(float,float);
int __builtin_ia32_comineq(float,float);
int __builtin_ia32_ucomieq(float,float);
int __builtin_ia32_ucomilt(float,float);
int __builtin_ia32_ucomile(float,float);
int __builtin_ia32_ucomigt(float,float);
int __builtin_ia32_ucomige(float,float);
int __builtin_ia32_ucomineq(float,float);
int __builtin_ia32_cvtss2si(float);
int __builtin_ia32_cvtss2si64(float);
int __builtin_ia32_cvtps2pi(float);
int __builtin_ia32_cvttss2si(float);
int __builtin_ia32_cvttss2si64(float);
int __builtin_ia32_cvttps2pi(float);
int __builtin_ia32_cvtsi2ss(float,float);
int __builtin_ia32_cvtsi642ss(float,float);
int __builtin_ia32_cvtsi642ss(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_movhlps(float,float);
int __builtin_ia32_cvtps2pi(float);
int __builtin_ia32_unpckhps(float,float);
int __builtin_ia32_unpcklps(float,float);
int __builtin_ia32_loadhps(float,int*);
int __builtin_ia32_storehps(int*,float);
int __builtin_ia32_movhlps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_loadlps(float,int*);
int __builtin_ia32_storelps(int*,float);
int __builtin_ia32_movmskps(float);
int __builtin_ia32_stmxcsr();
int __builtin_ia32_ldmxcsr(int);
int __builtin_ia32_loadups(float const*);
int __builtin_ia32_shufps(short,short,int);
int __builtin_ia32_vec_ext_v4sf(float,float);
int __builtin_ia32_vec_ext_v4sf(float,float);
int __builtin_ia32_storeups(float*,short);
int __builtin_ia32_pmaxsw(float,float);
int __builtin_ia32_pmaxub(float,float);
int __builtin_ia32_pminsw(float,float);
int __builtin_ia32_pminub(float,float);
int __builtin_ia32_pmovmskb(char);
int __builtin_ia32_pmulhuw(float,float);
int __builtin_ia32_maskmovq(char,char,char*);
int __builtin_ia32_pavgb(float,float);
int __builtin_ia32_pavgw(float,float);
int __builtin_ia32_psadbw(float,float);
int __builtin_ia32_movntq(unsigned long long*,unsigned long long);
int __builtin_ia32_movntps(float*,float);
int __builtin_ia32_sfence();


/* From: emmintrin.h */
int __builtin_ia32_movsd(double,double);
int __builtin_ia32_loadupd(double const *);
int __builtin_ia32_shufpd(double,double,int);
int __builtin_ia32_storeupd(double*,double);
int __builtin_ia32_vec_ext_v2df(double,int);
/* int __builtin_ia32_shufpd(); */
int __builtin_ia32_vec_ext_v4si(int,int);
int __builtin_ia32_vec_ext_v2di(long long,int);
int __builtin_ia32_addpd(double,double);
int __builtin_ia32_addsd(double,double);
int __builtin_ia32_subpd(double,double);
int __builtin_ia32_subsd(double,double);
int __builtin_ia32_mulpd(double,double);
int __builtin_ia32_mulsd(double,double);
int __builtin_ia32_divpd(double,double);
int __builtin_ia32_divsd(double,double);
int __builtin_ia32_sqrtpd(double);
int __builtin_ia32_sqrtsd(double);
int __builtin_ia32_minpd(double,double);
int __builtin_ia32_minsd(double,double);
int __builtin_ia32_maxpd(double,double);
int __builtin_ia32_maxsd(double,double);
int __builtin_ia32_andpd(double,double);
int __builtin_ia32_andnpd(double,double);
int __builtin_ia32_orpd(double,double);
int __builtin_ia32_xorpd(double,double);
int __builtin_ia32_cmpeqpd(double,double);
int __builtin_ia32_cmpltpd(double,double);
int __builtin_ia32_cmplepd(double,double);
int __builtin_ia32_cmpgtpd(double,double);
int __builtin_ia32_cmpgepd(double,double);
int __builtin_ia32_cmpneqpd(double,double);
int __builtin_ia32_cmpnltpd(double,double);
int __builtin_ia32_cmpnlepd(double,double);
int __builtin_ia32_cmpngtpd(double,double);
int __builtin_ia32_cmpngepd(double,double);
int __builtin_ia32_cmpordpd(double,double);
int __builtin_ia32_cmpunordpd(double,double);
int __builtin_ia32_cmpeqsd(double,double);
int __builtin_ia32_cmpltsd(double,double);
int __builtin_ia32_cmplesd(double,double);
int __builtin_ia32_cmpltsd(double,double);
int __builtin_ia32_cmplesd(double,double);
int __builtin_ia32_cmpneqsd(double,double);
int __builtin_ia32_cmpnltsd(double,double);
int __builtin_ia32_cmpnlesd(double,double);
int __builtin_ia32_cmpordsd(double,double);
int __builtin_ia32_cmpunordsd(double,double);
int __builtin_ia32_comisdeq(double,double);
int __builtin_ia32_comisdlt(double,double);
int __builtin_ia32_comisdle(double,double);
int __builtin_ia32_comisdgt(double,double);
int __builtin_ia32_comisdge(double,double);
int __builtin_ia32_comisdg(double,double);
int __builtin_ia32_comisdneq(double,double);
int __builtin_ia32_ucomisdeq(double,double);
int __builtin_ia32_ucomisdlt(double,double);
int __builtin_ia32_ucomisdle(double,double);
int __builtin_ia32_ucomisdgt(double,double);
int __builtin_ia32_ucomisdge(double,double);
int __builtin_ia32_ucomisdneq(double,double);
int __builtin_ia32_loaddqu(char const*);
int __builtin_ia32_storedqu(char*, char);
int __builtin_ia32_cvtdq2pd(int);
int __builtin_ia32_cvtdq2ps(int);
int __builtin_ia32_cvtpd2dq(double);
int __builtin_ia32_cvtpd2pi(double);
int __builtin_ia32_cvtpd2ps(double);
int __builtin_ia32_cvttpd2dq(double);
int __builtin_ia32_cvttpd2pi(double);
int __builtin_ia32_cvtpi2pd(int);
int __builtin_ia32_cvtps2dq(double);
int __builtin_ia32_cvttps2dq(double);
int __builtin_ia32_cvtps2pd(float);
int __builtin_ia32_cvtsd2si(double);
int __builtin_ia32_cvtsd2si64(double);
int __builtin_ia32_cvtsd2si64(double);
int __builtin_ia32_cvttsd2si(double);
int __builtin_ia32_cvttsd2si64(double);
int __builtin_ia32_cvtsd2ss(int,int);
int __builtin_ia32_cvtsi2sd(int,int);
int __builtin_ia32_cvtsi642sd(int,int);
int __builtin_ia32_cvtsi642sd(int,int);
int __builtin_ia32_cvtss2sd(int,int);
int __builtin_ia32_unpcklpd(int,int);
int __builtin_ia32_unpckhpd(double,double);
int __builtin_ia32_loadhpd(double, double const *);
int __builtin_ia32_loadlpd(double, double const *);
int __builtin_ia32_movmskpd(double);
int __builtin_ia32_packsswb128(short,short);
int __builtin_ia32_packssdw128(short,short);
int __builtin_ia32_packuswb128(short,short);
int __builtin_ia32_punpckhbw128(short,short);
int __builtin_ia32_punpckhwd128(int,int);
int __builtin_ia32_punpckhdq128(int,int);
int __builtin_ia32_punpckhqdq128(int,int);
int __builtin_ia32_punpcklbw128(int,int);
int __builtin_ia32_punpcklwd128(int,int);
int __builtin_ia32_punpckldq128(int,int);
int __builtin_ia32_punpcklqdq128(int,int);
int __builtin_ia32_paddb128(long long,long long);
int __builtin_ia32_paddw128(short,short);
int __builtin_ia32_paddd128(int,int);
int __builtin_ia32_paddq128(long long,long long);
int __builtin_ia32_paddsb128(long long,long long);
int __builtin_ia32_paddsw128(short,short);
int __builtin_ia32_paddusb128(char,char);
int __builtin_ia32_paddusw128(short,short);
int __builtin_ia32_psubb128(char,char);
int __builtin_ia32_psubw128(short,short);
int __builtin_ia32_psubd128(int,int);
int __builtin_ia32_psubq128(double,double);
int __builtin_ia32_psubsb128(char,char);
int __builtin_ia32_psubsw128(short,short);
int __builtin_ia32_psubusb128(char,char);
int __builtin_ia32_psubusw128(short,short);
int __builtin_ia32_pmaddwd128(short,short);
int __builtin_ia32_pmulhw128(short,short);
int __builtin_ia32_pmullw128(short,short);
int __builtin_ia32_pmuludq(int,int);
int __builtin_ia32_pmuludq128(int,int);
int __builtin_ia32_psllwi128(short,short);
int __builtin_ia32_pslldi128(int,int);
int __builtin_ia32_psllqi128(int,int);
int __builtin_ia32_psrawi128(short,short);
int __builtin_ia32_psradi128(short,short);
int __builtin_ia32_psrlwi128(short,short);
int __builtin_ia32_psrldi128(short,short);
int __builtin_ia32_psrlqi128(short,short);
int __builtin_ia32_psllw128(short,short);
int __builtin_ia32_pslld128(short,short);
int __builtin_ia32_psllq128(short,short);
int __builtin_ia32_psraw128(short,short);
int __builtin_ia32_psrad128(short,short);
int __builtin_ia32_psrlw128(short,short);
int __builtin_ia32_psrld128(short,short);
int __builtin_ia32_psrlq128(short,short);
int __builtin_ia32_pand128(int,int);
int __builtin_ia32_pandn128(int,int);
int __builtin_ia32_por128(int,int);
int __builtin_ia32_pxor128(int,int);
int __builtin_ia32_pcmpeqb128(char,char);
int __builtin_ia32_pcmpeqw128(short,short);
int __builtin_ia32_pcmpeqd128(short,short);
int __builtin_ia32_pcmpgtb128(char,char);
int __builtin_ia32_pcmpgtw128(short,short);
int __builtin_ia32_pcmpgtd128(int,int);
int __builtin_ia32_pcmpgtb128(char,char);
int __builtin_ia32_pcmpgtw128(short,short);
int __builtin_ia32_pmaxsw128(short,short);
int __builtin_ia32_pmaxub128(char,char);
int __builtin_ia32_pminsw128(short,short);
int __builtin_ia32_pminub128(char,char);
int __builtin_ia32_pmovmskb128(char);
int __builtin_ia32_pmulhuw128(short,short);
int __builtin_ia32_maskmovdqu(char,char,char*);
int __builtin_ia32_pavgb128(char,char);
int __builtin_ia32_pavgw128(short,short);
int __builtin_ia32_psadbw128(char,char);
int __builtin_ia32_movnti(int*,int);
int __builtin_ia32_movntdq(long long*,long long);
int __builtin_ia32_movntpd(double*,double);
int __builtin_ia32_clflush(void const *);
int __builtin_ia32_lfence();
int __builtin_ia32_mfence();

// Builtin functions specific to GNU 4.4.1 (likely 4.4.x)
int __builtin_ia32_psllwi(short,int);
int __builtin_ia32_pslldi(int,int);
int __builtin_ia32_psllqi(long long,int);
int __builtin_ia32_psrawi(short,int);
int __builtin_ia32_psradi(int,int);
int __builtin_ia32_psrlwi(int,int);
int __builtin_ia32_psrldi(int,int);
int __builtin_ia32_psrlqi(long long,int);

// DQ (12/14/2016): Commented out
#endif

// DQ (8/25/2012): Moved this to avoid their output in the debugging mode for ROSE.
// As a result of this move they are not output in the DOT graphs where the macro
// SKIP_ROSE_BUILTIN_DECLARATIONS is defined.

// Change from Tristan (I think): I think these definitions will break on 32-bit 
// systems The built-in declarations from gcc actually specify the bit length 
// explictly e.g. int32_t __builtin_bswap32 (int32_t x)
// However, integer types such as int32_t are not defined here.
// int __builtin_bswap32 (int x);
/* long int __builtin_bswap64 (long int x); */


/* DQ (9/12/2016): Comment out to use new automated generation of builtin functions. */
#endif

/* DQ (12/17/2016): Not clear why I don't appear to need this now. it could be that at least 
   one declaration is being seen so that this specific variable is no longer required. 
*/
/* DQ (12/15/2016): We need a variable to be from this file so that we can support internal checking. */
/* int __frontend_specific_variable_to_provide_header_file_path; */

/* DQ (9/6/2016): We need size_t to be defined, but we only want it defined so that the builtin functions 
   will make sense, then we will undefine it and allow the header files to define it properly.
 */
#undef size_t

/* matching else for SKIP_ROSE_BUILTIN_DECLARATIONS */
#else

/* When compiling using -DSKIP_ROSE_BUILTIN_DECLARATIONS we need to have a variable
   defined in this faile so that we can locate the file in the AST and obtain the
   absolute path of this front-end specific header file.  This allows us to mark
   all IR nodes in the AST as being front-end specific if they originate from this
   header file. The following variable guarentees that a variable declaration from
   this file will exist when -DSKIP_ROSE_BUILTIN_DECLARATIONS is used.  The 
   -DSKIP_ROSE_BUILTIN_DECLARATIONS option is typically used to reduce the size of
   the AST and permit visualization of the whole AST using DOT (which can't layout
   a graph containing too many IR nodes).
 */
int __frontend_specific_variable_to_provide_header_file_path;

/* matching endif for SKIP_ROSE_BUILTIN_DECLARATIONS */
#endif
#endif
