#include <blik.hpp>
#include "blik_platform_impl.hpp"

#if BLIK_WINDOWS
    #include <shlobj.h>
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "comdlg32.lib")
    #pragma comment(lib, "ole32.lib")
    #pragma comment(lib, "shell32.lib")
#elif BLIK_ANDROID
    #include <sys/sysinfo.h>
#endif

class FileMMIOBlock;
class FileMMIOView;

class FileElement
{
    friend class FileMMIOBlock;

public:
    FileElement();
    ~FileElement();

public:
    uint08* GetMap(sint32 fd, sint64 offset, sint64 size, bool readonly);
    static FileMMIOView* FindMap(uint08* map);

private:
    void SeekerCleanup(sint32 offset, sint32 size, FileMMIOBlock* block);
    void SeekerExtension(sint32 size);

public:
    buffer manager;
    FileMMIOBlock* mmio_head;
    Array<FileMMIOBlock*, datatype_pod_canmemcpy, 0> mmio_seeker;
};
Map<FileElement> FileElementMap;

class FileMMIOBlock
{
public:
    enum {UnitSize = 4096};
    typedef struct {uint08 _[UnitSize];} BlockUnit;

public:
    FileMMIOBlock(FileElement* parent, sint32 seekid) : m_parent(parent), m_seekid(seekid)
    {
        m_share = 0;
        m_usezone.i = 0;
        m_usezone.iend = 0;
        m_prev = this;
        m_next = this;
    }

    FileMMIOBlock(FileElement* parent, sint32 seekid, FileMMIOBlock* head) : m_parent(parent), m_seekid(seekid)
    {
        m_share = 0;
        m_usezone.i = 0;
        m_usezone.iend = 0;
        m_prev = head;
        m_next = m_prev->m_next;
        m_prev->m_next = this;
        m_next->m_prev = this;
    }

    ~FileMMIOBlock()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_share == 0);
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }

public:
    void CreateShare(sint32 offset, sint32 size)
    {
        m_share++;
        if(m_usezone.i == m_usezone.iend)
        {
            m_usezone.i = offset;
            m_usezone.iend = offset + size;
        }
        else
        {
            m_usezone.i = Math::Min(m_usezone.i, offset);
            m_usezone.iend = Math::Max(m_usezone.iend, offset + size);
        }
    }

    void ReleaseShare(sint32 fd)
    {
        if(--m_share == 0)
        {
            // 파일에 데이터쓰기
            Platform::File::FDSeek(fd, sizeof(BlockUnit) * m_seekid + m_usezone.i, 0);
            Platform::File::FDWrite(fd, &m_blocks[m_usezone.i], m_usezone.iend - m_usezone.i);
            // Seek청소 및 자체소멸
            m_parent->SeekerCleanup(m_seekid, m_blocks.Count(), this);
            delete this;
        }
    }

private:
    typedef Array<BlockUnit, datatype_pod_canmemcpy, 0> BlockUnits;

public:
    FileElement* const m_parent;
    const sint32 m_seekid;
    sint32 m_share;
    BlockUnits m_blocks;
    zone64 m_usezone;
    FileMMIOBlock* m_prev;
    FileMMIOBlock* m_next;
};

class FileMMIOView
{
public:
    FileMMIOView()
    {
        m_share = 0;
        m_fd = -1;
        m_block = nullptr;
    }

    ~FileMMIOView()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_share == 0);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_block);
    }

public:
    void CreateShare(sint32 fd, FileMMIOBlock* block)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_fd == -1 || m_fd == fd);
        BLIK_ASSERT("잘못된 시나리오입니다", m_block == nullptr || m_block == block);
        m_share++;
        m_fd = fd;
        m_block = block;
    }

    void ReleaseShare(const void* map)
    {
        if(--m_share == 0)
        {
            const sint32 fd = m_fd;
            m_block->ReleaseShare(fd);
            m_block = nullptr;
            Pool().Remove((ublock) map);
            Platform::File::FDClose(fd);
        }
    }

public:
    typedef Map<FileMMIOView> PoolType;

public:
    static PoolType& Pool()
    {return *BLIK_STORAGE_SYS(PoolType);}

