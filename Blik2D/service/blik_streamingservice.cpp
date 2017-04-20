#include <blik.hpp>
#include "blik_streamingservice.hpp"

#include <addon/blik_addon.hpp>
#include <format/blik_bmp.hpp>
#include <format/blik_flv.hpp>
#include <platform/blik_platform.hpp>

class CodecSender
{
public:
    CodecSender()
    {
        mFlash = nullptr;
        mFirstTimeMs = 0;
        mLastTimeMs = 0;
        mImageCodec = nullptr;
        mSoundCodec = nullptr;
    }
    ~CodecSender()
    {
        Reset();
    }

public:
    void Reset()
    {
        if(mFlash)
        {
            Flv::Remove(mFlash);
            mFlash = nullptr;
        }
        mFirstTimeMs = 0;
        mLastTimeMs = 0;
        if(mImageCodec)
        {
            AddOn::H264::Release(mImageCodec);
            mImageCodec = nullptr;
        }
        if(mSoundCodec)
        {
            AddOn::Aac::Release(mSoundCodec);
            mSoundCodec = nullptr;
        }
        while(0 < mBitmapQueue.Count())
            mBitmapQueue.Dequeue();
        while(0 < mPcmQueue.Count())
            mPcmQueue.Dequeue();
    }

public:
    id_flash mFlash;
    uint64 mFirstTimeMs;
    uint64 mLastTimeMs;
    id_h264 mImageCodec;
    id_acc mSoundCodec;
    Queue<uint08s> mBitmapQueue;
    Queue<uint08s> mPcmQueue;
    Array<id_tasking, datatype_pod_canmemcpy> mReceivers;
};

class CodecReceiver
{
public:
    CodecReceiver()
    {
        mKilled = false;
        mBeginTimeMs = 0;
    }
    virtual ~CodecReceiver()
    {
        ResetFrameQueue();
    }

protected:
    void ResetFrameQueue()
    {
        while(0 < mFrameQueue.Count())
            mFrameQueue.Dequeue();
    }

public:
    bool mKilled;
    Queue<uint08s> mFrameQueue;
    uint64 mBeginTimeMs;
};

class CodecBroadcastReceiver : public CodecReceiver
{
public:
    CodecBroadcastReceiver()
    {
        mSendedBytes = 0;
        mEpisodeNumber = 0;
    }
    ~CodecBroadcastReceiver() override
    {
        mSavedFrame.Clear();
    }

public:
    void Reset()
    {
        ResetFrameQueue();
        mSavedFrame.Clear();
    }

public:
    String mServiceName;
    String mStreamKey;
    uint08s mSavedFrame;
    sint64 mSendedBytes;
    sint32 mEpisodeNumber;
};

class CodecRecordReceiver : public CodecReceiver
{
public:
    CodecRecordReceiver()
    {
        mSavedBytes = 0;
    }
    ~CodecRecordReceiver() override
    {
    }

public:

public:
    sint64 mSavedBytes;
};

class CurlSender
{
public:
    CurlSender()
    {
        mStream = AddOn::Curl::Create();
    }
    ~CurlSender()
    {
        AddOn::Curl::Release(mStream);
    }

public:
    static size_t OnRead(void* ptr, size_t size, size_t nitems, payload data)
    {
        uint64 BegineTime = Platform::Utility::CurrentTimeMs();
        while(Platform::Utility::CurrentTimeMs() - BegineTime < 5000)
        {
            bool IsKilled = false;
            size_t CopiedSize = 0;
            BLIK_COMMON_TASK((id_common) data, common_buffer)
            if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
            {
                uint08s CurFrame = CodecR->mSavedFrame;
                CodecR->mSavedFrame.Clear();

                const size_t MaxSize = size * nitems;
                while(CopiedSize < MaxSize && !IsKilled)
                {
                    if(0 < CurFrame.Count())
                    {
                        if(CopiedSize + CurFrame.Count() <= MaxSize)
                        {
                            Memory::Copy(((uint08*) ptr) + CopiedSize, &CurFrame[0], CurFrame.Count());
                            CopiedSize += CurFrame.Count();
                            CodecR->mSendedBytes += CurFrame.Count();
                        }
                        else
                        {
                            CodecR->mSavedFrame = CurFrame;
                            CurFrame.Clear();
                            break;
                        }
                    }
                    if(0 < CodecR->mFrameQueue.Count())
                        CurFrame = CodecR->mFrameQueue.Dequeue();
                    else break;
                    IsKilled = CodecR->mKilled;
                }
            }
            if(0 < CopiedSize || IsKilled)
                return CopiedSize;
            Platform::Utility::Sleep(50, false);
        }
        return 0;
    }

public:
    id_curl mStream;
};

