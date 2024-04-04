/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Compile-time options displayed in -v output */
#define AIDECOMPILEOPTIONS "WITH_MMAP\nWITH_PCRE\nWITH_ZLIB\nWITH_GCRYPT\n"

/* package version */
//#define AIDEVERSION "0.17.4"

/* syslog ident */
#define AIDE_IDENT "idtt"

/* syslog logopt */
#define AIDE_LOGOPT LOG_CONS

/* syslog facility */
#define AIDE_SYSLOG_FACILITY LOG_LOCAL0

/* big endian */
/* #undef BIG_ENDIAN_HOST */

/* Location of configuration file */
#define CONFIG_FILE "/usr/local/etc/aide.conf"

/* Default location of signature database */
#define DEFAULT_DB "file:/usr/local/etc/aide.db"

/* Default output location for newly-generated signature database */
#define DEFAULT_DB_OUT "file:/usr/local/etc/aide.db.new"

/* Define to 1 if you have the `asprintf' function. */
#define HAVE_ASPRINTF 1

/* Define to 1 if the system has the type `byte'. */
/* #undef HAVE_BYTE */

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the `fcntl' function. */
#define HAVE_FCNTL 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `ftruncate' function. */
#define HAVE_FTRUNCATE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `audit' library (-laudit). */
/* #undef HAVE_LIBAUDIT */

/* Define to 1 if you have the `gcrypt' library (-lgcrypt). */
#define HAVE_LIBGCRYPT 1

/* Define to 1 if you have the <libintl.h> header file. */
/* #undef HAVE_LIBINTL_H */

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define to 1 if the type `long double' works and has more range or precision
   than `double'. */
#define HAVE_LONG_DOUBLE 1

/* Define to 1 if the type `long double' works and has more range or precision
   than `double'. */
#define HAVE_LONG_DOUBLE_WIDER 1

/* mhash has whirlpool */
/* #undef HAVE_MHASH_WHIRLPOOL */

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Define to 1 if you have the O_NOATIME flag. */
/* #undef HAVE_O_NOATIME */

/* Define to 1 if you have the `posix_fadvise' function. */
#define HAVE_POSIX_FADVISE 1

/* Define to 1 if you have the `readdir' function. */
#define HAVE_READDIR 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define to 1 if you have the `strnstr' function. */
/* #undef HAVE_STRNSTR */

/* Define to 1 if you have the `strtoimax' function. */
#define HAVE_STRTOIMAX 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* syslog available? */
#define HAVE_SYSLOG 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if the system has the type `u16'. */
/* #undef HAVE_U16 */

/* Define to 1 if the system has the type `u32'. */
/* #undef HAVE_U32 */

/* Define to 1 if the system has the type `u64'. */
/* #undef HAVE_U64 */

/* Define to 1 if the system has the type `ulong'. */
#define HAVE_ULONG 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type `ushort'. */
#define HAVE_USHORT 1

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define to 1 if you have the `va_copy' function. */
/* #undef HAVE_VA_COPY */

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `vsyslog' function. */
#define HAVE_VSYSLOG 1

/* Define to 1 if you have the <zlib.h> header file. */
#define HAVE_ZLIB_H 1

/* Define to 1 if you have the `__va_copy' function. */
/* #undef HAVE___VA_COPY */

/* little endian */
#define LITTLE_ENDIAN_HOST 1

/* Name of package */
#define PACKAGE "aide"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "aide"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "aide 0.17.4"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "aide"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.17.4"

/* path to prelink */
/* #undef PRELINK_PATH */

/* The size of `blkcnt_t', as computed by sizeof. */
#define SIZEOF_BLKCNT_T 8

/* The size of `gid_t', as computed by sizeof. */
#define SIZEOF_GID_T 4

/* The size of `ino_t', as computed by sizeof. */
#define SIZEOF_INO_T 8

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `nlink_t', as computed by sizeof. */
#define SIZEOF_NLINK_T 4

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 8

/* The size of `uid_t', as computed by sizeof. */
#define SIZEOF_UID_T 4

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* The size of `unsigned long long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* The size of `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* syslog priority */
#define SYSLOG_PRIORITY LOG_NOTICE

/* Define if LOCALE support should be used */
/* #undef USE_LOCALE */

/* Version number of package */
#define VERSION "0.17.4"

/* use ACL */
/* #undef WITH_ACL */

/* use audit library */
/* #undef WITH_AUDIT */

/* use capabilities */
/* #undef WITH_CAPABILITIES */

/* Define to 1 if libcurl is available */
/* #undef WITH_CURL */

/* use e2fsattrs */
/* #undef WITH_E2FSATTRS */

/* use gcrypt library */
#define WITH_GCRYPT 1

/* use mhash library */
/* #undef WITH_MHASH */

/* use POSIX ACLs */
/* #undef WITH_POSIX_ACL */

/* use prelink */
/* #undef WITH_PRELINK */

/* use SELinux */
/* #undef WITH_SELINUX */

/* use xattr */
/* #undef WITH_XATTR */

/* use zlib */
#define WITH_ZLIB 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */
