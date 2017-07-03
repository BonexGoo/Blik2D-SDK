#pragma once
/*!
\mainpage BLIK Engine
\section information 목적
    - Qt기반 UI프레임워크
\section advanced 소개
    - core그룹: 기본유틸(공유자원/컨테이너/스트링/파서/콘텍스트/프로파일링/태스킹)
    - element그룹: 기본객체
    - format그룹: BMP, PNG, ZEN
    - manager그룹: 이미지빌더, 뷰운영
    - platform그룹: 크로스플랫폼 제공
\section developer 개발자
    - Bonex Gu
*/

// About disable warning
#pragma warning(disable:4117) // 미리 정의된 매크로를 정의 또는 정의 해제하려고 합니다
#pragma warning(disable:4244) // type1에서 type2로 변환하면서 데이터가 손실될 수 있습니다
#pragma warning(disable:4273) // dll 링크가 일치하지 않습니다
#pragma warning(disable:4800) // true 또는 false로 bool 값을 강제하고 있습니다
#pragma warning(disable:4996) // 컴파일러가 사용되지 않는 선언을 발견했습니다
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

// About literal incoding
#pragma execution_character_set("utf-8")

// About debug macro
#if !defined(BLIK_NDEBUG) || (BLIK_NDEBUG != 0 && BLIK_NDEBUG != 1)
    #error BLIK_NDEBUG macro is invalid use
#endif

// About platform macro
#if defined(_WIN32) || defined(_WIN64)
    #define BLIK_WINDOWS 1
#else
    #define BLIK_WINDOWS 0
#endif

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC && !TARGET_OS_IPHONE
        #define BLIK_MAC_OSX 1
    #else
        #define BLIK_MAC_OSX 0
    #endif
    #if TARGET_OS_MAC && TARGET_OS_IPHONE
        #define BLIK_IPHONE 1
    #else
        #define BLIK_IPHONE 0
    #endif
#else
    #define BLIK_MAC_OSX 0
    #define BLIK_IPHONE 0
#endif

#if defined(ANDROID)
    #define BLIK_ANDROID 1
#else
    #define BLIK_ANDROID 0
#endif

#if (BLIK_WINDOWS + BLIK_MAC_OSX + BLIK_IPHONE + BLIK_ANDROID != 1)
    #error Unknown platform
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_WIN64)
    #define BLIK_X64 1
#else
    #define BLIK_X64 0
#endif

// About type_t
#if BLIK_MAC_OSX || BLIK_IPHONE
    typedef unsigned long blik_size_t;
    typedef long blik_ssize_t;
    #ifndef __cplusplus
        typedef int wchar_t;
    #endif
#elif BLIK_ANDROID
    typedef unsigned int blik_size_t;
    typedef long int blik_ssize_t;
    #ifndef __cplusplus
        typedef unsigned int wchar_t;
    #endif
#elif BLIK_WINDOWS
    #if BLIK_X64
        typedef unsigned __int64 blik_size_t;
        typedef __int64 blik_ssize_t;
    #else
        typedef unsigned int blik_size_t;
        typedef int blik_ssize_t;
    #endif
    #ifndef __cplusplus
        typedef unsigned short wchar_t;
    #endif
#endif
#define _SIZE_T_DEFINED
#define _SSIZE_T_DEFINED_
#define __SSIZE_T
#define _WCHAR_T_DEFINED
#define _WCHAR_T
#define size_t blik_size_t
#define ssize_t blik_ssize_t

