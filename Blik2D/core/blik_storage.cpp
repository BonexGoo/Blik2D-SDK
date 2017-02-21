#include <blik.hpp>
#include "blik_storage.hpp"

// 단위객체
class StorageUnit
{
public:
    Storage::DeleteCB Delete;
    ClearType Type;
    void* Ptr;
public:
    StorageUnit() {Delete = nullptr; Type = CT_Null; Ptr = nullptr;}
    ~StorageUnit() {}
};
typedef Array<StorageUnit, datatype_class_canmemcpy, 256> StorageUnits;

// 전역변수
static sint32 blik_storage_lastindex = -1;
#if HAS_CXX11_THREAD_LOCAL
    thread_local StorageUnits* blik_storage_root;
#elif defined(_MSC_VER)
    __declspec(thread) StorageUnits* blik_storage_root;
#elif defined(__GNUC__)
    #if BLIK_IPHONE
        static StorageUnits* blik_storage_root; // 수정해야 함!!!!!
    #else
        __thread StorageUnits* blik_storage_root;
    #endif
#else
    #error Unknown compiler
#endif

// 관리객체
class StorageClass
{
public:
    const sint32 Index;

public:
    StorageUnit& GetLocalUnit()
    {
        if(!blik_storage_root)
            blik_storage_root = new StorageUnits();
        return blik_storage_root->AtWherever(Index);
    }

    bool ClearLocalData()
    {
        StorageUnit& CurUnit = GetLocalUnit();
        if(CurUnit.Ptr)
        {
            CurUnit.Delete(CurUnit.Ptr);
            CurUnit.Ptr = nullptr;
            return true;
        }
        return false;
    }

    static sint32 ClearLocalDataAll(ClearType type)
    {
        sint32 ClearCount = 0;
        if(blik_storage_root)
        for(sint32 i = blik_storage_root->Count() - 1; 0 <= i; --i)
        {
            StorageUnit& CurUnit = blik_storage_root->At(i);
            if(CurUnit.Type == type && CurUnit.Ptr)
            {
                CurUnit.Delete(CurUnit.Ptr);
                CurUnit.Ptr = nullptr;
                ClearCount++;
            }
        }
        return ClearCount;
    }

public:
    StorageClass(Storage::DeleteCB cb, ClearType type) : Index(++blik_storage_lastindex)
    {
        StorageUnit& NewUnit = GetLocalUnit();
        NewUnit.Delete = cb;
        NewUnit.Type = type;
    }
    ~StorageClass() {}
};

namespace BLIK
{
    id_storage Storage::Create(DeleteCB cb, ClearType type)
    {
        BLIK_ASSERT("cb인수가 nullptr입니다", cb);
        StorageClass* NewStorage = new StorageClass(cb, type);
        return (id_storage) NewStorage;
    }

    void** Storage::Bind(id_storage storage)
    {
        BLIK_ASSERT("storage인수가 nullptr입니다", storage);
        StorageClass* CurStorage = (StorageClass*) storage;
        StorageUnit& CurUnit = CurStorage->GetLocalUnit();
        return &CurUnit.Ptr;
    }

    bool Storage::Clear(id_storage storage)
    {
        BLIK_ASSERT("storage인수가 nullptr입니다", storage);
        StorageClass* CurStorage = (StorageClass*) storage;
        return CurStorage->ClearLocalData();
    }

    sint32 Storage::ClearAll(ClearLevel level)
    {
        sint32 ClearCount = StorageClass::ClearLocalDataAll(CT_User);
        if(level == CL_SystemAndUser)
            ClearCount += StorageClass::ClearLocalDataAll(CT_System);
        return ClearCount;
    }
}