class VideoSaver
{
public:
    VideoSaver()
    {
        mFile = nullptr;
    }
    ~VideoSaver()
    {
        Asset::Close(mFile);
    }

public:
    void Append(CodecRecordReceiver* receiver)
    {
        if(!mFile) mFile = Asset::OpenForWrite("videoes/temp.flv", true);
        while(0 < receiver->mFrameQueue.Count())
        {
            uint08s CurFrame = receiver->mFrameQueue.Dequeue();
            if(0 < CurFrame.Count())
            {
                Asset::Write(mFile, &CurFrame[0], CurFrame.Count());
                receiver->mSavedBytes += CurFrame.Count();
            }
        }
    }

public:
    id_asset mFile;
};

namespace BLIK
{
    ////////////////////////////////////////////////////////////////////////////////
    // Codec
    ////////////////////////////////////////////////////////////////////////////////
    StreamingService::Codec::Codec()
    {
        mTasking = Tasking::Create(OnCodecTask);
        BLIK_COMMON(mTasking, common_buffer)
        {
            Buffer::Free(common_buffer);
            common_buffer = Buffer::Alloc<CodecSender>(BLIK_DBG 1);
        }
    }

    StreamingService::Codec::~Codec()
    {
        Tasking::Release(mTasking, true);
    }

