/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_JOINTWEIGHT_H__
#define C4D_JOINTWEIGHT_H__

#include "private_selectarrayex.h"
#include "private_ge_thread.h"
#include "c4d_hyperfile.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup JOINTWEIGHT
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define JOINTWEIGHT_CACHEFLAG_MODIFIED (1 << 0)
#define JOINTWEIGHT_CACHEFLAG_READONLY (1 << 1)
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class JointWeight
{
	class JointWeightData
	{
public:
		JointWeightData()
		{
			m_Lower	 = 0;
			m_Upper	 = 0;
			m_wIndex = NOTOK;
		}

		JointWeightData(Int32 index)
		{
			m_Upper	 = m_Lower = index;
			m_wIndex = NOTOK;
		}

		JointWeightData(Int32 lower, Int32 upper)
		{
			m_Upper	 = upper;
			m_Lower	 = lower;
			m_wIndex = NOTOK;
		}

		Bool operator <(const JointWeightData& dst) const
		{
			return m_Lower < dst.m_Lower;
		}

		Bool operator >(const JointWeightData& dst) const
		{
			return m_Lower > dst.m_Upper;
		}

		Bool operator <=(const JointWeightData& dst) const
		{
			return m_Lower <= dst.m_Upper;
		}

		Bool operator >=(const JointWeightData& dst) const
		{
			return m_Lower >= dst.m_Lower;
		}

		Bool operator ==(const JointWeightData& dst) const
		{
			return m_Lower >= dst.m_Lower && m_Lower <= dst.m_Upper;
		}

		JointWeightData& operator =(const JointWeightData& src)
		{
			m_Lower	 = src.m_Lower;
			m_Upper	 = src.m_Upper;
			m_wIndex = src.m_wIndex;
			return *this;
		}

		static Bool Write(HyperFile* hf, JointWeightData* data, Int32 cnt)
		{
			return hf->WriteArray(data, HYPERFILEARRAY_LONG, sizeof(Int32), cnt * 3);
		}

		static Bool Read(HyperFile* hf, JointWeightData* data, Int32 cnt)
		{
			return hf->ReadArray(data, HYPERFILEARRAY_LONG, sizeof(Int32), cnt * 3);
		}

		Int32	m_Lower;
		Int32	m_Upper;
		Int32	m_wIndex;
	};

public:
	JointWeight();
	~JointWeight();

	Bool Write(HyperFile* hf);
	Bool Read(HyperFile* hf);
	Bool CopyTo(JointWeight* dst) const;

	Float GetWeight(Int32 wi, Bool direct = false);
	Bool SetWeight(Int32 wi, Float w, Bool direct = false);

	Bool SetWeights(Float32* weights, Int32 count);
	Bool SetWeights(Float64* weights, Int32 count);
	Bool SetWeights(UInt16* weights, Int32 count);

	void FillWeights(Float32* weights, Int32 count);
	void FillWeights(Float64* weights, Int32 count);
	void FillWeights(UInt16* weights, Int32 count);

	Bool InitCache(Int32 count, Bool readonly = false);
	Bool FreeCache(Bool set);

	void Free();
	Bool HasCache();

	Int32 GetCount();
	Int32 GetPointCount();

	void GetIndexWeight(Int32 i, Int32& wi, Float& w);

	Bool ShiftWeights(Int32 offset, Int32 cnt);

	void LockRead();
	void UnlockRead();
	void LockWrite();
	void UnlockWrite();

	Int32 GetSegments();
	UInt16 GetTableWeight(Int32 i);

private:
	void _Free();
	Bool _FreeCache(Bool set);

	void _FillWeights(Float64* weights, Int32 count);
	void _FillWeights(Float32* weights, Int32 count);
	void _FillWeights(UInt16* weights, Int32 count);

	void IndexWeights();
	Int32 GetWeightIndex(Int32 wi);

	SelectBitArray									m_Select;		// Point indexes.
	SelectArrayExt<JointWeightData>	m_Index;		// Indexing table.

	UInt16*													m_pWeight;	// Array of weights for all indexes.

	GeRWSpinlock										m_Lock;

	// TEMP ONLY during Init->FreeCache().

	UInt16*	m_pCache;
	Int32		m_CacheCount;
	Int32		m_CacheFlags;
	Int32		m_CacheLowIndex;
	Int32		m_CacheUsed;
};

#pragma pack (pop)
}

#endif	// C4D_JOINTWEIGHT_H__