// About assert
#if !BLIK_NDEBUG
    #define BLIK_DBG __FILE__,__LINE__,__FUNCTION__,
    #define BLIK_DBG_PRM const char* file,const int line,const char* func,
    #define BLIK_DBG_VAL const char* File;const int Line;const char* Func;
    #define BLIK_DBG_INT File(file),Line(line),Func(func),
    #define BLIK_DBG_ARG file,line,func,
    #define BLIK_DBG_FILE file
    #define BLIK_DBG_LINE line
    #define BLIK_DBG_FUNC func
    #define BLIK_ASSERT(TEXT, QUERY) do{ \
        static int IsIgnore = 0; \
        if(IsIgnore || (QUERY) != 0) break; \
        switch(blik_platform_assert(BLIK_DBG TEXT, #QUERY)) \
        { \
        case 0: BLIK_DBG_BREAK; break; \
        case 1: IsIgnore = 1; break; \
        }} while(0)
    #define BLIK_ASSERT_PRM(TEXT, QUERY) do{ \
        static int IsIgnore = 0; \
        if(IsIgnore || (QUERY) != 0) break; \
        switch(blik_platform_assert(BLIK_DBG_ARG TEXT, #QUERY)) \
        { \
        case 0: BLIK_DBG_BREAK; break; \
        case 1: IsIgnore = 1; break; \
        }} while(0)
    #define BLIK_ASSERT_ARG(TEXT, QUERY, ...) do{ \
        static int IsIgnore = 0; \
        if(IsIgnore || (QUERY) != 0) break; \
        switch(blik_platform_assert(__VA_ARGS__, TEXT, #QUERY)) \
        { \
        case 0: BLIK_DBG_BREAK; break; \
        case 1: IsIgnore = 1; break; \
        }} while(0)
    #if BLIK_WINDOWS
        #define BLIK_DBG_BREAK do{__debugbreak();} while(0)
    #elif BLIK_MAC_OSX || BLIK_IPHONE
        #define BLIK_DBG_BREAK do{__builtin_trap();} while(0)
    #else
        #define BLIK_DBG_BREAK do{} while(0)
    #endif
#else
    #define BLIK_DBG
    #define BLIK_DBG_PRM
    #define BLIK_DBG_VAL
    #define BLIK_DBG_INT
    #define BLIK_DBG_ARG
    #define BLIK_DBG_FILE ""
    #define BLIK_DBG_LINE -1
    #define BLIK_DBG_FUNC ""
    #define BLIK_ASSERT(TEXT, QUERY)
    #define BLIK_ASSERT_PRM(TEXT, QUERY)
    #define BLIK_ASSERT_ARG(TEXT, QUERY, ...)
#endif

// About user config
#include <blik_config.h>

// About trace
#if !BLIK_NDEBUG | BLIK_NEED_RELEASE_TRACE
    #define BLIK_TRACE(...) blik_platform_trace(__VA_ARGS__)
#else
    #define BLIK_TRACE(...) do{} while(0)
#endif

// About global function
#ifdef __cplusplus
    extern "C" {
#endif
    #if defined(__GNUC__) && __GNUC__ >= 3
        typedef __builtin_va_list blik_va_list;
    #else
        typedef char* blik_va_list;
    #endif

    int blik_platform_assert(BLIK_DBG_PRM const char* name, const char* query);
    void blik_platform_trace(const char* text, ...);

    ////////////////////////////////////////////////////////////////////////////////
    // String
    ////////////////////////////////////////////////////////////////////////////////
    double blik_atof(const char* str);
    size_t blik_strlen(const char* str);
    size_t blik_wcslen(const wchar_t* str);
    int blik_snprintf(char* str, size_t n, const char* format, ...);
    int blik_vsnprintf(char* str, size_t n, const char* format, blik_va_list args);
    int blik_snwprintf(wchar_t* str, size_t n, const wchar_t* format, ...);
    int blik_vsnwprintf(wchar_t* str, size_t n, const wchar_t* format, blik_va_list args);
    int blik_strcmp(const char* str1, const char* str2);
    int blik_strncmp(const char* str1, const char* str2, size_t maxcount);
    int blik_stricmp(const char* str1, const char* str2);
    int blik_strnicmp(const char* str1, const char* str2, size_t maxcount);
    int blik_wcscmp(const wchar_t* str1, const wchar_t* str2);
    int blik_wcsncmp(const wchar_t* str1, const wchar_t* str2, size_t maxcount);
    int blik_wcsicmp(const wchar_t* str1, const wchar_t* str2);
    int blik_wcsnicmp(const wchar_t* str1, const wchar_t* str2, size_t maxcount);

    ////////////////////////////////////////////////////////////////////////////////
    // File
    ////////////////////////////////////////////////////////////////////////////////
    void* blik_fopen(char const* filename, char const* mode);
    int blik_fclose(void* stream);
    int blik_fseek(void* stream, long int offset, int origin);
    long int blik_ftell(void* stream);
    size_t blik_fread(void* ptr, size_t size, size_t count, void* stream);
    size_t blik_fwrite(const void* ptr, size_t size, size_t count, void* stream);
    int blik_fgetc(void* stream);
    int blik_ungetc(int character, void* stream);
    void blik_rewind(void* stream);
    char* blik_fgets(char* str, int num, void* stream);
    void* blik_opendir(const char* dirname);
    void* blik_readdir(void* dir);
    const char* blik_getdirname(void* dirent);
    int blik_closedir(void* dir);
#ifdef __cplusplus
    }
#endif