public:
    sint32 m_share;
    sint32 m_fd;
    FileMMIOBlock* m_block;
};

FileElement::FileElement()
{
    manager = nullptr;
    mmio_head = new FileMMIOBlock(nullptr, -1);
}

FileElement::~FileElement()
{
    Buffer::Free((buffer) manager);
    BLIK_ASSERT("해제하지 못한 MMIO블럭이 존재합니다", mmio_head->m_prev == mmio_head);
    delete mmio_head;
}

uint08* FileElement::GetMap(sint32 fd, sint64 offset, sint64 size, bool readonly)
{
    // Seek테이블확보
    const sint32 SeekBegin = offset / FileMMIOBlock::UnitSize;
    const sint32 SeekEnd = (offset + size - 1) / FileMMIOBlock::UnitSize + 1;
    SeekerExtension(SeekEnd);

    // 블럭접근 및 사용정보기록
    FileMMIOBlock* CurSeek = mmio_seeker.At(SeekBegin);
    if(!CurSeek) CurSeek = new FileMMIOBlock(this, SeekBegin, mmio_head);
    const sint32 OffsetAtBlock = offset - CurSeek->m_seekid * FileMMIOBlock::UnitSize;
    CurSeek->CreateShare(OffsetAtBlock, (sint32) size);

    // 블럭의 범위수정 및 메모리확보
    const sint32 NewBlockCount = (OffsetAtBlock + size - 1) / FileMMIOBlock::UnitSize + 1;
    const sint32 OldBlockCount = CurSeek->m_blocks.Count();
    FileMMIOBlock::BlockUnit* CurBlocks = CurSeek->m_blocks.AtDumping(0, NewBlockCount);

    ///////////// 사용처가 있다면.. 블럭이 연장되는 것은.. 불가..
    //////////BLIK_ASSERT("--확인해야 함--", OldBlockCount == 0);

    // 연장된 블럭범위에 해당하는 데이터확보
    for(sint32 i = OldBlockCount, iCopyCount = 0; i < NewBlockCount; i += iCopyCount)
    {
        // Seek업데이트 및 데이터구간계산
        FileMMIOBlock** NextSeek = &mmio_seeker.At(CurSeek->m_seekid + i);
        if(*NextSeek)
        {
            BLIK_ASSERT("현재 확보범위가 기존 확보된 블럭을 침해합니다", i == NewBlockCount - 1);
            BLIK_ASSERT("잘못된 시나리오입니다", *NextSeek != CurSeek);
            iCopyCount = 1;
            *NextSeek = CurSeek;
        }
        else
        {
            iCopyCount = 0;
            do {*(NextSeek++) = CurSeek; iCopyCount++;}
            while(i + iCopyCount < NewBlockCount && !(*NextSeek));
        }
        // 파일에서 데이터확보
        Platform::File::FDSeek(fd, offset + sizeof(FileMMIOBlock::BlockUnit) * i, 0);
        Platform::File::FDRead(fd, &CurBlocks[i], sizeof(FileMMIOBlock::BlockUnit) * iCopyCount);
    }

    // 싱글턴맵에 등록
    uint08* Result = &((uint08*) CurBlocks)[OffsetAtBlock];
    FileMMIOView::Pool()[(ublock) Result].CreateShare(fd, CurSeek);
    return Result;
}

FileMMIOView* FileElement::FindMap(uint08* map)
{
    auto& CurPool = FileMMIOView::Pool();
    return (FileMMIOView*) CurPool.Access(PtrToUint64(map));
}

void FileElement::SeekerCleanup(sint32 offset, sint32 size, FileMMIOBlock* block)
{
    BLIK_ASSERT("잘못된 시나리오입니다", size < 2 || mmio_seeker[offset + 1] == block);
    if(mmio_seeker[offset] != block) {offset++; size--;}
    Memory::Set(mmio_seeker.AtDumping(offset, size), 0x00, sizeof(FileMMIOBlock*) * size);
}

void FileElement::SeekerExtension(sint32 size)
{
    const sint32 OldSize = mmio_seeker.Count();
    if(OldSize < size)
        Memory::Set(mmio_seeker.AtDumping(OldSize, size - OldSize),
            0x00, sizeof(FileMMIOBlock*) * (size - OldSize));
}

