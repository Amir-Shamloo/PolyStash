/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_SELECTARRAYEX_H__
#define PRIVATE_SELECTARRAYEX_H__

#include "private_ge_mtools.h"
#include "c4d_hyperfile.h"
#include "c4d_file.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
template <class TYPE> class SelectArrayExt
{
private:
	SortedArray<TYPE, 4> m_Data;
	Int32								 m_SelectCount;
	Int32								 m_LastSegment;

	Int32 FindNum(Int32 num)
	{
		Int32 oseg = m_LastSegment;

		if (oseg < m_Data.GetCount() && num >= m_Data[oseg].m_Lower && num <= m_Data[oseg].m_Upper)
			return oseg;

		oseg = m_Data.SearchEqualIndex(TYPE(num));

		if (oseg == m_Data.GetCount())
			m_LastSegment = 0;
		else
			m_LastSegment = oseg;

		return oseg;
	}

public:
	SelectArrayExt()
	{
		m_LastSegment = 0;
		m_SelectCount = 0;
	}

	~SelectArrayExt() { }

	Int GetMemoryUsed()
	{
		return sizeof(this) + m_Data.GetCount() * sizeof(TYPE);
	}
	Int32 GetCount() const
	{
		return m_SelectCount;
	}
	Int32 GetSegments()
	{
		return m_Data.GetCount();
	}

	Bool Select(Int32 num)
	{
		if (m_Data.GetCount() == 0)
		{
			m_SelectCount++;
			return m_Data.AddLast(TYPE(num));
		}

		Int32 i = FindNum(num);

		if (i == m_Data.GetCount())
		{
			if (num > (m_Data[i - 1].m_Upper + 1))
			{
				if (!m_Data.Insert(i))
					return false;

				m_Data[i].m_Lower = num;
				m_Data[i].m_Upper = num;

				m_SelectCount++;

				return true;
			}
			else
			{
				i--;
			}
		}

		if (num >= m_Data[i].m_Lower && num <= m_Data[i].m_Upper)
			return true;

		m_SelectCount++;

		if (i > 0 && (m_Data[i - 1].m_Upper + 1) == num)
			i--;

		if ((m_Data[i].m_Upper + 1) == num)
		{
			m_Data[i].m_Upper++;
			if (i == (m_Data.GetCount() - 1) || (m_Data[i + 1].m_Lower - 1) != num)
				return true;

			m_Data[i].m_Upper = m_Data[i + 1].m_Upper;

			return m_Data.Remove(i + 1);
		}

		if ((m_Data[i].m_Lower - 1) == num)
		{
			m_Data[i].m_Lower--;
			if (i == 0 || (m_Data[i - 1].m_Upper + 1) != num)
				return true;

			m_Data[i - 1].m_Upper = m_Data[i].m_Upper;

			return m_Data.Remove(i);
		}

		if (!m_Data.Insert(i))
			return false;

		m_Data[i].m_Lower = num;
		m_Data[i].m_Upper = num;

		return true;
	}

	Bool SelectAll(Int32 min, Int32 max)
	{
		DeselectAll();
		if (max < min)
			max = min;
		m_SelectCount = max - min + 1;
		return m_Data.AddLast(TYPE(min, max));
	}

	Bool Deselect(Int32 num)
	{
		Int32 i = FindNum(num);
		if (i == m_Data.GetCount())
			return true;

		if (num >= m_Data[i].m_Lower && num <= m_Data[i].m_Upper)
		{
			m_SelectCount--;

			if (m_Data[i].m_Lower == m_Data[i].m_Upper)
				return m_Data.Remove(i);

			if (m_Data[i].m_Lower == num)
			{
				m_Data[i].m_Lower++;
				return true;
			}
			else if (m_Data[i].m_Upper == num)
			{
				m_Data[i].m_Upper--;
				return true;
			}
			else
			{
				if (!m_Data.Insert(i))
					return false;

				m_Data[i].m_Lower = m_Data[i + 1].m_Lower;
				m_Data[i].m_Upper = num - 1;
				m_Data[i + 1].m_Lower = num + 1;
			}
		}

		return true;
	}

	void DeselectAll()
	{
		m_Data.Free();
		m_SelectCount = 0;
	}

	Bool Toggle(Int32 num)
	{
		if (IsSelected(num))
			return Deselect(num);
		else
			return Select(num);
	}

	Bool ToggleAll(Int32 min, Int32 max)
	{
		Int32 i;

		for (i = min; i <= max; i++)
		{
			if (!Toggle(i))
				return false;
		}

		return true;
	}

	Bool GetRange(Int32 seg, Int32* a, Int32* b)
	{
		if (seg >= m_Data.GetCount())
			return false;

		*a = m_Data[seg].m_Lower;
		*b = m_Data[seg].m_Upper;

		return true;
	}

	Bool IsSelected(Int32 num)
	{
		Int32 seg;
		return FindSegment(num, &seg);
	}

	Bool FindSegment(Int32 num, Int32* segment)
	{
		*segment = NOTOK;

		if (m_Data.GetCount() == 0)
			return false;

		Int32 i = FindNum(num);
		if (i == m_Data.GetCount())
			return false;

		if (num >= m_Data[i].m_Lower && num <= m_Data[i].m_Upper)
		{
			*segment = i;
			return true;
		}

		return false;
	}

	TYPE GetSegmentData(Int32 seg)
	{
		return m_Data[seg];
	}
	TYPE* GetSegmentDataAt(Int32 seg)
	{
		return m_Data.GetAt(seg);
	}

	Bool GetData(Int32 num, TYPE* data)
	{
		if (m_Data.GetCount() == 0)
			return false;

		Int32 i = FindNum(num);
		if (i == m_Data.GetCount())
			return false;

		if (num >= m_Data[i].m_Lower && num <= m_Data[i].m_Upper)
		{
			*data = m_Data[i];
			return true;
		}

		return false;
	}

	Bool CopyTo(SelectArrayExt* dst) const
	{
		if (dst == this)
			return true;

		if (!m_Data.CopyTo(&dst->m_Data))
			return false;
		dst->m_SelectCount = m_SelectCount;
		return true;
	}
	/*
	 Bool CopyTo(BaseSelect *bs) const
	 {
	 if (!m_Data.GetCount()) return bs->DeselectAll();

	 if (!bs->CopyFrom(nullptr, m_Data.GetCount())) return false;

	 CopyMem(m_Data.GetArray(), bs->GetData(), sizeof(Int32)*2*m_Data.GetCount());

	 DebugAssert(sizeof(TYPE) == sizeof(Int32)*2);

	 return true;
	 }
	 */
	/*
	 Bool CopyFrom(BaseSelect *bs)
	 {
	 if (!bs->GetSegments())
	 {
	 DeselectAll();
	 return true;
	 }

	 if (!m_Data.SetCount(bs->GetSegments())) return false;

	 CopyMem(bs->GetData(), m_Data.GetArray(), sizeof(Int32)*2*m_Data.GetCount());

	 DebugAssert(sizeof(TYPE) == sizeof(Int32)*2);

	 m_SelectCount = bs->GetCount();

	 return true;
	 }
	 */
	Int32 GetLower()
	{
		if (!m_Data.GetCount())
			return NOTOK;
		return m_Data[0].m_Lower;
	}

	Int32 GetUpper()
	{
		if (!m_Data.GetCount())
			return NOTOK;
		return m_Data.GetLast()->m_Upper;
	}

	Bool Write(HyperFile* hf)
	{
		if (!hf->WriteInt32(m_SelectCount))
			return false;
		if (!hf->WriteInt32(m_LastSegment))
			return false;
		if (!hf->WriteInt32(m_Data.GetCount()))
			return false;

		if (m_Data.GetCount())
		{
			if (!TYPE::Write(hf, m_Data.GetArray(), m_Data.GetCount()))
				return false;
		}

		return true;
	}

	Bool Read(HyperFile* hf)
	{
		if (!hf->ReadInt32(&m_SelectCount))
			return false;
		if (!hf->ReadInt32(&m_LastSegment))
			return false;

		Int32 dcnt;

		if (!hf->ReadInt32(&dcnt))
			return false;

		if (dcnt)
		{
			if (hf->GetFileVersion() < 11600)
			{
				DebugAssert((sizeof(TYPE) % sizeof(Int32)) == 0);

				Int		cnt;
				void* data = nullptr;

				if (!_HFReadMemoryL(hf, (void**)&data, &cnt))
					return false;

				if (!m_Data.CopyFrom((TYPE*)data, dcnt))
				{
					DeleteMem(data);
					return false;
				}

				DeleteMem(data);
			}
			else
			{
				if (!m_Data.SetCount(dcnt))
					return false;
				if (!TYPE::Read(hf, m_Data.GetArray(), dcnt))
					return false;
			}
		}

		return true;
	}
};

#pragma pack (pop)
}

#endif	// PRIVATE_SELECTARRAYEX_H__
