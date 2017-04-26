#pragma once
#include <blik.hpp>

namespace BLIK
{
    //! \brief 서비스-Streaming
    class StreamingService
    {
    public:
        class Codec
        {
        public:
            struct EncodingReport
            {
                sint32 mWaitingBitmapCount; // 인코딩 대기중인 비트맵원본 수량
                sint32 mWaitingPcmCount; // 인코딩 대기중인 사운드원본 수량
                sint32 mUnsentFrameCount; // 전송 대기중인 Flv프레임 수량
                sint32 mSleepTimeMsec; // 전송원활 상황에 따른 슬립조절
                sint32 mAacEncodingTimeMsec; // Aac인코딩시간
                sint32 mFlvEncodingTimeMsec; // Flv인코딩시간
                sint32 mReportTimeMsec; // 전체작업시간
            };

        public:
            Codec();
            ~Codec();

        public:
            void Bind(const StreamingService* service);
            void Unbind(const StreamingService* service);
            void AddFrame(id_bitmap_read bitmap, uint64 timems, id_microphone microphone);
            EncodingReport* GetReportOnce() const;

        private:
            static sint32 OnCodecTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

        private:
            mutable id_tasking mTasking;
        };

    public:
        StreamingService();
        StreamingService(const StreamingService& rhs);
        ~StreamingService();
        StreamingService& operator=(const StreamingService& rhs);

    public:
        void InitForBroadcast(chars servicename, chars streamkey);
        void InitForRecord();
        sint32 GetBroadcastState(uint64& beginTimeMsec, sint64& sendedBytes) const;

    public:
        inline chars service_name() const {return mServiceName;}

    private:
        static sint32 OnBroadcastTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);
        static sint32 OnRecordTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    private:
        String mServiceName;
        mutable id_tasking mTasking;
    };
}
