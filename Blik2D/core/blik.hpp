#pragma once
#include <blik.h>

namespace BLIK {}
using namespace BLIK;

namespace BLIK
{
    // About base types
    typedef signed char                       sint08; // "signed char" for __GNUC__
    typedef unsigned char                     uint08;
    typedef short                             sint16;
    typedef unsigned short                    uint16;
    typedef int                               sint32;
    typedef unsigned int                      uint32;
    typedef long long                         sint64;
    typedef unsigned long long                uint64;
    typedef const unsigned char*              bytes;
    typedef const char*                       chars;
    typedef const char*                       chars_endless;
    typedef const wchar_t*                    wchars;
    typedef const wchar_t*                    wchars_endless;
    typedef unsigned int                      argb32;
    typedef struct {sint32 x, y;}             point64;
    typedef struct {sint32 w, h;}             size64;
    typedef struct {sint32 l, t, r, b;}       rect128;
    typedef struct {sint32 i, iend;}          zone64;
    typedef struct {sint32 x, xend;}          xzone64;
    typedef struct {sint32 y, yend;}          yzone64;
    typedef struct {sint16 i, iend, x, xend;} ixzone64;
    typedef struct {sint16 i, iend, y, yend;} iyzone64;

    // About extended types
    typedef void*                             payload;
    typedef long long                         unknown;
    typedef bool                              autorun;
    typedef long long                         dependency;
    typedef const long long                   linkstep1;
    typedef const long long                   linkstep2;
    typedef const long long                   linkstep3;
    typedef const long long                   linkstep4;
    typedef const long long                   linkstep5;
    typedef const long long                   linkstep6;
    typedef const long long                   linkstep7;
    typedef const long long                   linkstep8;
    typedef const long long                   linkstep9;

    // About buffer
    #if BLIK_X64
        typedef sint64                        sblock;
        typedef uint64                        ublock;
    #else
        typedef sint32                        sblock;
        typedef uint32                        ublock;
    #endif
    typedef sblock*                           nakedbuffer;
    typedef ublock*                           buffer;
    typedef buffer                            nullbuffer;

    // About pointer checker
    template<typename TYPE>
    inline bool _IsTypePointer(TYPE&) {return false;}
    template<typename TYPE>
    inline bool _IsTypePointer(TYPE*&) {return true;}
    template<typename TYPE>
    inline bool IsTypePointer()
    {
        TYPE* Pointer = nullptr;
        TYPE& Variable = *Pointer;
        return _IsTypePointer(Variable);
    }

    // About enum
    enum datatype:uint08 {
        datatype_class_nomemcpy,
        datatype_class_canmemcpy,
        datatype_pod_canmemcpy,
        datatype_pod_canmemcpy_zeroset};
    enum didstack:bool   {didstack_null = false, didstack_ok = true};
    enum endstack:bool   {endstack_null = false};
    enum haschild:bool   {haschild_null = false, haschild_ok = true};
    enum roottype:uint08 {roottype_null = 0,
        roottype_assets, roottype_assetsrem,
        roottype_assets_syspath0, roottype_assetsrem_syspath0,
        roottype_assets_syspath1, roottype_assetsrem_syspath1,
        roottype_assets_syspath2, roottype_assetsrem_syspath2};
    enum packettype:uint08 {packettype_null = 0,
        packettype_entrance, packettype_message, packettype_leaved, packettype_kicked};
    enum jumper:sint32   {jumper_null = 0};
    enum jumper16:sint16 {jumper16_null = 0};
    template<sint32 ENUM> struct EnumToType {};
}

// About allocator
inline void* operator new(size_t, sblock ptr) {return (void*) ptr;}
inline void* operator new[](size_t, sblock ptr) {return (void*) ptr;}
inline void operator delete(void*, sblock) {}
inline void operator delete[](void*, sblock) {}

// About constant
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
    #define ox100000000 0x100000000LL
    #define ox00000000000000FF 0x00000000000000FFULL
    #define ox000000000000FF00 0x000000000000FF00ULL
    #define ox0000000000FF0000 0x0000000000FF0000ULL
    #define ox00000000FF000000 0x00000000FF000000ULL
    #define ox000000FF00000000 0x000000FF00000000ULL
    #define ox0000FF0000000000 0x0000FF0000000000ULL
    #define ox00FF000000000000 0x00FF000000000000ULL
    #define oxFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFFULL
    #define oxFFFFFFFF00000000 0xFFFFFFFF00000000ULL
    #define oxFFFF000000000000 0xFFFF000000000000ULL
    #define oxFF00000000000000 0xFF00000000000000ULL
    #define oxF000000000000000 0xF000000000000000ULL
    #define ox0000000000000000 0x0000000000000000ULL
    #define EpochToWindow(UTC) (((uint64) UTC) +  11644473600000ULL)
    #define WindowToEpoch(MS)  (((uint64) MS)  -  11644473600000ULL)
    #define EpochToJulian(UTC) (((uint64) UTC) + 210866803200000ULL)
    #define JulianToEpoch(JUL) (((uint64) JUL) - 210866803200000ULL)
