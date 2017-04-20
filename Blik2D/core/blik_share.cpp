#include <blik.hpp>
#include "blik_share.hpp"

namespace BLIK
{
    const Share* Share::Create(buffer gift)
    {
        return new Share(gift);
    }

    const Share* Share::Create(const buffer& sample, sint32 mincount)
    {
        return new Share(sample, mincount);
    }

    void Share::Destroy(id_cloned_share cloned)
    {
        const Share* OldShare = (const Share*) cloned;
        Share::Remove(OldShare);
    }

    const Share* Share::CopyOnWrite(const Share* share, sint32 mincount)
    {
        if(share)
        {
            if(1 < share->mShareCount) // 독립
            {
                --_DebugShareCount();
                --share->mShareCount;
                // 사본화, 전체삭제의 경우에는 예외
                share = new Share(*share, mincount, mincount != -2);
            }
            if(share->mValidCount < mincount) // 확장
            {
                Share* wshare = (Share*) share;
                wshare->mValidCount = mincount;
                sint32 count = Math::Max(1, Buffer::CountOf(share->mData));
                while(count < mincount) count <<= 1;
                wshare->mData = Buffer::Realloc(BLIK_DBG share->mData, count);
            }
            else if(mincount == -1 && 0 < share->mValidCount) // 후미삭제
            {
                Share* wshare = (Share*) share;
                Buffer::ResetOne(wshare->mData, --wshare->mValidCount);
            }
            else if(mincount == -2 && 0 < share->mValidCount) // 전체삭제
            {
                Share* wshare = (Share*) share;
                wshare->mValidCount = 0;
                Buffer::ResetAll(wshare->mData);
            }
        }
        return share;
    }

    void Share::Remove(const Share*& share)
    {
        if(share)
        {
            --_DebugShareCount();
            if(--share->mShareCount == 0)
                delete share;
            share = nullptr;
        }
    }

    void Share::DebugPrint()
    {
        BLIK_TRACE("");
        BLIK_TRACE("==================== Share ====================");
        BLIK_TRACE(">>>> %d EA", _DebugShareCount());
        BLIK_TRACE("===============================================");
    }

    chars Share::Type() const
    {
        return Buffer::NameOf(mData);
    }

    sblock Share::TypeID() const
    {
        return Buffer::TypeOf(mData);
    }

    const Share* Share::Clone() const
    {
        ++_DebugShareCount();
        ++mShareCount;
        return this;
    }

    buffer Share::CopiedBuffer() const
    {
        const sint32 count = Buffer::CountOf(mData);
        buffer NewBuffer = Buffer::AllocBySample(BLIK_DBG count, mData);
        Buffer::Copy(NewBuffer, 0, mData, 0, count);
        return NewBuffer;
    }

    Share::Share(buffer gift)
    {
        ++_DebugShareCount();
        mShareCount = 1;
        mValidCount = Buffer::CountOf(gift);
        mData = gift;
    }

    Share::Share(const buffer& sample, sint32 mincount)
    {
        ++_DebugShareCount();
        mShareCount = 1;
        mValidCount = 0;
        mData = Buffer::AllocBySample(BLIK_DBG mincount, sample);
    }

    Share::Share(const Share& rhs, sint32 mincount, bool duplicate)
    {
        const sint32 rhscount = Buffer::CountOf(rhs.mData);
        const sint32 maxcount = (rhscount > mincount)? rhscount : mincount;

        ++_DebugShareCount();
        mShareCount = 1;
        mValidCount = 0;
        mData = Buffer::AllocBySample(BLIK_DBG maxcount, rhs.mData);

        if(duplicate) // 사본으로 구성
        {
            mValidCount = rhs.mValidCount;
            Buffer::Copy(mData, 0, rhs.mData, 0, mValidCount);
        }
    }

    Share::~Share()
    {
        Buffer::Free(mData);
    }

    sint32& Share::_DebugShareCount()
    {
        static sint32 ShareCount = 0;
        return ShareCount;
    }
}
