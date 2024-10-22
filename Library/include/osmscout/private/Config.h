#ifndef OSMSCOUT_PRIVATE_CONFIG_H
#define OSMSCOUT_PRIVATE_CONFIG_H

/* Support Altivec instructions */
#ifndef HAVE_ALTIVEC
/* #undef HAVE_ALTIVEC */
#endif

/* Support AVX (Advanced Vector Extensions) instructions */
#ifndef HAVE_AVX
#define HAVE_AVX 1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef HAVE_DLFCN_H
/* #undef HAVE_DLFCN_H */
#endif

/* Define to 1 if you have the <fcntl.h> header file. */
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#ifndef HAVE_FSEEKO
#define HAVE_FSEEKO 1
#endif

/* Define to 1 if _fseeki64 exists and is declared. */
#ifndef HAVE__FSEEKI64
#define HAVE__FSEEKI64 1
#endif

/* Define to 1 if _ftelli64 exists and is declared. */
#ifndef HAVE__FTELLI64
#define HAVE__FTELLI64 1
#endif

/* #undef _LARGEFILE_SOURCE */
/* #undef _LARGE_FILES */
#define _FILE_OFFSET_BITS 64

/* Define to 1 if the system has the type `int16_t'. */
#ifndef HAVE_INT16_T
#define HAVE_INT16_T 1
#endif

/* Define to 1 if the system has the type `int32_t'. */
#ifndef HAVE_INT32_T
#define HAVE_INT32_T 1
#endif

/* Define to 1 if the system has the type `int64_t'. */
#ifndef HAVE_INT64_T
#define HAVE_INT64_T 1
#endif

/* Define to 1 if the system has the type `int8_t'. */
#ifndef HAVE_INT8_T
#define HAVE_INT8_T 1
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef HAVE_INTTYPES_H
#define HAVE_INTTYPES_H 1
#endif

/* Define to 1 if the system has the type `long long'. */
#ifndef HAVE_LONG_LONG
#define HAVE_LONG_LONG 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H
#define HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the `mmap' function. */
#ifndef HAVE_MMAP
/* #undef HAVE_MMAP */
#endif

/* Support mmx instructions */
#ifndef HAVE_MMX
#define HAVE_MMX 1
#endif

/* Define to 1 if you have the `posix_fadvise' function. */
#ifndef HAVE_POSIX_FADVISE
/* #undef HAVE_POSIX_FADVISE */
#endif

/* Define to 1 if you have the `posix_madvise' function. */
#ifndef HAVE_POSIX_MADVISE
/* #undef HAVE_POSIX_MADVISE */
#endif

/* Support SSE (Streaming SIMD Extensions) instructions */
#ifndef HAVE_SSE
/* #undef HAVE_SSE */
#endif

/* Support SSE2 (Streaming SIMD Extensions 2) instructions */
#ifndef HAVE_SSE2
/* #undef HAVE_SSE2 */
#endif

/* Support SSE3 (Streaming SIMD Extensions 3) instructions */
#ifndef HAVE_SSE3
/* #undef HAVE_SSE3 */
#endif

/* Support SSSE4.1 (Streaming SIMD Extensions 4.1) instructions */
#ifndef HAVE_SSE4_1
/* #undef HAVE_SSE4_1 */
#endif

/* Support SSSE4.2 (Streaming SIMD Extensions 4.2) instructions */
#ifndef HAVE_SSE4_2
/* #undef HAVE_SSE4_2 */
#endif

/* Support SSSE3 (Supplemental Streaming SIMD Extensions 3) instructions */
#ifndef HAVE_SSSE3
/* #undef HAVE_SSSE3 */
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef HAVE_STRINGS_H
#define HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef HAVE_SYS_TIME_H
#define HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef HAVE_SYS_TYPES_H
#define HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if the system has the type `uint16_t'. */
#ifndef HAVE_UINT16_T
#define HAVE_UINT16_T 1
#endif

/* Define to 1 if the system has the type `uint32_t'. */
#ifndef HAVE_UINT32_T
#define HAVE_UINT32_T 1
#endif

/* Define to 1 if the system has the type `uint64_t'. */
#ifndef HAVE_UINT64_T
#define HAVE_UINT64_T 1
#endif

/* Define to 1 if the system has the type `uint8_t'. */
#ifndef HAVE_UINT8_T
#define HAVE_UINT8_T 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the <codecvt> header file. */
#ifndef HAVE_CODECVT
#define HAVE_CODECVT 1
#endif

/* Define to 1 if the system has the type `unsigned long long'. */
#ifndef HAVE_UNSIGNED_LONG_LONG
#define HAVE_UNSIGNED_LONG_LONG 1
#endif

/* Define to 1 or 0, depending whether the compiler supports simple visibility
   declarations. */
#ifndef HAVE_VISIBILITY
#define HAVE_VISIBILITY 1
#endif

/* Define to 1 or 0, depending whether the compiler supports parallel execution algorithms. */
#ifndef HAVE_STD_EXECUTION
/* #undef HAVE_STD_EXECUTION */
#endif

/* Define to 1 or 0, depending whether the tbb::task_scheduler_init exists. */
#ifndef TBB_HAS_SCHEDULER_INIT
/* #undef TBB_HAS_SCHEDULER_INIT */
#endif