#else
    #define ox100000000 0x100000000i64
    #define ox00000000000000FF 0x00000000000000FFui64
    #define ox000000000000FF00 0x000000000000FF00ui64
    #define ox0000000000FF0000 0x0000000000FF0000ui64
    #define ox00000000FF000000 0x00000000FF000000ui64
    #define ox000000FF00000000 0x000000FF00000000ui64
    #define ox0000FF0000000000 0x0000FF0000000000ui64
    #define ox00FF000000000000 0x00FF000000000000ui64
    #define oxFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFFui64
    #define oxFFFFFFFF00000000 0xFFFFFFFF00000000ui64
    #define oxFFFF000000000000 0xFFFF000000000000ui64
    #define oxFF00000000000000 0xFF00000000000000ui64
    #define oxF000000000000000 0xF000000000000000ui64
    #define ox0000000000000000 0x0000000000000000ui64
    #define EpochToWindow(UTC) (((uint64) UTC) +  11644473600000ui64)
    #define WindowToEpoch(MS)  (((uint64) MS)  -  11644473600000ui64)
    #define EpochToJulian(UTC) (((uint64) UTC) + 210866803200000ui64)
    #define JulianToEpoch(JUL) (((uint64) JUL) - 210866803200000ui64)
#endif
#if BLIK_X64
    #define PtrToUint64(PTR) (*((const uint64*) &(PTR)))
    #define AnyTypeToPtr(ANY) ((void*) (ANY))
#else
    #define PtrToUint64(PTR) ((const uint64) *((const uint32*) &(PTR)))
    #define AnyTypeToPtr(ANY) ((void*) (ANY))
#endif
#define JumperPos(JMP) ((const sint32*) &(JMP))
#define JumperToPtr(JMP) (((sint32*) &(JMP)) + (JMP))
#define Jumper16Pos(JMP) ((const sint16*) &(JMP))
#define Jumper16ToPtr(JMP) (((sint16*) &(JMP)) + (JMP))

// About declare
#define BLIK_DECLARE_ID(NAME) \
    struct struct_##NAME {const buffer unused;}; \
    typedef struct struct_##NAME* NAME; \
    typedef const struct struct_##NAME* NAME##_read
#define BLIK_DECLARE_HANDLE(NAME) \
    class NAME \
    { \
    private: \
        struct data \
        { \
            sint32 mRef; \
            buffer mBuf; \
            void* mPtr; \
        }; \
        data* mData; \
    private: \
        NAME(data* gift) {mData = gift; mData->mRef++;} \
    public: \
        NAME() {mData = (data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BLIK_DBG 1); mData->mRef = 1;} \
        NAME(const NAME& rhs) {mData = rhs.mData; mData->mRef++;} \
        NAME(id_share rhs) {mData = (data*) rhs; mData->mRef++;} \
        NAME(id_cloned_share rhs) {mData = (data*) rhs;} \
        ~NAME() {release();} \
        NAME& operator=(const NAME& rhs) \
        { \
            release(); \
            mData = rhs.mData; \
            mData->mRef++; \
            return *this; \
        } \
    public: \
        static inline NAME null() \
        { \
            return NAME((data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BLIK_DBG 1)); \
        } \
        static inline NAME create_by_buf(BLIK_DBG_PRM buffer buf) \
        { \
            data* newdata = (data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BLIK_DBG_ARG 1); \
            newdata->mBuf = buf; \
            return NAME(newdata); \
        } \
        static inline NAME create_by_ptr(BLIK_DBG_PRM void* ptr) \
        { \
            data* newdata = (data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BLIK_DBG_ARG 1); \
            newdata->mPtr = ptr; \
            return NAME(newdata); \
        } \
    public: \
        inline void clear_buf() {mData->mBuf = nullptr;} \
        inline void set_buf(buffer buf) {buffer oldbuf = mData->mBuf; mData->mBuf = buf; Buffer::Free(oldbuf);} \
        inline void* set_ptr(void* ptr) {void* oldptr = mData->mPtr; mData->mPtr = ptr; return oldptr;} \
        inline void* get() const {return (void*) (((ublock) mData->mBuf) | ((ublock) mData->mPtr));} \
        inline id_share share() {return (id_share) mData;} \
        inline id_cloned_share cloned_share() {mData->mRef++; return (id_cloned_share) mData;} \
    private: \
        inline void release() \
        { \
            if(mData && --mData->mRef == 0) \
            { \
                Buffer::Free(mData->mBuf); \
                Buffer::Free((buffer) mData); \
            } \
        } \
    }; \
    typedef const NAME NAME##_read
