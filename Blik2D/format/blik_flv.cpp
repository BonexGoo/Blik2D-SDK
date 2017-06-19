#include <blik.hpp>
#include "blik_flv.hpp"

class FlvPrivate
{
public:
    typedef Array<uint08, datatype_pod_canmemcpy, 256> FlvBits;

public:
    FlvPrivate() {}
    ~FlvPrivate() {}

public:
    FlvPrivate(const FlvPrivate&)
    {BLIK_ASSERT("잘못된 시나리오입니다", false);}
    FlvPrivate& operator=(const FlvPrivate&)
    {BLIK_ASSERT("잘못된 시나리오입니다", false); return *this;}

public:
    static void WriteECMA(FlvBits& dst, chars name, sint32 value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x08; // type: ecma
        Memory::Copy(dst.AtDumpingAdded(4), Flv::GetBE4(value), 4); // value
    }
    static void WriteNumber(FlvBits& dst, chars name, double value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x00; // type: number
        Memory::Copy(dst.AtDumpingAdded(8), Flv::GetBE8_Double(value), 8); // value
    }
    static void WriteBoolean(FlvBits& dst, chars name, bool value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x01; // type: boolean
        dst.AtAdding() = value; // value
    }
    static void WriteString(FlvBits& dst, chars name, chars value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x02; // type: string
        WriteScriptData(dst, value);
    }

private:
    static void WriteScriptData(FlvBits& dst, chars name)
    {
        const sint32 Length = blik_strlen(name);
        Memory::Copy(dst.AtDumpingAdded(2), Flv::GetBE2(Length), 2); // stringsize
        Memory::Copy(dst.AtDumpingAdded(Length), name, Length); // string
    }

public:
    FlvBits mBits;
};

namespace BLIK
{
    id_flash Flv::Create(sint32 width, sint32 height)
    {
        auto NewFlash = (FlvPrivate*) Buffer::Alloc<FlvPrivate>(BLIK_DBG 1);
        FlvPrivate::FlvBits& Dst = NewFlash->mBits;

        // flv header
        Memory::Copy(Dst.AtDumpingAdded(3), "FLV", 3); // Signature
        Dst.AtAdding() = 0x01; // FLV Version
        Dst.AtAdding() = 0x01 | 0x04; // Video + Audio
        Memory::Copy(Dst.AtDumpingAdded(4), GetBE4(9), 4); // DataOffset
        Memory::Copy(Dst.AtDumpingAdded(4), GetBE4(0), 4); // PreviousTagSize0

        // metaData
        FlvPrivate::FlvBits NewBits;
        NewBits.AtAdding() = 0x02; // 스트링데이터ID
        FlvPrivate::WriteECMA(NewBits, "onMetaData", 13);
        FlvPrivate::WriteNumber(NewBits, "duration", 0); // YouTube에는 없어도 되는 정보
        FlvPrivate::WriteNumber(NewBits, "width", width);
        FlvPrivate::WriteNumber(NewBits, "height", height);
        FlvPrivate::WriteNumber(NewBits, "videodatarate", 781.25);
        FlvPrivate::WriteNumber(NewBits, "framerate", 1000);
        FlvPrivate::WriteNumber(NewBits, "videocodecid", 7); // FLV_CODECID_H264: 7
        FlvPrivate::WriteNumber(NewBits, "audiodatarate", 128);
        FlvPrivate::WriteNumber(NewBits, "audiosamplerate", 44100);
        FlvPrivate::WriteNumber(NewBits, "audiosamplesize", 16);
        FlvPrivate::WriteBoolean(NewBits, "stereo", true);
        FlvPrivate::WriteNumber(NewBits, "audiocodecid", 10); // FLV_CODECID_AAC: 10
        FlvPrivate::WriteString(NewBits, "encoder", "Lavf53.24.2");
        FlvPrivate::WriteNumber(NewBits, "filesize", 0); // YouTube에는 없어도 되는 정보
        Memory::Copy(NewBits.AtDumpingAdded(3), GetBE3(9), 3); // scriptdata endcode: always 9
        AddChunk((id_flash) NewFlash, 0x12, &NewBits[0], NewBits.Count()); // script

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
        FlvPrivate::FlvBits& Dst = ((FlvPrivate*) flash)->mBits;

        Dst.AtAdding() = type; // type
        Memory::Copy(Dst.AtDumpingAdded(3), GetBE3(length), 3); // datasize
        Memory::Copy(Dst.AtDumpingAdded(3), GetBE3(timestamp & 0x00FFFFFF), 3); // timestamp
        Dst.AtAdding() = (timestamp & 0xFF000000) >> 24; // timestamp extended
        Memory::Copy(Dst.AtDumpingAdded(3), GetBE3(0), 3); // streamid
        Memory::Copy(Dst.AtDumpingAdded(length), chunk, length);
        Memory::Copy(Dst.AtDumpingAdded(4), GetBE4(length + 11), 4); // chunk size match
    }

    void Flv::Empty(id_flash flash)
    {
        if(!flash) return;
        ((FlvPrivate*) flash)->mBits.SubtractionAll();
    }

    bytes Flv::GetBits(id_flash_read flash, sint32* length)
    {
        if(!flash) return nullptr;
        FlvPrivate::FlvBits& Dst = ((FlvPrivate*) flash)->mBits;
        if(length) *length = Dst.Count();
        return (bytes) ((Share*)(id_share) Dst)->const_data();
    }

    String Flv::BuildLog(bytes bits, sint32 length)
    {
        String LogCollector;
        while(0 < length)
        {
            sint32 ChunkSize = 0;
            if(bits[0] == (uint08) 'F' && bits[1] == (uint08) 'L' && bits[2] == (uint08) 'V')
            {
                ChunkSize = 13;
                LogCollector += "[FLV]\r\n";
            }
            else
            {
                const sint32 DataSize = ((bits[1] & 0xFF) << 16) | ((bits[2] & 0xFF) << 8) | (bits[3] & 0xFF);
                ChunkSize = 15 + DataSize;
                switch(bits[0])
                {
                case 0x08: LogCollector += "[Audio: "; break;
                case 0x09: LogCollector += "[Video: "; break;
                case 0x12: LogCollector += "[MetaData: "; break;
                default: LogCollector += "[Unknown: "; break;
                }
                const sint32 TimeStamp = ((bits[7] & 0xFF) << 24) | ((bits[4] & 0xFF) << 16) | ((bits[5] & 0xFF) << 8) | (bits[6] & 0xFF);
                LogCollector += String::Format("0x%08X, %d Byte]\r\n", TimeStamp, DataSize);
            }
            bits += ChunkSize;
            length -= ChunkSize;
        }
        BLIK_ASSERT("불완전한 청크가 존재합니다", length == 0);
        return LogCollector;
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