    void StreamingService::Codec::Bind(const StreamingService* service)
    {
        BLIK_COMMON(mTasking, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        {
            CodecS->Reset();
            CodecS->mReceivers.AtAdding() = service->mTasking;
        }
    }

    void StreamingService::Codec::Unbind(const StreamingService* service)
    {
        BLIK_COMMON(mTasking, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        {
            for(sint32 i = 0, iend = CodecS->mReceivers.Count(); i < iend; ++i)
            {
                if(CodecS->mReceivers[i] == service->mTasking)
                {
                    for(sint32 j = i; j < iend - 1; ++j)
                        CodecS->mReceivers.At(j) = CodecS->mReceivers[j + 1];
                    CodecS->mReceivers.SubtractionOne();
                    break;
                }
            }
        }
    }

    void StreamingService::Codec::AddFrame(id_bitmap_read bitmap, uint64 timems, id_microphone microphone)
    {
        BLIK_COMMON(mTasking, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        {
            // 플래시비디오 초기화
            if(!CodecS->mFlash)
            {
                CodecS->mFlash = Flv::Create(Bmp::GetWidth(bitmap), Bmp::GetHeight(bitmap));
                CodecS->mFirstTimeMs = timems;
                CodecS->mLastTimeMs = timems;
            }
            // 이미지코덱 초기화
            if(!CodecS->mImageCodec)
                CodecS->mImageCodec = AddOn::H264::Create(Bmp::GetWidth(bitmap), Bmp::GetHeight(bitmap), true);
            // 사운드코덱 초기화
            if(!CodecS->mSoundCodec)
                CodecS->mSoundCodec = AddOn::Aac::Create(
                    Platform::Microphone::GetBitRate(microphone),
                    Platform::Microphone::GetChannel(microphone),
                    Platform::Microphone::GetSampleRate(microphone));

            // 비트맵을 현재시간과 함께 큐저장
            uint08s NewBitmap;
            const sint32 NewBitmapSize = Bmp::GetFileSizeWithoutBM(bitmap);
            Memory::Copy(NewBitmap.AtDumping(0, sizeof(uint64) + NewBitmapSize), &timems, sizeof(uint64));
            Memory::Copy(NewBitmap.AtDumping(sizeof(uint64), NewBitmapSize), bitmap, NewBitmapSize);
            CodecS->mBitmapQueue.Enqueue(NewBitmap);

            // 한 프레임동안 발생한 모든 사운드를 각 발생시간과 함께 큐저장
            while(Platform::Microphone::TryNextSound(microphone))
            {
                uint08s NewPcm;
                sint32 PcmLength = 0;
                uint64 PcmTimeMs = 0;
                bytes Pcm = Platform::Microphone::GetSoundData(microphone, &PcmLength, &PcmTimeMs);
                Memory::Copy(NewPcm.AtDumping(0, sizeof(uint64) + PcmLength), &PcmTimeMs, sizeof(uint64));
                Memory::Copy(NewPcm.AtDumping(sizeof(uint64), PcmLength), Pcm, PcmLength);
                CodecS->mPcmQueue.Enqueue(NewPcm);
            }
        }
    }

    StreamingService::Codec::EncodingReport* StreamingService::Codec::GetReportOnce() const
    {
        return (EncodingReport*) Tasking::GetAnswer(mTasking);
    }

    sint32 StreamingService::Codec::OnCodecTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        static sint32 SleepTime = 5;
        BLIK_COMMON_TASK(common, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        if(CodecS->mImageCodec && 0 < CodecS->mBitmapQueue.Count())
        {
            auto NewReport = (EncodingReport*) Buffer::Alloc<EncodingReport, datatype_pod_canmemcpy_zeroset>(BLIK_DBG 1);
            const uint64 ReportBegin = Platform::Utility::CurrentTimeMs();

            NewReport->mWaitingBitmapCount = CodecS->mBitmapQueue.Count();
            NewReport->mWaitingPcmCount = CodecS->mPcmQueue.Count();
            // 비트맵과 사운드의 보관상한선
            while(16 < CodecS->mBitmapQueue.Count())
                CodecS->mBitmapQueue.Dequeue();
            while(128 < CodecS->mPcmQueue.Count())
                CodecS->mPcmQueue.Dequeue();

            if(0 < CodecS->mReceivers.Count()) // 채널이 있다면
            {
                // 모든 채널들에 송신하지 못하고 쌓인 데이터량 수집
                sint32 SumFrameQueueCount = 0;
                for(sint32 i = 0, iend = CodecS->mReceivers.Count(); i < iend; ++i)
                {
                    BLIK_COMMON(CodecS->mReceivers[i], receiver_common_buffer)
                    if(auto CodecR = (CodecReceiver*) receiver_common_buffer)
                        SumFrameQueueCount += CodecR->mFrameQueue.Count();
                }
                NewReport->mUnsentFrameCount = SumFrameQueueCount;

                // 그에 따른 슬립조절
                if(SumFrameQueueCount <= 1) SleepTime = Math::Max(5, SleepTime - 1);
                else if(8 <= SumFrameQueueCount) SleepTime = Math::Min(SleepTime + 1, 1000);
                NewReport->mSleepTimeMs = SleepTime;

                if(SumFrameQueueCount < 10) // 채널들의 송신상태가 원활
                {
                    NewReport->mFlvEncodingTimeMs = 0;
                    NewReport->mAacEncodingTimeMs = 0;
                    uint08s LastBitmapQueue, LastPcmQueue;
                    uint64 LastBitmapTimeMs = 0, LastPcmTimeMs = 0;
                    while(true)
                    {
                        if(!LastBitmapTimeMs && 0 < CodecS->mBitmapQueue.Count())
                        {
                            LastBitmapQueue = CodecS->mBitmapQueue.Dequeue();
                            LastBitmapTimeMs = *((uint64*) &LastBitmapQueue[0]);
                        }
                        if(!LastPcmTimeMs && CodecS->mSoundCodec && 0 < CodecS->mPcmQueue.Count())
                        {
                            LastPcmQueue = CodecS->mPcmQueue.Dequeue();
                            LastPcmTimeMs = *((uint64*) &LastPcmQueue[0]);
                        }
                        // H264인코딩
                        if((LastBitmapTimeMs && !LastPcmTimeMs) || LastPcmTimeMs > LastBitmapTimeMs)
                        {
                            const uint64 FlvEncodingBegin = Platform::Utility::CurrentTimeMs();
                            const uint32* CurBitmapBits = (const uint32*) Bmp::GetBits((id_bitmap_read) &LastBitmapQueue[sizeof(uint64)]);
                            CodecS->mLastTimeMs = (CodecS->mLastTimeMs > LastBitmapTimeMs)? CodecS->mLastTimeMs : LastBitmapTimeMs;
                            AddOn::H264::EncodeTo(CodecS->mImageCodec, CurBitmapBits, CodecS->mFlash,
                                CodecS->mLastTimeMs - CodecS->mFirstTimeMs);
                            NewReport->mFlvEncodingTimeMs += (sint32) (Platform::Utility::CurrentTimeMs() - FlvEncodingBegin);
                            LastBitmapTimeMs = 0;
                        }
                        // AAC인코딩
                        else if((!LastBitmapTimeMs && LastPcmTimeMs) || LastPcmTimeMs < LastBitmapTimeMs)
                        {
                            const uint64 AacEncodingBegin = Platform::Utility::CurrentTimeMs();
                            bytes CurPcm = &LastPcmQueue[sizeof(uint64)];
                            const sint32 CurPcmLength = LastPcmQueue.Count() - sizeof(uint64);
                            CodecS->mLastTimeMs = (CodecS->mLastTimeMs > LastPcmTimeMs)? CodecS->mLastTimeMs : LastPcmTimeMs;
                            AddOn::Aac::EncodeTo(CodecS->mSoundCodec, CurPcm, CurPcmLength, CodecS->mFlash,
                                CodecS->mLastTimeMs - CodecS->mFirstTimeMs);
                            NewReport->mAacEncodingTimeMs += (sint32) (Platform::Utility::CurrentTimeMs() - AacEncodingBegin);
                            LastPcmTimeMs = 0;
                        }
                        else if(!LastBitmapTimeMs && !LastPcmTimeMs)
                            break;
                    }

                    // 각 스트림에 전달
                    sint32 VideoLength = 0;
                    bytes Video = Flv::EmptyChunks(CodecS->mFlash, &VideoLength);
                    if(0 < VideoLength)
                    {
                        uint08s ClonedFrame;
                        Memory::Copy(ClonedFrame.AtDumpingAdded(VideoLength), Video, VideoLength);
                        for(sint32 i = 0, iend = CodecS->mReceivers.Count(); i < iend; ++i)
                        {
                            BLIK_COMMON(CodecS->mReceivers[i], receiver_common_buffer)
                            if(auto CodecR = (CodecReceiver*) receiver_common_buffer)
                                CodecR->mFrameQueue.Enqueue(ClonedFrame);
                        }
                    }
                }
            }

            NewReport->mReportTimeMs = (sint32) (Platform::Utility::CurrentTimeMs() - ReportBegin);
            answer.Enqueue((buffer) NewReport);
        }
        return SleepTime;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // StreamingService
    ////////////////////////////////////////////////////////////////////////////////
    StreamingService::StreamingService()
    {
        mTasking = nullptr;
    }

    StreamingService::StreamingService(const StreamingService& rhs)
    {
        operator=(rhs);
    }

    StreamingService::~StreamingService()
    {
        if(mTasking)
        {
            BLIK_COMMON(mTasking, common_buffer)
            if(auto CodecR = (CodecReceiver*) common_buffer)
                CodecR->mKilled = true;
            Tasking::Release(mTasking, true);
        }
    }

    StreamingService& StreamingService::operator=(const StreamingService& rhs)
    {
        mServiceName = rhs.mServiceName;
        mTasking = rhs.mTasking;
        rhs.mTasking = nullptr;
        return *this;
    }

    void StreamingService::InitForBroadcast(chars servicename, chars streamkey)
    {
        BLIK_ASSERT("본 객체는 여러번 초기화될 수 없습니다", !mTasking);
        mServiceName = servicename;
        mTasking = Tasking::Create(OnBroadcastTask);
        BLIK_COMMON(mTasking, common_buffer)
        {
            common_buffer = Buffer::Alloc<CodecBroadcastReceiver>(BLIK_DBG 1);
            auto NewCodec = (CodecBroadcastReceiver*) common_buffer;
            NewCodec->mBeginTimeMs = Platform::Utility::CurrentTimeMs();
            NewCodec->mServiceName = servicename;
            NewCodec->mStreamKey = streamkey;
        }
    }

    void StreamingService::InitForRecord()
    {
        BLIK_ASSERT("본 객체는 여러번 초기화될 수 없습니다", !mTasking);
        mTasking = Tasking::Create(OnRecordTask);
        BLIK_COMMON(mTasking, common_buffer)
        {
            common_buffer = Buffer::Alloc<CodecRecordReceiver>(BLIK_DBG 1);
            auto NewCodec = (CodecRecordReceiver*) common_buffer;
            NewCodec->mBeginTimeMs = Platform::Utility::CurrentTimeMs();
        }
    }

    sint32 StreamingService::GetBroadcastState(uint64& beginTimeMs, sint64& sendedBytes) const
    {
        sint32 Result = 0;
        if(mTasking)
        BLIK_COMMON(mTasking, common_buffer)
        if(Buffer::TypeCheck<CodecBroadcastReceiver, datatype_class_nomemcpy>(common_buffer))
        if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
        {
            beginTimeMs = CodecR->mBeginTimeMs;
            sendedBytes = CodecR->mSendedBytes;
            Result = CodecR->mEpisodeNumber;
        }
        return Result;
    }

    sint32 StreamingService::OnBroadcastTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        if(!self) self = Buffer::Alloc<CurlSender>(BLIK_DBG 1);
        auto CurSender = (CurlSender*) self;

        bool ValidService = false;
        String ServiceName, StreamKey;
        BLIK_COMMON_TASK(common, common_buffer)
        if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
        {
            ValidService = (!CodecR->mKilled && 0 < CodecR->mFrameQueue.Count());
            if(ValidService)
            {
                ServiceName = CodecR->mServiceName;
                StreamKey = CodecR->mStreamKey;
                CodecR->mBeginTimeMs = Platform::Utility::CurrentTimeMs();
                CodecR->mSendedBytes = 0;
                CodecR->mEpisodeNumber++;
            }
        }

        if(ValidService)
        {
            String Url = "";
            if(!String::Compare(ServiceName, "YouTube"))
                Url = String::Format("rtmp://a.rtmp.youtube.com/live2/%s", (chars) StreamKey);
            else if(!String::Compare(ServiceName, "Facebook"))
                Url = String::Format("rtmp://rtmp-api.facebook.com:80/rtmp/%s", (chars) StreamKey);
            else BLIK_ASSERT("준비되지 않은 서비스입니다", false);

            // 여기서 무한루프
            AddOn::Curl::SendStream(CurSender->mStream, Url, CurlSender::OnRead, common);

            // SendStream루프를 빠져나왔다는 것은 접속불량 상황
            BLIK_COMMON_TASK(common, common_buffer)
            if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
                CodecR->Reset();
        }
        return 500;
    }

    sint32 StreamingService::OnRecordTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        if(!self) self = Buffer::Alloc<VideoSaver>(BLIK_DBG 1);
        auto CurSaver = (VideoSaver*) self;

        BLIK_COMMON_TASK(common, common_buffer)
        if(auto CodecR = (CodecRecordReceiver*) common_buffer)
        if(!CodecR->mKilled && 0 < CodecR->mFrameQueue.Count())
            CurSaver->Append(CodecR);
        return 500;
    }
}