#define BLIK_DECLARE_ADDON_FUNCTION(GROUP, NAME, RET_TYPE, ...) \
    typedef RET_TYPE (*Type_AddOn_##GROUP##_##NAME)(__VA_ARGS__); \
    Type_AddOn_##GROUP##_##NAME& Core_AddOn_##GROUP##_##NAME(); \
    static RET_TYPE Customized_AddOn_##GROUP##_##NAME(__VA_ARGS__);
#define BLIK_DEFINE_ADDON_FUNCTION(GROUP, NAME, RET_TYPE, RET_CODE, ...) \
    static RET_TYPE Default_AddOn_##GROUP##_##NAME(__VA_ARGS__) {GROUP##_Error(); RET_CODE;} \
    typedef RET_TYPE (*Type_AddOn_##GROUP##_##NAME)(__VA_ARGS__); \
    Type_AddOn_##GROUP##_##NAME& Core_AddOn_##GROUP##_##NAME() \
    {static Type_AddOn_##GROUP##_##NAME _ = Default_AddOn_##GROUP##_##NAME; return _;}

// About opengl
#if BLIK_ANDROID
    #define GL_RGBA8 0x8058
    #define GL_RGB16F 0x881B
    #define GL_HALF_FLOAT 0x140B
    #define GL_RED 0x1903
    #define GL_RG 0x8227
#endif

// About based declare id
namespace BLIK
{
    BLIK_DECLARE_ID(id_file);
    BLIK_DECLARE_ID(id_socket);
    BLIK_DECLARE_ID(id_server);
    BLIK_DECLARE_ID(id_serial);
    BLIK_DECLARE_ID(id_bitmap);
    BLIK_DECLARE_ID(id_image);
    BLIK_DECLARE_ID(id_clock);
    BLIK_DECLARE_ID(id_camera);
    BLIK_DECLARE_ID(id_microphone);
}

// About based include and declare handle
#ifndef BLIK_NOT_INCLUDE_FOR_INTELLIGENCE
    #include <blik_array.hpp>
    #include <blik_asset.hpp>
    #include <blik_buffer.hpp>
    #include <blik_counter.hpp>
    #include <blik_library.hpp>
    #include <blik_map.hpp>
    #include <blik_math.hpp>
    #include <blik_memory.hpp>
    #include <blik_mutex.hpp>
    #include <blik_object.hpp>
    #include <blik_parser.hpp>
    #include <blik_profile.hpp>
    #include <blik_property.hpp>
    #include <blik_queue.hpp>
    #include <blik_remote.hpp>
    #include <blik_share.hpp>
    #include <blik_storage.hpp>
    #include <blik_string.hpp>
    #include <blik_tasking.hpp>
    #include <blik_updater.hpp>
    #include <blik_wstring.hpp>

    namespace BLIK
    {
        BLIK_DECLARE_HANDLE(h_icon);
        BLIK_DECLARE_HANDLE(h_action);
        BLIK_DECLARE_HANDLE(h_policy);
        BLIK_DECLARE_HANDLE(h_view);
        BLIK_DECLARE_HANDLE(h_dock);
        BLIK_DECLARE_HANDLE(h_window);
    }
#endif

// About user-config macro exist check
#if !defined(BLIK_NEED_FORCED_FAKEWIN) || (BLIK_NEED_FORCED_FAKEWIN != 0 && BLIK_NEED_FORCED_FAKEWIN != 1)
    #error BLIK_NEED_FORCED_FAKEWIN macro is invalid use
#endif
#if !defined(BLIK_NEED_FULLSCREEN) || (BLIK_NEED_FULLSCREEN != 0 && BLIK_NEED_FULLSCREEN != 1)
    #error BLIK_NEED_FULLSCREEN macro is invalid use
#endif
#if !defined(BLIK_NEED_LOW_DEBUGING) || (BLIK_NEED_LOW_DEBUGING != 0 && BLIK_NEED_LOW_DEBUGING != 1)
    #error BLIK_NEED_LOW_DEBUGING macro is invalid use
#endif
#if !defined(BLIK_NEED_MAIN) || (BLIK_NEED_MAIN != 0 && BLIK_NEED_MAIN != 1)
    #error BLIK_NEED_MAIN macro is invalid use
#endif
#if !defined(BLIK_NEED_PLATFORM_FILE) || (BLIK_NEED_PLATFORM_FILE != 0 && BLIK_NEED_PLATFORM_FILE != 1)
    #error BLIK_NEED_PLATFORM_FILE macro is invalid use
#endif
#if !defined(BLIK_NEED_PROFILE) || (BLIK_NEED_PROFILE != 0 && BLIK_NEED_PROFILE != 1)
    #error BLIK_NEED_PROFILE macro is invalid use
#endif
#if !defined(BLIK_NEED_RELEASE_TRACE) || (BLIK_NEED_RELEASE_TRACE != 0 && BLIK_NEED_RELEASE_TRACE != 1)
    #error BLIK_NEED_RELEASE_TRACE macro is invalid use
#endif
