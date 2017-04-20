#include <blik.hpp>
#include "blik_flv.hpp"

class FlvPrivate
{
public:
    typedef Array<uint08, datatype_pod_canmemcpy, 256> Chunk;

public:
    FlvPrivate() {}
    ~FlvPrivate() {}

public:
    FlvPrivate(const FlvPrivate&)
    {BLIK_ASSERT("잘못된 시나리오입니다", false);}
    FlvPrivate& operator=(const FlvPrivate&)
    {BLIK_ASSERT("잘못된 시나리오입니다", false); return *this;}

public:
    static void WriteECMA(Chunk& dst, chars name, sint32 value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x08; // type: ecma
        Memory::Copy(dst.AtDumpingAdded(4), Flv::GetBE4(value), 4); // value
    }
    static void WriteNumber(Chunk& dst, chars name, double value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x00; // type: number
        Memory::Copy(dst.AtDumpingAdded(8), Flv::GetBE8_Double(value), 8); // value
    }
    static void WriteBoolean(Chunk& dst, chars name, bool value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x01; // type: boolean
        dst.AtAdding() = value; // value
    }
    static void WriteString(Chunk& dst, chars name, chars value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x02; // type: string
        WriteScriptData(dst, value);
    }

private:
    static void WriteScriptData(Chunk& dst, chars name)
    {
        const sint32 Length = blik_strlen(name);
        Memory::Copy(dst.AtDumpingAdded(2), Flv::GetBE2(Length), 2); // stringsize
        Memory::Copy(dst.AtDumpingAdded(Length), name, Length); // string
    }

public:
    Chunk mChunks;
};

namespace BLIK
{
    id_flash Flv::Create(sint32 width, sint32 height)
    {
        auto NewFlash = (FlvPrivate*) Buffer::Alloc<FlvPrivate>(BLIK_DBG 1);
        FlvPrivate::Chunk& Dst = NewFlash->mChunks;

        // flv header
        Memory::Copy(Dst.AtDumpingAdded(3), "FLV", 3); // Signature
        Dst.AtAdding() = 0x01; // FLV Version
        Dst.AtAdding() = 0x01 | 0x04; // Video + Audio
        Memory::Copy(Dst.AtDumpingAdded(4), GetBE4(9), 4); // DataOffset
        Memory::Copy(Dst.AtDumpingAdded(4), GetBE4(0), 4); // PreviousTagSize0

        // metaData
        FlvPrivate::Chunk NewChunk;
        NewChunk.AtAdding() = 0x02; // 스트링데이터ID
        FlvPrivate::WriteECMA(NewChunk, "onMetaData", 13);
        FlvPrivate::WriteNumber(NewChunk, "duration", 0); // YouTube에는 없어도 되는 정보
        FlvPrivate::WriteNumber(NewChunk, "width", width);
        FlvPrivate::WriteNumber(NewChunk, "height", height);
        FlvPrivate::WriteNumber(NewChunk, "videodatarate", 781.25);
        FlvPrivate::WriteNumber(NewChunk, "framerate", 1000);
        FlvPrivate::WriteNumber(NewChunk, "videocodecid", 7); // FLV_CODECID_H264: 7
        FlvPrivate::WriteNumber(NewChunk, "audiodatarate", 128);
        FlvPrivate::WriteNumber(NewChunk, "audiosamplerate", 44100);
        FlvPrivate::WriteNumber(NewChunk, "audiosamplesize", 16);
        FlvPrivate::WriteBoolean(NewChunk, "stereo", true);
        FlvPrivate::WriteNumber(NewChunk, "audiocodecid", 10); // FLV_CODECID_AAC: 10
        FlvPrivate::WriteString(NewChunk, "encoder", "Lavf53.24.2");
        FlvPrivate::WriteNumber(NewChunk, "filesize", 0); // YouTube에는 없어도 되는 정보
        Memory::Copy(NewChunk.AtDumpingAdded(3), GetBE3(9), 3); // scriptdata endcode: always 9
        AddChunk((id_flash) NewFlash, 0x12, &NewChunk[0], NewChunk.Count()); // script

        // 0001 0... .... .... : AAC low complexity
        // .... .010 0... .... : 44100 hz
        // .... .... .001 0... : 2 channel
        // .... .... .... .0.. : 1024 sample
        // .... .... .... ..0. : depends on core coder
        // .... .... .... ...0 : extension flag
        // 1    2    1    0
        AddChunk((id_flash) NewFlash, 0x08, (bytes) "\xaf\x00" "\x12\x10", 4); // audio
        return (id_flash) NewFlash;
    }

