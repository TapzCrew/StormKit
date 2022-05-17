#pragma once

#if defined(__cplusplus)
    #if !(_MSC_VER >= 1900 || __cplusplus >= 202002L)
        #error "Stormkit need a C++20 compiler"
    #endif
#else
    #error "Stormkit need a c++ compiler"
#endif

#if defined(_MSC_VER)
    #pragma warning(disable : 4251)
    #define STORMKIT_COMPILER_MSVC "MSVC " + std::to_string(_MSC_VER)
    #define STORMKIT_COMPILER STORMKIT_COMPILER_MSVC
    #define STORMKIT_EXPORT __declspec(dllexport)
    #define STORMKIT_IMPORT __declspec(dllimport)
    #define STORMKIT_PRIVATE
#elif defined(__MINGW32__)
    #define STORMKIT_EXPORT __declspec(dllexport)
    #define STORMKIT_IMPORT __declspec(dllimport)
    #define STORMKIT_PRIVATE
#else
    #define STORMKIT_IMPORT [[gnu::visibility("default")]]
    #define STORMKIT_EXPORT [[gnu::visibility("default")]]
    #define STORMKIT_PRIVATE [[gnu::visibility("hidden")]]
#endif

#if defined(__MINGW32__)
    #define STORMKIT_COMPILER STORMKIT_COMPILER_MINGW
    #if defined(__clang__)
        #define STORMKIT_COMPILER_CLANG "MinGW Clang " + __clang_version__
        #define STORMKIT_COMPILER STORMKIT_COMPILER_CLANG
    #elif defined(__GNUC__) || defined(__GNUG__)
        #define STORMKIT_COMPILER_GCC                                                              \
            "MinGW GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + \
                "." + std::to_string(__GNUC_PATCHLEVEL__)
        #define STORMKIT_COMPILER_MINGW STORMKIT_COMPILER_GCC
    #endif
    #define STORMKIT_COMPILER_MINGW STORMKIT_COMPILER
#elif defined(__clang__) && !defined(_MSC_VER)
    #define STORMKIT_COMPILER_CLANG "Clang " + __clang_version__
    #define STORMKIT_COMPILER STORMKIT_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define STORMKIT_COMPILER_GCC                                                              \
        "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + "." + \
            std::to_string(__GNUC_PATCHLEVEL__)
    #define STORMKIT_COMPILER STORMKIT_COMPILER_GCC
#endif

#if defined(__SWITCH__)
    #define STORMKIT_OS_NX "Nintendo Switch"
    #define STORMKIT_BITS_64
    #define STORMKIT_OS STORMKIT_OS_NX
#elif defined(_WIN64)
    #define STORMKIT_OS_WIN64 "Windows 64 bits" A
    #define STORMKIT_OS_WINDOWS STORMKIT_OS_WIN64
    #define STORMKIT_BITS_64
    #define STORMKIT_OS STORMKIT_OS_WIN64
#elif defined(_WIN32)
extern "C" {
    #include <windows.h>
}
    #define STORMKIT_OS_WIN32 "Windows 32 bits"
    #define STORMKIT_OS_WINDOWS STORMKIT_OS_WIN32
    #define STORMKIT_BITS_32
    #define STORMKIT_PRIVATE
    #define STORMKIT_OS STORMKIT_OS_WIN32
#elif defined(__ANDROID__)
    #define STORMKIT_OS_ANDROID "Android"
    #define STORMKIT_OS STORMKIT_OS_ANDROID
#elif defined(__linux__)
    #if defined(__x86_64__)
        #define STORMKIT_OS_LINUX64 "Linux 64 bits"
        #define STORMKIT_OS_LINUX STORMKIT_OS_LINUX64
        #define STORMKIT_BITS_64
    #else
        #define STORMKIT_OS_LINUX32 "Linux 32 bits"
        #define STORMKIT_OS_LINUX STORMKIT_OS_LINUX32
        #define STORMKIT_BITS_32
    #endif

    #define STORMKIT_OS STORMKIT_OS_LINUX
#elif defined(__MACH__)
extern "C" {
    #include <TargetConditionals.h>
}
    #if TARGET_OS_IPHONE
        #define STORMKIT_OS_IOS "IOS"
        #define STORMKIT_OS_APPLE STORMKIT_OS_IOS
    #elif TARGET_OS_SIMULATOR
        #define STORMKIT_OS_IOS_SIMULATOR "IOS Simulator"
        #define STORMKIT_OS_IOS STORMKIT_OS_IOS_SIMULATOR
        #define STORMKIT_OS_APPLE STORMKIT_OS_IOS
    #elif TARGET_OS_MAC
        #define STORMKIT_OS_MACOS "OSX 64 bits"
        #define STORMKIT_OS_APPLE STORMKIT_OS_MACOS
    #endif
    #define STORMKIT_OS STORMKIT_OS_APPLE
#else
    #error "Targeted platform not supported !"
#endif

#ifndef STORMKIT_STATIC
    #ifdef STORMKIT_BUILD
        #define STORMKIT_PUBLIC STORMKIT_EXPORT
    #else
        #define STORMKIT_PUBLIC STORMKIT_IMPORT
    #endif
#else
    #define STORMKIT_PUBLIC
#endif

#ifdef _POSIX_VERSION
    #define STORMKIT_POSIX
#endif

#if STORMKIT_BUILD_TYPE == 1
    #define STORMKIT_BUILD_DEBUG
    #define STORMKIT_BUILD_STRING "Debug"
#else
    #define STORMKIT_BUILD_RELEASE
    #define STORMKIT_BUILD_STRING "Release"
#endif

#define UNUSED(x) (void)x;

#define STORMKIT_STRINGIFY_DETAILS(x) #x
#define STORMKIT_STRINGIFY(x) STORMKIT_STRINGIFY_DETAILS(x)

#if defined(STORMKIT_COMPILER_GCC) || defined(STORMKIT_COMPILER_CLANG)
    #define STORMKIT_CURRENT_FUNCTION_OLD STORMKIT_STRINGIFY(__PRETTY_FUNCTION__)
#elif defined(STORMKIT_COMPILER_MSVC)
    #define STORMKIT_CURRENT_FUNCTION_OLD STORMKIT_STRINGIFY(__FUNCSIG__)
#else
    #define STORMKIT_CURRENT_FUNCTION_OLD STORMKIT_STRINGIFY(__func__)
#endif

#if __has_include(<source_location>)
    #include <source_location>

    #define STORMKIT_CURRENT_FILE std::source_location::current().file_name()
    #define STORMKIT_CURRENT_LINE std::source_location::current().line()
    #define STORMKIT_CURRENT_FUNCTION std::source_location::current().function_name()
#else
    #define STORMKIT_CURRENT_FILE " "
    #define STORMKIT_CURRENT_LINE 0
    #define STORMKIT_CURRENT_FUNCTION STORMKIT_CURRENT_FUNCTION_OLD
#endif