namespace BLIK
{
    namespace PlatformImpl
    {
        ////////////////////////////////////////////////////////////////////////////////
        // 일반함수와 전역인스턴스
        ////////////////////////////////////////////////////////////////////////////////
        namespace Core
        {
            chars NormalPath(chars itemname, bool QCodeTest)
            {
                itemname = (itemname[0] == '\\' &&
                    itemname[1] == '\\' &&
                    itemname[2] == '?' &&
                    itemname[3] == '\\')? itemname + 4 : itemname;
                if(QCodeTest)
                    itemname = (itemname[0] == 'Q' &&
                        itemname[1] == ':')? itemname + 2 : itemname;
                return itemname;
            }

            wchars NormalPathW(wchars itemname, bool QCodeTest)
            {
                itemname = (itemname[0] == L'\\' &&
                    itemname[1] == L'\\' &&
                    itemname[2] == L'?' &&
                    itemname[3] == L'\\')? itemname + 4 : itemname;
                if(QCodeTest)
                    itemname = (itemname[0] == L'Q' &&
                        itemname[1] == L':')? itemname + 2 : itemname;
                return itemname;
            }

            static sint32 LastFD = -1;
            sint32 CreateManager(buffer manager)
            {
                auto& NewFileElement = FileElementMap[++LastFD];
                NewFileElement.manager = manager;
                return LastFD;
            }

            void RemoveManager(sint32 id)
            {
                FileElementMap.Remove(id);
            }

            buffer GetManager(sint32 id)
            {
                if(auto CurFileElement = FileElementMap.Access(id))
                    return CurFileElement->manager;
                return nullptr;
            }

            View::CreatorCB g_Creator = View::Creator;
            String g_AssetsRoot;
            String g_AssetsRemRoot;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // 래핑함수
        ////////////////////////////////////////////////////////////////////////////////
        namespace Wrap
        {
            sint64 Utility_CurrentAvailableMemory(sint64* totalbytes)
            {
                #if BLIK_WINDOWS
                    MEMORYSTATUSEX MemoryStatus;
                    Memory::Set(&MemoryStatus, 0, sizeof(MemoryStatus));
                    MemoryStatus.dwLength = sizeof(MemoryStatus);
                    if(GlobalMemoryStatusEx(&MemoryStatus))
                    {
                        if(totalbytes) *totalbytes = (sint64) MemoryStatus.ullTotalPhys;
                        return (sint64) MemoryStatus.ullAvailPhys;
                    }
                #elif BLIK_ANDROID
                    struct sysinfo MemoryStatus;
                    if(!sysinfo(&MemoryStatus))
                    {
                        if(totalbytes) *totalbytes = (sint64) MemoryStatus.totalram;
                        return (sint64) MemoryStatus.freeram;
                    }
                #endif
                return -1;
            }

            class StaticalMutexClass
            {
            public:
                StaticalMutexClass()
                {
                    // static 인스턴스끼리의 호출순서로 생성자보다 Lock()이 먼저 호출될 수 있기에
                    // 본 생성자에선 mMutex를 초기화하지 않는다. (static 인스턴스는 자동으로 nullptr초기화됨)
                }
                ~StaticalMutexClass()
                {
                    if(mMutex)
                    {
                        Mutex::Close(mMutex);
                        mMutex = nullptr;
                    }
                }
            public:
                inline void Lock()
                {
                    if(!mMutex) // static 인스턴스끼리의 호출로 Lock()이 생성자보다 먼저 호출될 수 있음.
                        mMutex = Mutex::Open();
                    Mutex::Lock(mMutex);
                }
                inline void Unlock()
                {
                    if(mMutex)
                        Mutex::Unlock(mMutex);
                }
            private:
                id_mutex mMutex;
            };

            static Map<bool> g_OptionFlagMap;
            static StaticalMutexClass g_OptionFlagMutex;
            void Utility_SetOptionFlag(chars name, bool flag)
            {
                g_OptionFlagMutex.Lock();
                g_OptionFlagMap(name) = flag;
                g_OptionFlagMutex.Unlock();
            }

