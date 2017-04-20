#pragma once
#include <blik.hpp>

namespace BLIK
{
    BLIK_DECLARE_ID(id_acc);
    BLIK_DECLARE_ID(id_alpr);
    BLIK_DECLARE_ID(id_curl);
    BLIK_DECLARE_ID(id_h264);
    BLIK_DECLARE_ID(id_git);
    BLIK_DECLARE_ID(id_tesseract);
    BLIK_DECLARE_ID(id_jpg);
    enum ProgressType {PT_DownloadPack, PT_DownloadTip, PT_InflateFile};
    typedef size_t (*CurlReadCB)(void* ptr, size_t size, size_t nitems, payload data);

    //! \brief 애드온지원
    class AddOn
    {
    public:
        //! \brief AAC연동
        class Aac
        {
        public:
            static id_acc Create(sint32 bitrate, sint32 channel, sint32 samplerate);
            static void Release(id_acc acc);
            static void EncodeTo(id_acc acc, bytes pcm, sint32 length, id_flash flash, uint64 timems);
        };

        //! \brief ALPR연동
        class Alpr
        {
        public:
            static id_alpr Create(chars country = "", h_view listener = h_view::null());
            static void Release(id_alpr alpr);
            static chars Recognize(id_alpr alpr, id_bitmap_read bitmap);
            static chars Summary(chars recognizedtext);
        };

        //! \brief CURL연동
        class Curl
        {
        public:
            static id_curl Create(void);
            static id_curl Clone(id_curl curl);
            static void Release(id_curl curl);
            static chars RequestString(id_curl curl, chars url, chars postdata = nullptr, chars headerdata = nullptr);
            static bytes RequestBytes(id_curl curl, chars url, sint32* getsize, chars postdata = nullptr, chars headerdata = nullptr);
            static chars RequestRedirectUrl(id_curl curl, chars url, sint32 successcode, chars postdata = nullptr, chars headerdata = nullptr);
            static void SendStream(id_curl curl, chars url, CurlReadCB cb, payload data);
        };

        //! \brief H264연동
        class H264
        {
        public:
            static id_h264 Create(sint32 width, sint32 height, bool fastmode);
            static void Release(id_h264 h264);
            static void EncodeTo(id_h264 h264, const uint32* rgba, id_flash flash, uint64 timems);
        };

        //! \brief GIT연동
        class Git
        {
        public:
            typedef void (*ProgressCB)(payload, ProgressType, chars);

        public:
            static id_git Create(chars rootpath, chars sshname, chars id, chars password);
            static void Release(id_git git);
            static void Update(id_git git, ProgressCB cb = nullptr, payload data = nullptr);
        };

        //! \brief TESSERACT연동
        class Tesseract
        {
        public:
            static id_tesseract Create(chars tifpath, chars otherpath);
            static void Release(id_tesseract tesseract);
            static void Training(id_tesseract tesseract, chars workname);
        };

        //! \brief JPG연동
        class Jpg
        {
        public:
            static id_jpg Create(id_bitmap_read bmp, sint32 quality = 100);
            static void Release(id_jpg jpg);
            static sint32 GetLength(id_jpg jpg);
            static bytes GetBits(id_jpg jpg);
            static id_bitmap ToBmp(bytes jpg, sint32 length);
        };
    };
}