    void Flv::Remove(id_flash flash)
    {
        Buffer::Free((buffer) flash);
    }

    void Flv::AddChunk(id_flash flash, uint08 type, bytes chunk, sint32 length, sint32 timestamp)
    {
        if(!flash) return;
        FlvPrivate::Chunk& Dst = ((FlvPrivate*) flash)->mChunks;

        Dst.AtAdding() = type; // type
        Memory::Copy(Dst.AtDumpingAdded(3), GetBE3(length), 3); // datasize
        Memory::Copy(Dst.AtDumpingAdded(3), GetBE3(timestamp & 0x00FFFFFF), 3); // timestamp
        Dst.AtAdding() = (timestamp & 0xFF000000) >> 24; // timestamp extended
        Memory::Copy(Dst.AtDumpingAdded(3), GetBE3(0), 3); // streamid
        Memory::Copy(Dst.AtDumpingAdded(length), chunk, length);
        Memory::Copy(Dst.AtDumpingAdded(4), GetBE4(length + 11), 4); // chunk size match
    }

    bytes Flv::EmptyChunks(id_flash flash, sint32* length)
    {
        if(!flash) return nullptr;
        FlvPrivate::Chunk& Dst = ((FlvPrivate*) flash)->mChunks;
        if(length) *length = Dst.Count();
        Dst.SubtractionAll(); // datatype_pod_canmemcpy타입은 내용을 지우지 않음
        return (bytes) ((Share*)(id_share) Dst)->const_data();
    }

    static uint32 result_be2;
    const void* Flv::GetBE2(sint32 value)
    {
        result_be2  = (value >>  8) & 0x000000FF;
        result_be2 |= (value <<  8) & 0x0000FF00;
        return &result_be2;
    }

    static uint32 result_be3;
    const void* Flv::GetBE3(sint32 value)
    {
        result_be3  = (value >> 16) & 0x000000FF;
        result_be3 |= (value >>  0) & 0x0000FF00;
        result_be3 |= (value << 16) & 0x00FF0000;
        return &result_be3;
    }

    static uint32 result_be4;
    const void* Flv::GetBE4(sint32 value)
    {
        result_be4  = (value >> 24) & 0x000000FF;
        result_be4 |= (value >>  8) & 0x0000FF00;
        result_be4 |= (value <<  8) & 0x00FF0000;
        result_be4 |= (value << 24) & 0xFF000000;
        return &result_be4;
    }

    static uint64 result_be8_double;
    const void* Flv::GetBE8_Double(double value)
    {
        uint64 SrcValue = *((uint64*) &value);
        result_be8_double  = (SrcValue >> 56) & ox00000000000000FF;
        result_be8_double |= (SrcValue >> 40) & ox000000000000FF00;
        result_be8_double |= (SrcValue >> 24) & ox0000000000FF0000;
        result_be8_double |= (SrcValue >>  8) & ox00000000FF000000;
        result_be8_double |= (SrcValue <<  8) & ox000000FF00000000;
        result_be8_double |= (SrcValue << 24) & ox0000FF0000000000;
        result_be8_double |= (SrcValue << 40) & ox00FF000000000000;
        result_be8_double |= (SrcValue << 56) & oxFF00000000000000;
        return &result_be8_double;
    }
}
