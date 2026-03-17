#ifndef STREAM_CIPHER_CONFIG_H
#define STREAM_CIPHER_CONFIG_H

/* Compiler detection */

#if defined(__GNUC__)
#  define STMCPHR_ON_GNUC 1
#  if defined(__clang__)
#    define STMCPHR_ON_CLANG 1
#  else
#    define STMCPHR_ON_GCC 1
#  endif
#elif defined(_MSC_VER)
#  define STMCPHR_ON_MSVC 1
#else
#  error Unsupported compiler
#endif

/* Detecting endianness */

#if STMCPHR_ON_GNUC
#  if defined(__BYTE_ORDER__)
#    if   __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#      define STMCPHR_IS_LITTLE 1
#      define STMCPHR_IS_BIG    0
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#      define STMCPHR_IS_LITTLE 0
#      define STMCPHR_IS_BIG    1
#    else
#      error Unknown endianness
#    endif
#  else
#    error Not defined __BYTE_ORDER__
#  endif
#elif STMCPHR_ON_MSVC
#  define STMCPHR_IS_LITTLE 1
#  define STMCPHR_IS_BIG    0
#else
#  error Unsupported compiler
#endif

/* Turning off warning "-Wlong-long" on GNUC */

#if STMCPHR_ON_GNUC && __STDC_VERSION__ < 199901L
#  define STMCPHR_U64_WARN_BEGIN \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wlong-long\"")
#  define STMCPHR_U64_WARN_END \
    _Pragma("GCC diagnostic pop")
#else
#  define STMCPHR_U64_WARN_BEGIN
#  define STMCPHR_U64_WARN_END
#endif

/* Define fixed width integer types */

#if __STDC_VERSION__ >= 199901L

#include <stdint.h>

typedef uint8_t stmcphr_u8_t;
typedef uint16_t stmcphr_u16_t;
typedef uint32_t stmcphr_u32_t;
typedef uint64_t stmcphr_u64_t;

#else /* C89 */

#include <limits.h>

typedef unsigned char stmcphr_u8_t;
typedef unsigned short stmcphr_u16_t;

#if ULONG_MAX == 0xFFFFFFFFul
typedef unsigned long stmcphr_u32_t;
#else
typedef unsigned int  stmcphr_u32_t;
#endif

#if STMCPHR_ON_GNUC
STMCPHR_U64_WARN_BEGIN
typedef unsigned long long stmcphr_u64_t;
STMCPHR_U64_WARN_END
#elif STMCPHR_ON_MSVC
typedef unsigned __int64 stmcphr_u64_t;
#else
#  error Unsupported copmiler
#endif

#endif /* __STDC_VERSION__ >= 199901L */

/* Integer byte swap functions */

#if STMCPHR_ON_GNUC
#  define stmcphr_bswap16 __builtin_bswap16
#  define stmcphr_bswap32 __builtin_bswap32
#  define stmcphr_bswap64 __builtin_bswap64
#elif STMCPHR_ON_MSVC
#  include <stdlib.h>
#  define stmcphr_bswap16 _byteswap_ushort
#  define stmcphr_bswap32 _byteswap_ulong
#  define stmcphr_bswap64 _byteswap_uint64
#else
#  error Unsupported compiler
#endif

/* If condition for preprocessor */

#define STMCPHR_CONCAT_(left, right) left ## right
#define STMCPHR_CONCAT(left, right) STMCPHR_CONCAT_(left, right)

#define STMCPHR_IF_0(stmt)
#define STMCPHR_IF_1(stmt) stmt
#define STMCPHR_IF(cond, stmt) STMCPHR_CONCAT(STMCPHR_IF_, cond)(stmt)

#define STMCPHR_IF_LITTLE(stmt) STMCPHR_IF(STMCPHR_IS_LITTLE, stmt)
#define STMCPHR_IF_BIG(stmt) STMCPHR_IF(STMCPHR_IS_BIG, stmt)

/* Byte swapping one, pair and blocks */

#define STMCPHR_BSWAP_B_STMT_1(bits, array) \
    (array)[0] = stmcphr_bswap##bits((array)[0]);
#define STMCPHR_BSWAP_B_STMT_2(bits, array) \
    STMCPHR_BSWAP_B_STMT_1(bits, array)     \
    (array)[1] = stmcphr_bswap##bits((array)[1]);
#define STMCPHR_BSWAP_B_STMT_3(bits, array) \
    STMCPHR_BSWAP_B_STMT_2(bits, array)     \
    (array)[2] = stmcphr_bswap##bits((array)[2]);
#define STMCPHR_BSWAP_B_STMT_4(bits, array) \
    STMCPHR_BSWAP_B_STMT_3(bits, array)     \
    (array)[3] = stmcphr_bswap##bits((array)[3]);
#define STMCPHR_BSWAP_B_STMT_5(bits, array) \
    STMCPHR_BSWAP_B_STMT_4(bits, array)     \
    (array)[4] = stmcphr_bswap##bits((array)[4]);
#define STMCPHR_BSWAP_B_STMT_6(bits, array) \
    STMCPHR_BSWAP_B_STMT_5(bits, array)     \
    (array)[5] = stmcphr_bswap##bits((array)[5]);
#define STMCPHR_BSWAP_B_STMT_7(bits, array) \
    STMCPHR_BSWAP_B_STMT_6(bits, array)     \
    (array)[6] = stmcphr_bswap##bits((array)[6]);
#define STMCPHR_BSWAP_B_STMT_8(bits, array) \
    STMCPHR_BSWAP_B_STMT_7(bits, array)     \
    (array)[7] = stmcphr_bswap##bits((array)[7]);

#define STMCPHR_BSWAP_B_ONE(bits, value) do { \
    (value) = stmcphr_bswap##bits(value); \
} while (0)

#define STMCPHR_BSWAP_BxN(number, bits, array) do { \
    stmcphr_bswap_b_stmt_##number(bits, array) \
} while (0)

/* Bit rotation functions */

#ifdef STMCPHR_USE_ROTL32
static stmcphr_u32_t STMCPHR_USE_ROTL32(stmcphr_u32_t n, stmcphr_u32_t s)
    { s &= 31; return n << s | n >> (-s & 31); }
#endif

#ifdef STMCPHR_USE_ROTR32
static stmcphr_u32_t STMCPHR_USE_ROTR32(stmcphr_u32_t n, stmcphr_u32_t s)
    { s &= 31; return n >> s | n << (-s & 31); }
#endif

#ifdef STMCPHR_USE_ROTL64
static stmcphr_u64_t STMCPHR_USE_ROTL64(stmcphr_u64_t n, stmcphr_u64_t s)
    { s &= 63; return n << s | n >> (-s & 63); }
#endif

#ifdef STMCPHR_USE_ROTR64
static stmcphr_u64_t STMCPHR_USE_ROTR64(stmcphr_u64_t n, stmcphr_u64_t s)
    { s &= 63; return n >> s | n << (-s & 63); }
#endif

#endif /* STREAM_CIPHER_CONFIG_H */