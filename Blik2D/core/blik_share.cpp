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
            if(1 < share->sharecount) // 독립
            {
                --_DebugShareCount();
                --share->sharecount;
                // 사본화, 전체삭제의 경우에는 예외
                share = new Share(*share, mincount, mincount != -2);
            }
            if(share->validcount < mincount) // 확장
            {
                Share* wshare = (Share*) share;
                wshare->validcount = mincount;
                sint32 count = Math::Max(1, Buffer::CountOf(share->data));
                while(count < mincount) count <<= 1;
                wshare->data = Buffer::Realloc(BLIK_DBG share->data, count);
            }
            else if(mincount == -1 && 0 < share->validcount) // 후미삭제
            {
                Share* wshare = (Share*) share;
                Buffer::ResetOne(wshare->data, --wshare->validcount);
            }
			else if(mincount == -2 && 0 < share->validcount) // 전체삭제
            {
                Share* wshare = (Share*) share;
				wshare->validcount = 0;
                Buffer::ResetAll(wshare->data);
            }
        }
        return share;
    }

    void Share::Remove(const Share*& share)
    {
        if(share)
        {
            --_DebugShareCount();
            if(--share->sharecount == 0)
            {
                delete share;
                share = nullptr;
            }
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
        return Buffer::NameOf(data);
    }

	sblock Share::TypeID() const
	{
		return Buffer::TypeOf(data);
	}

    const Share* Share::Clone() const
    {
        ++_DebugShareCount();
        ++sharecount;
        return this;
    }

    sint32 Share::Count() const
    {
        return validcount;
    }

    buffer Share::CopiedBuffer() const
    {
        const sint32 count = Buffer::CountOf(data);
        buffer NewBuffer = Buffer::AllocBySample(BLIK_DBG count, data);
        Buffer::Copy(NewBuffer, 0, data, 0, count);
        return NewBuffer;
    }

    Share::Share(buffer gift)
    {
        ++_DebugShareCount();
        sharecount = 1;
        validcount = Buffer::CountOf(gift);
        data = gift;
    }

    Share::Share(const buffer& sample, sint32 mincount)
    {
        ++_DebugShareCount();
        sharecount = 1;
        validcount = 0;
        data = Buffer::AllocBySample(BLIK_DBG mincount, sample);
    }

    Share::Share(const Share& rhs, sint32 mincount, bool duplicate)
    {
        const sint32 rhscount = Buffer::CountOf(rhs.data);
        const sint32 maxcount = (rhscount > mincount)? rhscount : mincount;

        ++_DebugShareCount();
        sharecount = 1;
        validcount = 0;
        data = Buffer::AllocBySample(BLIK_DBG maxcount, rhs.data);

        if(duplicate) // 사본으로 구성
        {
            validcount = rhs.validcount;
            Buffer::Copy(data, 0, rhs.data, 0, validcount);
        }
    }

    Share::~Share()
    {
        Buffer::Free(data);
    }

    sint32& Share::_DebugShareCount()
    {
        static sint32 ShareCount = 0;
        return ShareCount;
    }
}
