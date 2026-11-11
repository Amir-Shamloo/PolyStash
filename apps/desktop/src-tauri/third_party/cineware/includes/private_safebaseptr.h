/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_SAFEBASEPTR_H__
#define PRIVATE_SAFEBASEPTR_H__

#include "c4d_system.h"
#include "private_symbols.h"
#include "c4d_baselink.h"
#include "c4d_gelistnode.h"
#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;
class BaseDocument;

/// @addtogroup group_safebaseptr_defines SAFEBASEPTR
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define SAFEBASEPTR_LEVEL						2
#define SAFEBASEPTR_ID							1022430
#define SAFEBASEPTR_FLAGS_PROCESSED	(1 << 0)
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class SafeBasePtr
{
public:
	SafeBasePtr()
	{
		m_pLink = nullptr;
		m_GUID	= 0;
		m_Dirty = (UInt32)NOTOK;
		m_Flags = 0;
	}

	// Only allow marker (Set() must then be called). DO NOT use link with constructor
	SafeBasePtr(UInt64 guid)
	{
		m_pLink = nullptr;
		m_GUID	= guid;
		m_Dirty = (UInt32)NOTOK;
		m_Flags = 0;
	}

	~SafeBasePtr()
	{
		BaseLink::Free(m_pLink);
	}

	BaseList2D* Get(BaseDocument* pDoc, Int32 base = 0) const
	{
		if (!m_pLink)
			return nullptr;
		return (BaseList2D*)m_pLink->GetLink();
	}

	Bool Refresh(BaseDocument* pDoc, Int32 dflags = DIRTYFLAGS_DATA | DIRTYFLAGS_MATRIX);
	Bool Set(BaseList2D* op, Int32 dflags = DIRTYFLAGS_DATA | DIRTYFLAGS_MATRIX);

	SafeBasePtr(const SafeBasePtr& src)
	{
		m_GUID	= 0;
		m_Dirty = (UInt32)NOTOK;
		m_Flags = 0;
		m_pLink = nullptr;
		CopyFrom(src, nullptr);
	}

	SafeBasePtr& operator =(const SafeBasePtr& src)
	{
		CopyFrom(src, nullptr);
		return *this;
	}

	Bool CopyFrom(const SafeBasePtr& src, AliasTrans* trn)
	{
		if (!m_pLink && src.m_pLink)
			m_pLink = BaseLink::Alloc();

		if (m_pLink || !src.m_pLink)
		{
			if (m_pLink && src.m_pLink)
				src.m_pLink->CopyData(m_pLink, trn);
			if (!src.m_pLink && m_pLink)
				m_pLink->SetLink(nullptr);

			m_Dirty = src.m_Dirty;
			m_GUID	= src.m_GUID;
			m_Flags = src.m_Flags;
		}
		else
		{
			m_Dirty = (UInt32)NOTOK;
			m_GUID	= 0;
			m_Flags = 0;
			return false;
		}

		return true;
	}

	UInt32 GetDirty()
	{
		return m_Dirty;
	}
	void SetDirty(UInt32 dirty)
	{
		m_Dirty = dirty;
	}

	UInt64 GetGUID()
	{
		return m_GUID;
	}
	void SetGUID(UInt64 guid)
	{
		m_GUID = guid;
	}

	BaseLink* GetLink()
	{
		return m_pLink;
	}

	Bool Write(HyperFile* hf)
	{
		DebugAssert(m_pLink);

		if (!m_pLink)
			return false;

		if (!hf->WriteUInt32(m_Dirty))
			return false;
		if (!m_pLink->Write(hf))
			return false;

		return true;
	}

	Bool Read(HyperFile* hf)
	{
		if (!m_pLink)
			m_pLink = BaseLink::Alloc();
		if (!m_pLink)
			return false;

		if (!hf->ReadUInt32(&m_Dirty))
			return false;
		if (!m_pLink->Read(hf))
			return false;

		return true;
	}

	Bool ReadEx(HyperFile* hf)
	{
		Int32 cid, clevel;

		HYPERFILEVALUE h;

		if (hf->ReadValueHeader(&h) && h == HYPERFILEVALUE_START)
		{
			if (!hf->ReadChunkStart(&cid, &clevel))
				return false;

			if (cid != SAFEBASEPTR_ID)
				return false;

			if (!m_pLink)
				m_pLink = BaseLink::Alloc();
			if (!m_pLink)
				return false;

			if (!hf->ReadUInt32(&m_Dirty))
				return false;
			if (!hf->ReadInt64((Int64*)&m_GUID))
				return false;
			if (!m_pLink->Read(hf))
				return false;

			if (clevel >= 2)
			{
				if (!hf->ReadUInt32(&m_Flags))
					return false;
			}

			if (!hf->SkipToEndChunk())
				return false;
		}

		return true;
	}

	Bool WriteEx(HyperFile* hf)
	{
		DebugAssert(m_pLink);

		if (!m_pLink)
			goto Error;

		if (!hf->WriteChunkStart(SAFEBASEPTR_ID, SAFEBASEPTR_LEVEL))
			goto Error;

		if (!hf->WriteUInt32(m_Dirty))
			goto Error;
		if (!hf->WriteInt64((Int64)m_GUID))
			goto Error;
		if (!m_pLink->Write(hf))
			goto Error;
		if (!hf->WriteUInt32(m_Flags))
			goto Error;

		if (!hf->WriteChunkEnd())
			goto Error;

		return true;

Error:

		hf->SetError(FILEERROR_OUTOFMEMORY);

		return false;
	}

	UInt32 GetFlags()
	{
		return m_Flags;
	}

	Bool GetFlag(UInt32 flags)
	{
		return (m_Flags & flags) == flags;
	}

	void SetFlags(UInt32 flags)
	{
		m_Flags = flags;
	}

	void SetFlag(UInt32 flag)
	{
		m_Flags |= flag;
	}

	void DelFlag(UInt32 flag)
	{
		m_Flags &= ~flag;
	}

	Bool CopyTo(BaseLink* dlink, AliasTrans* trn = nullptr)
	{
		if (!dlink)
			return false;

		if (!m_pLink)
			dlink->SetLink(nullptr);
		else
			m_pLink->CopyData(dlink, trn);

		return true;
	}

	Bool CopyFrom(BaseLink* dlink, AliasTrans* trn = nullptr)
	{
		if (!dlink)
			return false;

		if (!m_pLink)
		{
			m_pLink = BaseLink::Alloc();
			if (!m_pLink)
				return false;
		}

		dlink->CopyData(m_pLink, trn);
		return true;
	}

	Bool operator <(const SafeBasePtr& dst) const
	{
		return m_GUID < dst.m_GUID;
	}

	Bool operator >(const SafeBasePtr& dst) const
	{
		return m_GUID > dst.m_GUID;
	}

	Bool operator <=(const SafeBasePtr& dst) const
	{
		return m_GUID <= dst.m_GUID;
	}

	Bool operator >=(const SafeBasePtr& dst) const
	{
		return m_GUID >= dst.m_GUID;
	}

	Bool operator ==(const SafeBasePtr& dst) const
	{
		return m_GUID == dst.m_GUID;
	}

private:
	BaseLink*	m_pLink;
	UInt32		m_Dirty;

	// Extended read/write only
	UInt64 m_GUID;
	UInt32 m_Flags;
};

#pragma pack (pop)
}

#endif	// PRIVATE_SAFEBASEPTR_H__