/* int16_t is available */
#ifndef OSMSCOUT_HAVE_INT16_T
#define OSMSCOUT_HAVE_INT16_T 1
#endif

/* int32_t is available */
#ifndef OSMSCOUT_HAVE_INT32_T
#define OSMSCOUT_HAVE_INT32_T 1
#endif

/* int64_t is available */
#ifndef OSMSCOUT_HAVE_INT64_T
#define OSMSCOUT_HAVE_INT64_T 1
#endif

/* int8_t is available */
#ifndef OSMSCOUT_HAVE_INT8_T
#define OSMSCOUT_HAVE_INT8_T 1
#endif

/* long long is available */
#ifndef OSMSCOUT_HAVE_LONG_LONG
#define OSMSCOUT_HAVE_LONG_LONG 1
#endif

/* SSE2 processor extension available */
#ifndef OSMSCOUT_HAVE_SSE2
/* #undef OSMSCOUT_HAVE_SSE2 */
#endif

/* system header <stdint.h> is available */
#ifndef OSMSCOUT_HAVE_STDINT_H
#define OSMSCOUT_HAVE_STDINT_H 1
#endif

/* std::wstring is available */
#ifndef OSMSCOUT_HAVE_STD_WSTRING
/* #undef OSMSCOUT_HAVE_STD_WSTRING */
#endif

/* uint16_t is available */
#ifndef OSMSCOUT_HAVE_UINT16_T
#define OSMSCOUT_HAVE_UINT16_T 1
#endif

/* uint32_t is available */
#ifndef OSMSCOUT_HAVE_UINT32_T
#define OSMSCOUT_HAVE_UINT32_T 1
#endif

/* uint64_t is available */
#ifndef OSMSCOUT_HAVE_UINT64_T
#define OSMSCOUT_HAVE_UINT64_T 1
#endif

/* uint8_t is available */
#ifndef OSMSCOUT_HAVE_UINT8_T
#define OSMSCOUT_HAVE_UINT8_T 1
#endif

/* unsigned long long is available */
#ifndef OSMSCOUT_HAVE_ULONG_LONG
#define OSMSCOUT_HAVE_ULONG_LONG 1
#endif

/* The size of `wchar_t', as computed by sizeof. */
#ifndef SIZEOF_WCHAR_T
#define SIZEOF_WCHAR_T 2
#endif

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#ifndef LT_OBJDIR
#define LT_OBJDIR ".libs/"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "tim@teulings.org"
#endif

/* Define to the full name of this package. */
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "libosmscout"
#endif

/* Define to the full name and version of this package. */
#ifndef PACKAGE_STRING
#define PACKAGE_STRING "libosmscout 1.1.1"
#endif

/* Define to the one symbol short name of this package. */
#ifndef PACKAGE_TARNAME
#define PACKAGE_TARNAME "libosmscout"
#endif

/* Define to the home page for this package. */
#ifndef PACKAGE_URL
#define PACKAGE_URL "http://libosmscout.sourceforge.net/"
#endif

/* Define to the version of this package. */
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "1.1.1"
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef STDC_HEADERS
#define STDC_HEADERS 1
#endif

/* Define to 1 if you have OpenMP. */
#ifndef OSMSCOUT_HAVE_OPENMP
#define OSMSCOUT_HAVE_OPENMP 1
#endif

/* libmarisa detected */
#ifndef OSMSCOUT_IMPORT_HAVE_LIB_MARISA
/* #undef OSMSCOUT_IMPORT_HAVE_LIB_MARISA */
#endif

/* libprotobuf detected */
#ifndef HAVE_LIB_PROTOBUF
/* #undef HAVE_LIB_PROTOBUF */
#endif

/* libxml detected */
#ifndef HAVE_LIB_XML
/* #undef HAVE_LIB_XML */
#endif

/* zlib detected */
#ifndef HAVE_LIB_ZLIB
#define HAVE_LIB_ZLIB 1
#endif

/* libagg detected */
#ifndef HAVE_LIB_AGG
/* #undef HAVE_LIB_AGG */
#endif

/* freetype detected */
#ifndef HAVE_LIB_FREETYPE
/* #undef HAVE_LIB_FREETYPE */
#endif

/* cairo detected */
#ifndef HAVE_LIB_CAIRO
/* #undef HAVE_LIB_CAIRO */
#endif

/* pango detected */
#ifndef HAVE_LIB_PANGO
/* #undef HAVE_LIB_PANGO */
#endif

/* pango detected */
#ifndef OSMSCOUT_MAP_CAIRO_HAVE_LIB_PANGO
/* #undef OSMSCOUT_MAP_CAIRO_HAVE_LIB_PANGO */
#endif

/* pango detected */
#ifndef OSMSCOUT_MAP_SVG_HAVE_LIB_PANGO
/* #undef OSMSCOUT_MAP_SVG_HAVE_LIB_PANGO */
#endif

#ifndef OSMSCOUT_PTHREAD_NAME
/* Threads are pthreads and non-posix setname is available */
#define OSMSCOUT_PTHREAD_NAME
#endif

#endif // OSMSCOUT_PRIVATE_CONFIG_H
