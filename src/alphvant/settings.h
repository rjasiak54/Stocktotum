#pragma once
#include <filesystem>
// settings for alphvant
// TODO(ipkn) replace with runtime config. libucl?

/* #ifdef - enables debug mode */
// #define ALPHVANT_ENABLE_DEBUG

/* #ifdef - enables logging */
#define ALPHVANT_ENABLE_LOGGING

/* #ifdef - enforces section 5.2 and 6.1 of RFC6455 (only accepting masked
 * messages from clients) */
// #define ALPHVANT_ENFORCE_WS_SPEC

/* #define - specifies log level */
/*
    Debug       = 0
    Info        = 1
    Warning     = 2
    Error       = 3
    Critical    = 4

    default to INFO
*/
#ifndef ALPHVANT_LOG_LEVEL
#define ALPHVANT_LOG_LEVEL 1
#endif

#ifndef ALPHVANT_STATIC_DIRECTORY
#define ALPHVANT_STATIC_DIRECTORY "static/"
#endif
#ifndef ALPHVANT_STATIC_ENDPOINT
#define ALPHVANT_STATIC_ENDPOINT "/static/<path>"
#endif

// compiler flags
#if defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#define ALPHVANT_CAN_USE_CPP14
#endif
#if __cplusplus >= 201402L
#define ALPHVANT_CAN_USE_CPP14
#endif

#if defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#define ALPHVANT_CAN_USE_CPP17
#endif
#if __cplusplus >= 201703L
#define ALPHVANT_CAN_USE_CPP17
#if defined(__GNUC__) && __GNUC__ < 8
#define ALPHVANT_FILESYSTEM_IS_EXPERIMENTAL
#endif
#endif

#if defined(_MSC_VER)
#if _MSC_VER < 1900
#define ALPHVANT_MSVC_WORKAROUND
#define constexpr const
#define noexcept throw()
#endif
#endif

#if defined(__GNUC__) && __GNUC__ == 8 && __GNUC_MINOR__ < 4
#if __cplusplus > 201103L
#define ALPHVANT_GCC83_WORKAROUND
#else
#error                                                                         \
    "GCC 8.1 - 8.3 has a bug that prevents Crow from compiling with C++11. Please update GCC to > 8.3 or use C++ > 11."
#endif
#endif