            bool Utility_GetOptionFlag(chars name)
            {
                g_OptionFlagMutex.Lock();
                bool* Result = g_OptionFlagMap.Access(name);
                g_OptionFlagMutex.Unlock();
                return (Result)? *Result : false;
            }

            Strings Utility_GetOptionFlagNames()
            {
                Strings Result;
                payload Param = (payload) &Result;
                g_OptionFlagMutex.Lock();
                g_OptionFlagMap.AccessByCallback(
                    [](const MapPath* path, const bool* data, payload param)->void
                    {
                        Strings& Result = *((Strings*) param);
                        Result.AtAdding() = &path->GetPath()[0];
                    }, Param);
                g_OptionFlagMutex.Unlock();
                return Result;
            }

            static Map<payload> g_OptionPayloadMap;
            static StaticalMutexClass g_OptionPayloadMutex;
            void Utility_SetOptionPayload(chars name, payload data)
            {
                g_OptionPayloadMutex.Lock();
                g_OptionPayloadMap(name) = data;
                g_OptionPayloadMutex.Unlock();
            }

            payload Utility_GetOptionPayload(chars name)
            {
                g_OptionPayloadMutex.Lock();
                payload* Result = g_OptionPayloadMap.Access(name);
                g_OptionPayloadMutex.Unlock();
                return (Result)? *Result : nullptr;
            }

            Strings Utility_GetOptionPayloadNames()
            {
                Strings Result;
                payload Param = (payload) &Result;
                g_OptionPayloadMutex.Lock();
                g_OptionPayloadMap.AccessByCallback(
                    [](const MapPath* path, const payload* data, payload param)->void
                    {
                        Strings& Result = *((Strings*) param);
                        Result.AtAdding() = &path->GetPath()[0];
                    }, Param);
                g_OptionPayloadMutex.Unlock();
                return Result;
            }

            bool Popup_FileDialog(String& path, String* shortpath, chars title, bool isdir)
            {
                bool Result = false;
                #if BLIK_WINDOWS
                    CoInitialize(NULL);
                    wchar_t ResultPath[_MAX_PATH] = {L'\0'};
                    WString TitleName = WString::FromChars(title);
                    WString InitDir = Platform::File::GetDirName(WString::FromChars(path), L'/', L'\\');

                    if(isdir)
                    {
                        BROWSEINFOW bi = {0};
                        bi.lpszTitle = (wchars) TitleName;
                        bi.lParam = (LPARAM)(wchars) InitDir;
                        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                        bi.lpfn = [](HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)->int
                        {
                            switch(uMsg)
                            {
                            case BFFM_INITIALIZED:
                                if(lpData) SendMessageW(hwnd, BFFM_SETSELECTION, TRUE, lpData);
                                break;
                            }
                            return 0;
                        };

                        LPITEMIDLIST pidlFolder = SHBrowseForFolderW(&bi);
                        if(pidlFolder && SHGetPathFromIDListW(pidlFolder, ResultPath))
                        {
                            CoTaskMemFree(pidlFolder);
                            Result = true;
                        }
                    }
                    else
                    {
                        OPENFILENAMEW ofn = {0};
                        ofn.lStructSize = sizeof(OPENFILENAMEW);
                        ofn.lpstrTitle = (wchars) TitleName;
                        ofn.lpstrInitialDir = (wchars) InitDir;
                        ofn.lpstrFilter =
                            L"All File(*.*)\0"
                            L"*.*\0"
                            L"Text File\0"
                            L"*.txt;*.doc\0";
                        ofn.lpstrFile = ResultPath;
                        ofn.nMaxFile = _MAX_PATH - 4;

                        if(GetOpenFileNameW(&ofn))
                            Result = true;
                    }

                    if(Result)
                    {
                        path = String::FromWChars(ResultPath);
                        if(shortpath) *shortpath = String::FromWChars(Platform::File::GetShortName(ResultPath));
                    }
                    CoUninitialize();
                #endif
                return Result;
            }

            void Popup_WebBrowserDialog(String url)
            {
                #if BLIK_WINDOWS
                    #ifdef UNICODE
                        ShellExecute(NULL, L"open", (wchars) WString::FromChars(url),
                            NULL, NULL, SW_SHOWNORMAL);
                    #else
                        ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
                    #endif
                #endif
            }

