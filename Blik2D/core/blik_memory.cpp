#include <blik.hpp>
#include "blik_memory.hpp"

extern "C"
{
    extern void* malloc(size_t);
    extern void free(void*);
    extern void* memset(void*, sint32, size_t);
    extern void* memcpy(void*, const void*, size_t);
    extern sint32 memcmp(const void*, const void*, size_t);
}

namespace BLIK
{
    void* Memory::Alloc(const sint32 size)
    {
        return malloc(size);
    }

    void Memory::Free(void* ptr)
    {
        free(ptr);
    }

    void Memory::Set(void* ptr, const uint08 value, const sint32 size)
    {
        memset(ptr, value, size);
    }

    void Memory::Copy(void* dst, const void* src, const sint32 size)
    {
        memcpy(dst, src, size);
    }

    void Memory::CopyVPTR(void* dst, const void* src)
    {
        switch(sizeof(void*))
        {
        case 4:
            *((uint32*) dst) = *((const uint32*) src);
            break;
        case 8:
            *((uint64*) dst) = *((const uint64*) src);
            break;
        default:
            BLIK_ASSERT("VPTR의 복사에 실패하였습니다", false);
            break;
        }
    }

    sint32 Memory::Compare(const void* dst, const void* src, const sint32 size)
    {
        return memcmp(dst, src, size);
    }
}
