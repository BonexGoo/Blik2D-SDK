#pragma once
#include <blik.hpp>

namespace BLIK
{
    //! \brief 포맷-FLV
    class Flv
    {
    public:
        static id_flash Create(sint32 width, sint32 height);
        static void Remove(id_flash flash);

    public:
        static void AddChunk(id_flash flash, uint08 type, bytes chunk, sint32 length, sint32 timestamp = 0);
        static bytes EmptyChunks(id_flash flash, sint32* length = nullptr);

    public:
        static const void* GetBE2(sint32 value);
        static const void* GetBE3(sint32 value);
        static const void* GetBE4(sint32 value);
        static const void* GetBE8_Double(double value);
    };
}