            WString File_GetDirName(wchars itemname, wchar_t badslash, wchar_t goodslash)
            {
                if(itemname && itemname[0] != L'\0')
                {
                    String ItemnameUTF8 = String::FromWChars(itemname);
                    ItemnameUTF8.Replace(badslash, goodslash);
                    if(ItemnameUTF8[-2] == goodslash)
                        ItemnameUTF8.Sub(1);

                    if(Platform::File::ExistForDir(ItemnameUTF8))
                        return WString::FromChars(ItemnameUTF8);

                    for(sint32 i = ItemnameUTF8.Length() - 1; 0 <= i; --i)
                        if(ItemnameUTF8[i] == goodslash)
                            return WString::FromChars(ItemnameUTF8, i);
                }
                return L"";
            }

            WString File_GetShortName(wchars itemname)
            {
                if(itemname && itemname[0] != L'\0')
                {
                    String ItemnameUTF8 = String::FromWChars(itemname);
                    if(ItemnameUTF8[-2] == L'/' || ItemnameUTF8[-2] == L'\\')
                        ItemnameUTF8.Sub(1);

                    for(sint32 i = ItemnameUTF8.Length() - 1; 0 <= i; --i)
                        if(ItemnameUTF8[i] == L'/' || ItemnameUTF8[i] == L'\\')
                            return WString::FromChars(&((chars) ItemnameUTF8)[i + 1]);
                }
                return L"";
            }

            sint32 File_GetDriveCode()
            {
                static const sint32 DriveCode = Platform::File::GetFullPath((wchars) L"")[0] - L'A' + 1;
                return DriveCode;
            }

            bool File_Tempname(char* format, sint32 length)
            {
                static const char TempChars[36] = {
                    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                    'u', 'v', 'w', 'x', 'y', 'z'};

                String NewName;
                do
                {
                    NewName.Empty();
                    for(sint32 i = 0; i < length; ++i)
                    {
                        if(format[i] == 'X')
                            NewName += TempChars[Platform::Utility::Random() % 36];
                        else NewName += format[i];
                    }
                }
                while(Platform::File::CanAccess(WString::FromChars(NewName)));

                Memory::Copy(format, (chars) NewName, length);
                return true;
            }

            void* File_FDMap(sint32 fd, sint64 offset, sint64 size, bool readonly)
            {
                if(FileElement* CurElement = FileElementMap.Access(fd))
                {
                    BLIK_TRACE("FDMap(%d)", fd);
                    if(uint08* NewMap = CurElement->GetMap(fd, offset, size, readonly))
                    {
                        Platform::File::FDOpenRetain(fd);
                        BLIK_TRACE("FDMap(%d, %d, %d%s) - 0x%08X[%02X%02X%02X%02X...%02X%02X%02X%02X]",
                            fd, (sint32) offset, (sint32) size, (readonly)? ", readonly" : "",
                            NewMap, NewMap[0], NewMap[1], NewMap[2], NewMap[3],
                            NewMap[size - 4], NewMap[size - 3], NewMap[size - 2], NewMap[size - 1]);
                        return NewMap;
                    }
                }
                BLIK_TRACE("FDMap(%d, %d, %d%s) - Failed", fd, (sint32) offset, (sint32) size, (readonly)? ", readonly" : "");
                return nullptr;
            }

            bool File_FDUnmap(const void* map)
            {
                if(FileMMIOView* OldView = FileElement::FindMap((uint08*) map))
                {
                    BLIK_TRACE("FDUnmap(%d, 0x%08X)", OldView->m_fd, map);
                    OldView->ReleaseShare(map);
                    return true;
                }
                BLIK_TRACE("FDUnmap(0x%08X) - Failed", map);
                return false;
            }

            void File_ResetAssetsRoot(chars dirname)
            {
                PlatformImpl::Core::g_AssetsRoot = dirname;
                PlatformImpl::Core::g_AssetsRoot += '/';
            }

            void File_ResetAssetsRemRoot(chars dirname)
            {
                PlatformImpl::Core::g_AssetsRemRoot = dirname;
                PlatformImpl::Core::g_AssetsRemRoot += '/';
            }
        }
    }
}
