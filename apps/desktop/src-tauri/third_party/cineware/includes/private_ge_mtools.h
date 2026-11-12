/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_GE_MTOOLS_H__
#define PRIVATE_GE_MTOOLS_H__

#include "ge_math.h"
#include "c4d_memory.h"
#include "private_ge_sort.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class BaseFile;
class HyperFile;
class BaseSelect;
class GeMarker;
class C4DAtom
{
};

template <class T> inline void swap(T& x, T& y)
{
	T z = x;
	x = y;
	y = z;
}

////////////////////////////////

#ifndef GCC
	#define STATIC_CAST(a, b) static_cast<a>(b)
	#define CONST_CAST(a, b)	const_cast<a>(b)
#else
	#define STATIC_CAST(a, b) (a)(b)
	#define CONST_CAST(a, b)	(a)(b)
#endif

////////////////////////////////

#define GE_TOOL_NOT_FOUND (-1)

////////////////////////////////

class GeToolNode2D
{
	friend class GeToolList2D;

protected:
	GeToolNode2D() : _next(nullptr), _prev(nullptr) { }
	virtual ~GeToolNode2D();

	void*	GetNext() const
	{
		return (_next && _next->_next) ? STATIC_CAST(void*, _next) : nullptr;
	}
	void*	GetPrev() const
	{
		return (_prev && _prev->_prev) ? STATIC_CAST(void*, _prev) : nullptr;
	}

	void InsertBefore(void* nv);
	void InsertAfter(void* nv);
	void Remove();

private:
	GeToolNode2D*	_next;
	GeToolNode2D*	_prev;
};

class GeToolNode4D;

class GeToolList2D
{
	friend class GeToolList4D;
	friend class GeToolNode4D;

protected:
	GeToolList2D();
	virtual ~GeToolList2D();

	void*	GetFirst() const
	{
		return (head._next != &tail) ? STATIC_CAST(void*, head._next) : nullptr;
	}
	void*	GetLast() const
	{
		return (tail._prev != &head) ? STATIC_CAST(void*, tail._prev) : nullptr;
	}
	void*	GetIndex(Int32 index) const;

	void Insert(void* nv);
	void InsertAfter(void* nv, void* pv);
	void Append(void* nv);

	void Clear();
	void FlushAll();
	void RemoveAll();

	Int32	GetCount() const;

private:
	GeToolNode2D	head;
	GeToolNode2D	tail;
	GeToolNode4D*	node;
};

class GeToolNode4D : public GeToolNode2D
{
	friend class GeToolList4D;

protected:
	GeToolNode4D()
	{
		list.node = this;
	}
	virtual ~GeToolNode4D() { }

	void InsertUnder(void* nv)
	{
		if (nv)
			((GeToolNode4D*)nv)->list.Insert(this);
	}
	void InsertUnderLast(void* nv)
	{
		if (nv)
			((GeToolNode4D*)nv)->list.Append(this);
	}

	void*	GetDown() const
	{
		return list.GetFirst();
	}
	void*	GetUp() const
	{
		return list.node;
	}

private:
	GeToolList2D list;
};

class GeToolList4D : public GeToolList2D
{
protected:
	virtual ~GeToolList4D() { }
};

////////////////////////////////

// GeTempPrivateNode2D - read only access

template <class TYPE>
class GeTempPrivateNode2D : private GeToolNode2D
{
public:
	TYPE* GetNext() const
	{
		return STATIC_CAST(TYPE*, GeToolNode2D::GetNext());
	}
	TYPE* GetPrev() const
	{
		return STATIC_CAST(TYPE*, GeToolNode2D::GetPrev());
	}
};

// GeTempPrivateList2D - read only access

template <class TYPE>
class GeTempPrivateList2D : private GeToolList2D
{
public:
	TYPE* GetFirst() const
	{
		return STATIC_CAST(TYPE*, GeToolList2D::GetFirst());
	}
	TYPE* GetLast() const
	{
		return STATIC_CAST(TYPE*, GeToolList2D::GetLast());
	}
	TYPE* GetIndex(Int32 index) const
	{
		return STATIC_CAST(TYPE*, GeToolList2D::GetIndex(index));
	}
	Int32	GetCount() const
	{
		return GeToolList2D::GetCount();
	}
};

// GeTempPublicNode2D - read & write access

template <class TYPE>
class GeTempPublicNode2D : private GeToolNode2D
{
public:
	TYPE* GetNext() const
	{
		return STATIC_CAST(TYPE*, GeToolNode2D::GetNext());
	}
	TYPE* GetPrev() const
	{
		return STATIC_CAST(TYPE*, GeToolNode2D::GetPrev());
	}

	void InsertBefore(TYPE* n)
	{
		GeToolNode2D::InsertBefore(STATIC_CAST(void*, n));
	}
	void InsertAfter(TYPE* n)
	{
		GeToolNode2D::InsertAfter(STATIC_CAST(void*, n));
	}
	void Remove()
	{
		GeToolNode2D::Remove();
	}
};

// GeTempPublicList2D - read & write access

template <class TYPE>
class GeTempPublicList2D : private GeToolList2D
{
public:
	TYPE* GetFirst() const
	{
		return STATIC_CAST(TYPE*, GeToolList2D::GetFirst());
	}
	TYPE* GetLast() const
	{
		return STATIC_CAST(TYPE*, GeToolList2D::GetLast());
	}
	TYPE*	GetIndex(Int32 index) const
	{
		return STATIC_CAST(TYPE*, GeToolList2D::GetIndex(index));
	}
	Int32	GetCount() const
	{
		return GeToolList2D::GetCount();
	}

	void Insert(TYPE* n)
	{
		GeToolList2D::Insert(STATIC_CAST(void*, n));
	}
	void InsertAfter(TYPE* n, TYPE* p)
	{
		GeToolList2D::InsertAfter(STATIC_CAST(void*, n), STATIC_CAST(void*, p));
	}
	void Append(TYPE* n)
	{
		GeToolList2D::Append(STATIC_CAST(void*, n));
	}

	void FlushAll()
	{
		GeToolList2D::FlushAll();
	}
	void RemoveAll()
	{
		GeToolList2D::RemoveAll();
	}
};

// nodes with additional type info

template <class TYPE, class CLASS>
class GeTempExtPrivateNode2D : public GeTempPrivateNode2D<CLASS>
{
public:
	const TYPE&	GetType() const
	{
		return type;
	}
	void SetType(const TYPE& p_type)
	{
		type = p_type;
	}

private:
	TYPE type;
};

template <class TYPE, class CLASS>
class GeTempExtPublicNode2D : private GeTempPublicNode2D<CLASS>
{
public:
	const TYPE& GetType() const
	{
		return type;
	}
	void SetType(const TYPE& p_type)
	{
		type = p_type;
	}

private:
	TYPE type;
};

////////////////////////////////

// GeTempPrivateNode4D - read only access

template <class TYPE>
class GeTempPrivateNode4D : private GeToolNode4D
{
public:
	TYPE* GetNext() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetNext());
	}
	TYPE* GetPrev() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetPrev());
	}
	TYPE* GetDown() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetDown());
	}
	TYPE* GetUp() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetUp());
	}
};

// GeTempPrivateList4D - read only access

template <class TYPE>
class GeTempPrivateList4D : private GeToolList4D
{
public:
	TYPE* GetFirst() const
	{
		return STATIC_CAST(TYPE*, GeToolList4D::GetFirst());
	}
	TYPE* GetLast() const
	{
		return STATIC_CAST(TYPE*, GeToolList4D::GetLast());
	}
	TYPE*	GetIndex(Int32 index) const
	{
		return STATIC_CAST(TYPE*, GeToolList4D::GetIndex(index));
	}
	Int32	GetCount() const
	{
		return GeToolList4D::GetCount();
	}
};

// GeTempPublicNode4D - read & write access

template <class TYPE>
class GeTempPublicNode4D : private GeToolNode4D
{
public:
	TYPE* GetNext() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetNext());
	}
	TYPE* GetPrev() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetPrev());
	}
	TYPE* GetDown() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetDown());
	}
	TYPE* GetUp() const
	{
		return STATIC_CAST(TYPE*, GeToolNode4D::GetUp());
	}

	void InsertBefore(TYPE* n)
	{
		GeToolNode4D::InsertBefore(STATIC_CAST(void*, n));
	}
	void InsertAfter(TYPE* n)
	{
		GeToolNode4D::InsertAfter(STATIC_CAST(void*, n));
	}
	void InsertUnder(TYPE* n)
	{
		GeToolNode4D::InsertUnder(STATIC_CAST(void*, n));
	}
	void InsertUnderLast(TYPE* n)
	{
		GeToolNode4D::InsertUnderLast(STATIC_CAST(void*, n));
	}
	void Remove()
	{
		GeToolNode4D::Remove();
	}
};

// GeTempPublicList4D - read & write access

template <class TYPE>
class GeTempPublicList4D : private GeToolList4D
{
public:
	TYPE* GetFirst() const
	{
		return STATIC_CAST(TYPE*, GeToolList4D::GetFirst());
	}
	TYPE* GetLast() const
	{
		return STATIC_CAST(TYPE*, GeToolList4D::GetLast());
	}
	TYPE* GetIndex(Int32 index) const
	{
		return STATIC_CAST(TYPE*, GeToolList4D::GetIndex(index));
	}
	Int32 GetCount() const
	{
		return GeToolList4D::GetCount();
	}

	void Insert(TYPE* n)
	{
		GeToolList4D::Insert(STATIC_CAST(void*, n));
	}
	void Append(TYPE* n)
	{
		GeToolList4D::Append(STATIC_CAST(void*, n));
	}

	void FlushAll()
	{
		GeToolList4D::FlushAll();
	}
	void RemoveAll()
	{
		GeToolList4D::RemoveAll();
	}
};

// nodes with additional type info

template <class TYPE, class CLASS>
class GeTempExtPrivateNode4D : public GeTempPrivateNode4D<CLASS>
{
public:
	const TYPE&	GetType() const
	{
		return type;
	}
	void SetType(const TYPE& p_type)
	{
		type = p_type;
	}

private:
	TYPE type;
};

template <class TYPE, class CLASS>
class GeTempExtPublicNode4D : public GeTempPublicNode4D<CLASS>
{
public:
	const TYPE&	GetType() const
	{
		return type;
	}
	void SetType(const TYPE& p_type)
	{
		type = p_type;
	}

private:
	TYPE type;
};

////////////////////////////////

template <class TYPE> inline void GeTempSwap(TYPE& a, TYPE& b)
{
	TYPE t = a;
	a = b;
	b = t;
}

class GeToolDynArray
{
	friend class GeToolDynClassArray;

private:
	void** array;
	Int32	 count;	// actual array entries
	Int32	 size;	// allocated array entries
	Int32	 init;	// nr of entries for the first alloc
	Int32	 step;	// enlarge quantum for succeeding allocs

protected:
	GeToolDynArray(Int32 p_init = 1, Int32 p_step = 0) : array(nullptr), count(0), size(0), init(p_init), step(p_step) { }
	virtual ~GeToolDynArray();

	void** GetRoot()
	{
		return array;
	}

	void SetAlloc(Int32 p_init = 1, Int32 p_step = 0)
	{
		init = p_init;
		step = p_step;
	}
	Int32	GetCount() const
	{
		return count;
	}
	void* GetIndex(Int32 i) const
	{
		if (i >= 0 && i < count)
			return array[i];
		return nullptr;
	}
	void* GetFirst() const
	{
		if (count > 0)
			return array[0];
		return nullptr;
	}
	void* GetLast() const
	{
		if (count > 0)
			return array[count - 1];
		return nullptr;
	}

	Bool SetIndex(Int32 i, void* e) const
	{
		if (i < 0 || i >= count)
			return false;
		array[i] = e;
		return true;
	}

	void Swap(Int32 a, Int32 b);
	void Move(Int32 source, Int32 dest);
	void Sort(Int32 (* compare)(void* a, void* b));
	void* Search(void* key, Int32 (* search)(void* a, void* b));

	void* Pop();
	Bool Append(void* e);
	Bool Insert(void* e, Int32 n);
	Bool Remove(Int32 n);
	Bool Remove(void* e);
	Int32	Find(const void* e) const;

	void ResetCount();
	void FlushThis();

	Bool CopyTo(GeToolDynArray* dest) const;

private:
	Bool Enlarge();
};

////////////////////////////////

template <class TYPE>
class GeTempDynArray : private GeToolDynArray
{
protected:
	void** GetRoot()
	{
		return GeToolDynArray::GetRoot();
	}

public:
	GeTempDynArray(Int32 p_init = 1, Int32 p_step = 0) : GeToolDynArray(p_init, p_step) { }

	void SetAlloc(Int32 p_init = 1, Int32 p_step = 0)
	{
		GeToolDynArray::SetAlloc(p_init, p_step);
	}
	Int32	GetCount() const
	{
		return GeToolDynArray::GetCount();
	}
	TYPE* GetIndex(Int32 i) const
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::GetIndex(i));
	}
	TYPE* GetFirst() const
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::GetFirst());
	}
	TYPE* GetLast() const
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::GetLast());
	}
	Bool SetIndex(Int32 i, TYPE* e) const
	{
		return GeToolDynArray::SetIndex(i, STATIC_CAST(void*, e));
	}

	void Swap(Int32 a, Int32 b)
	{
		GeToolDynArray::Swap(a, b);
	}
	void Move(Int32 source, Int32 dest)
	{
		GeToolDynArray::Move(source, dest);
	}
	void Sort(Int32 (* compare)(TYPE** a, TYPE** b))
	{
		GeToolDynArray::Sort((Int32 (*)(void* a, void* b))compare);
	}
	TYPE** Search(void* key, Int32 (* search)(void* key, TYPE** b))
	{
		return (TYPE**)GeToolDynArray::Search(key, (Int32 (*)(void* a, void* b))search);
	}

	Bool Append(TYPE* e)
	{
		return GeToolDynArray::Append(STATIC_CAST(void*, e));
	}

	TYPE*	AppendNew()
	{
		TYPE* e = NewObj(TYPE);
		if (!e)
			return nullptr;
		if (!Append(e))
			DeleteObj(e);
		return e;
	}

	Bool Insert(TYPE* e, Int32 n)
	{
		return GeToolDynArray::Insert(STATIC_CAST(void*, e), n);
	}
	Bool Remove(Int32 n)
	{
		return GeToolDynArray::Remove(n);
	}
	Bool Remove(TYPE* e)
	{
		return GeToolDynArray::Remove(STATIC_CAST(void*, e));
	}
	Int32	Find(const TYPE* e) const
	{
		return GeToolDynArray::Find((void*)e);
	}
	void ResetCount()
	{
		GeToolDynArray::ResetCount();
	}
	void FlushThis()
	{
		GeToolDynArray::FlushThis();
	}
	Bool CopyTo(GeTempDynArray* dest) const
	{
		return GeToolDynArray::CopyTo(dest);
	}

	void FlushAll()
	{
		TYPE* c;
		Int32 i;

		for (i = 0; i < GeToolDynArray::GetCount(); ++i)
		{
			c = STATIC_CAST(TYPE*, GeToolDynArray::GetIndex(i));
			DeleteObj(c);
		}
		GeToolDynArray::FlushThis();
	}

	TYPE* operator [] (Int32 l)
	{
		return GetIndex(l);
	}
	const TYPE* operator [] (Int32 l) const
	{
		return GetIndex(l);
	}
};

template <class TYPE>
class GeTempDynStack : private GeToolDynArray
{
public:
	GeTempDynStack(Int32 p_init = 1, Int32 p_step = 0) : GeToolDynArray(p_init, p_step) { }

	void SetAlloc(Int32 p_init = 1, Int32 p_step = 0)
	{
		GeToolDynArray::SetAlloc(p_init, p_step);
	}
	Int32	GetCount() const
	{
		return GeToolDynArray::GetCount();
	}
	TYPE* GetIndex(Int32 i) const
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::GetIndex(i));
	}
	TYPE* GetFirst() const
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::GetFirst());
	}
	TYPE* GetLast() const
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::GetLast());
	}

	void Swap(Int32 a, Int32 b)
	{
		GeToolDynArray::Swap(a, b);
	}

	Bool Push(TYPE* e)
	{
		return GeToolDynArray::Append(STATIC_CAST(void*, e));
	}
	TYPE* Pop()
	{
		return STATIC_CAST(TYPE*, GeToolDynArray::Pop());
	}
	void ResetCount()
	{
		GeToolDynArray::ResetCount();
	}
	void FlushThis()
	{
		GeToolDynArray::FlushThis();
	}
	Bool CopyTo(GeTempDynStack* dest) const
	{
		return GeToolDynArray::CopyTo(dest);
	}

	void FlushAll()
	{
		TYPE* c;
		Int32 i;

		for (i = 0; i < GeToolDynArray::GetCount(); ++i)
		{
			c = STATIC_CAST(TYPE*, GeToolDynArray::GetIndex(i));
			DeleteObj(c);
		}
		GeToolDynArray::FlushThis();
	}
};



////////////////////////////////

template <class CLASS, class TYPE> class GeToolTempDynSortArraySort : public GeSortAndSearch
{
	CLASS* ptr;
	Int32	 (CLASS::* compare_func)(TYPE* a, TYPE* b);

public:
	GeToolTempDynSortArraySort(CLASS* t_ptr, Int32(CLASS::* compare)(TYPE* a, TYPE* b)) : ptr(t_ptr), compare_func(compare) { }

	Int32 Compare(void* a, void* b)
	{
		return (ptr->*compare_func)(*(TYPE**)a, *(TYPE**)b);
	}
};

template <class CLASS, class TYPE, class SEARCH> class GeToolTempDynSortArraySearch : public GeSortAndSearch
{
	CLASS* ptr;
	Int32	 (CLASS::* compare_func)(SEARCH* a, TYPE* b);

public:
	GeToolTempDynSortArraySearch(CLASS* t_ptr, Int32(CLASS::* search)(SEARCH* a, TYPE* b)) : ptr(t_ptr), compare_func(search) { }

	Int32 Compare(void* a, void* b)
	{
		return (ptr->*compare_func)((SEARCH*)a, *(TYPE**)b);
	}
};



template <class TYPE, class SEARCH, int FLUSH> class GeTempDynSortSearchArray : private GeTempDynArray<TYPE>
{
	Bool m_sorted;

protected:
	void CheckSort()
	{
		if (!m_sorted)
		{
			GeToolTempDynSortArraySort<GeTempDynSortSearchArray<TYPE, SEARCH, FLUSH>, TYPE> sort(this, &GeTempDynSortSearchArray<TYPE, SEARCH, FLUSH>::CompareFunc);
			sort.Sort(GeTempDynArray<TYPE>::GetRoot(), GeTempDynArray<TYPE>::GetCount(), sizeof(TYPE*));

			((GeTempDynSortSearchArray*)this)->m_sorted = true;
			SortNotify();
		}
	}

	virtual void SortNotify() { }

private:
	// OVERRIDE THIS
	virtual Int32 CompareFunc(TYPE* aa, TYPE* bb) = 0;
	virtual Int32 SearchFunc(SEARCH* data, TYPE* bb) = 0;
	// OVERRIDE THIS

public:
	GeTempDynSortSearchArray()
	{
		m_sorted = false;
	}

	~GeTempDynSortSearchArray()
	{
		if (FLUSH)
			FlushAll();
	}

	void FlushAll()
	{
		GeTempDynArray<TYPE>::FlushAll();
		m_sorted = false;
	}

	void FlushThis()
	{
		GeTempDynArray<TYPE>::FlushThis();
		m_sorted = false;
	}

	Bool Append(TYPE* vi)
	{
		m_sorted = false;
		return GeTempDynArray<TYPE>::Append(vi);
	}

	Bool Remove(TYPE* vi)
	{
		return GeTempDynArray<TYPE>::Remove(vi);
	}

	Bool Remove(Int32 i)
	{
		return GeTempDynArray<TYPE>::Remove(i);
	}

	TYPE* GetIndex(Int32 i)
	{
		return GeTempDynArray<TYPE>::GetIndex(i);
	}

	const TYPE* GetIndex(Int32 i) const
	{
		return GeTempDynArray<TYPE>::GetIndex(i);
	}

	Int32 GetIndex(const TYPE* o) const
	{
		return GeTempDynArray<TYPE>::Find(o);
	}

	TYPE* GetFirst()
	{
		return GeTempDynArray<TYPE>::GetFirst();
	}

	TYPE* GetLast()
	{
		return GeTempDynArray<TYPE>::GetLast();
	}

	TYPE* operator [] (Int32 i)
	{
		return GeTempDynArray<TYPE>::GetIndex(i);
	}

	const TYPE* operator [] (Int32 i) const
	{
		return GeTempDynArray<TYPE>::GetIndex(i);
	}

	Int32 GetCount() const
	{
		return GeTempDynArray<TYPE>::GetCount();
	}

	// TODO: findidx must become Int
	TYPE* Find(SEARCH data, TYPE*** ptr = nullptr, Int32* findidx = nullptr)
	{
		CheckSort();

		GeToolTempDynSortArraySearch<GeTempDynSortSearchArray<TYPE, SEARCH, FLUSH>, TYPE, SEARCH> sort(this, &GeTempDynSortSearchArray<TYPE, SEARCH, FLUSH>::SearchFunc);
		TYPE** res = (TYPE**)sort.Search(&data, GeTempDynArray<TYPE>::GetRoot(), GeTempDynArray<TYPE>::GetCount(), sizeof(TYPE*));
		if (res)
		{
			if (ptr)
				*ptr = res;
			if (findidx)
				*findidx = Int32(res - (TYPE**)GeTempDynArray<TYPE>::GetRoot());
			return *res;
		}
		if (ptr)
			*ptr = nullptr;
		return nullptr;
	}

	// TODO: findidx must become Int
	TYPE* InsertValueNew(SEARCH data, Bool& newelement, Int32* findidx = nullptr)
	{
		CheckSort();

		GeToolTempDynSortArraySearch<GeTempDynSortSearchArray<TYPE, SEARCH, FLUSH>, TYPE, SEARCH> sort(this, &GeTempDynSortSearchArray<TYPE, SEARCH, FLUSH>::SearchFunc);
		Int32	 insertidx = NOTOK;
		TYPE** res = (TYPE**)sort.SearchOrInsert(&data, GeTempDynArray<TYPE>::GetRoot(), GeTempDynArray<TYPE>::GetCount(), sizeof(TYPE*), insertidx);
		if (res)
		{
			if (findidx)
				*findidx = Int32(res - (TYPE**)GeTempDynArray<TYPE>::GetRoot());
			return *res;
		}
		TYPE* e = NewObj(TYPE);
		if (!e)
			return nullptr;
		if (insertidx == NOTOK)
			insertidx = 0;
		if (!this->Insert(e, insertidx))
		{
			DeleteObj(e);
			return e;
		}
		if (findidx)
			*findidx = insertidx;
		newelement = true;
		return e;
	}

	Bool CopyTo(GeTempDynSortSearchArray* dest, Bool copyelements) const
	{
		if (copyelements)
			return CloneTo(dest);

		dest->m_sorted = m_sorted;
		return GeTempDynArray<TYPE>::CopyTo(dest);
	}

	Bool CloneTo(GeTempDynSortSearchArray* dest) const
	{
		dest->m_sorted = m_sorted;
		Int32 i, cnt = GetCount();
		for (i = 0; i < cnt; i++)
		{
			TYPE* e = dest->AppendNew();
			if (!e)
				return false;
			*e = *GetIndex(i);
		}
		return true;
	}

	const TYPE* Find(SEARCH data, const TYPE*** ptr = nullptr) const
	{
		return (const TYPE*)((GeTempDynSortSearchArray*)this)->Find(data, (TYPE***)ptr);
	}

	TYPE* AppendNew()
	{
		m_sorted = false;
		return GeTempDynArray<TYPE>::AppendNew();
	}

	void Sort()
	{
		CheckSort();
	}

	// important! call this if elements has been changed and array need to be resorted!
	void SortChanged()
	{
		m_sorted = false;
	}

};

/////////////////////////////////

#define CRC32_NEGL 0xffffffff

class Crc32
{
protected:
	static const UInt32 crc32tab[256];

protected:
	UInt32 _crc;

public:
	Crc32()
	{
		Reset();
	}

	void Reset()
	{
		_crc = CRC32_NEGL;
	}

	void Update(const void *s, Int n)
	{
		UInt32 c = _crc;
		UChar* d = (UChar*)s;
		while (n--)
			c = crc32tab[UChar(c ^ *d++)] ^ (c >> 8);
		_crc = c;
	}

	void UpdateByte(UChar b)
	{
		_crc = crc32tab[UChar(_crc ^ b)] ^ (_crc >> 8);
	}

	inline void UpdateUInt32(UInt32 u)
	{
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateUInt64(UInt64 u)
	{
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8); u >>= 8;
		c = crc32tab[UChar(c ^ (u & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateInt32(Int32 u)
	{
		UInt32 n = (UInt32)u;
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateBool(Bool b)
	{
		UInt32 n = (UInt32)b;
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateFloat32(Float32 r)
	{
		UInt32 n = *((UInt32*)&r);
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateFloat64(Float64 r)
	{
		UInt64 n = *((UInt64*)&r);
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateFloat(Float r)
	{
		UInt64 n = *((UInt64*)&r);
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdatePointer(const void* p)
	{
		UInt64 n = (UInt64)p;
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	inline void UpdateInt(Int p)
	{
		UInt64 n = (UInt64)p;
		UInt32 c = _crc;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8); n >>= 8;
		c = crc32tab[UChar(c ^ (n & 0xff))] ^ (c >> 8);
		_crc = c;
	}

	UInt32 GetCrc()
	{
		return _crc ^ CRC32_NEGL;
	}
};

#ifdef MAXON_TARGET_WINDOWS
	#define CRC64_NEGL 0xffffffffffffffff
#else
	#define CRC64_NEGL 0xffffffffffffffffLLU
#endif

class Crc64
{
protected:
	static const UInt64 crc64tab[256];

protected:
	UInt64 crc;

public:
	Crc64();
	virtual void Reset();
	virtual void Update(const void* input, Int length);
	virtual void UpdateByte(UChar input);
	virtual UInt64 GetCrc();
};

////////////////////////////////

Bool CDynArrayWriteHeader(HyperFile* pFile, Int32 lElements, Int32 lLevel);
Bool CDynArrayReadHeader(HyperFile* pFile, Int32& lElements, Int32& lLevel);

template <class ArrayElement> class CDynArray
{
public:
	CDynArray();
	CDynArray(ArrayElement* pDefaultElement);
	virtual ~CDynArray();

	Bool Alloc(Int32 lExtraElements, Int32 lNewElementCount = -1, Bool bClear = true);
	Bool SetMinSize(Int32 lElements);
	Bool SetMinSizeNoCopy(Int32 lElements, Bool bClear = true);
	Bool Init(const ArrayElement* pArray, Int32 lCount);	// copies lCount elements of pArray into this
	Bool Init(const CDynArray<ArrayElement>& ar);					// copies ar into this
	Bool InitNoCopy(ArrayElement* pArray, Int32 lCount);
	ArrayElement* GetNext(Int32 lNewElementCount = -1, Bool bClear = true);
	ArrayElement* InsertAtA(Int32 l = 0, Int32 lNewElementCount = -1);
	ArrayElement* InsertMoreAt(Int32 l = 0, Int32 lInsert = 1, Int32 lNewElementCount = -1);
	ArrayElement* InsertMoreAt(Int32 lPos, Int32 lCount, const ArrayElement* pElements);
	ArrayElement* GetElement(Int32 lIndex) const;
	const ArrayElement* GetArray() const
	{
		return m_pElements;
	}
	ArrayElement* GetArray()
	{
		return m_pElements;
	}
	Bool Append(const ArrayElement& e, Int32 lGetNextAdd = 20, Bool bClear = true);
	Bool Append(const ArrayElement* e, Int32 lElements);	// appends lElements of ArrayElement to this
	Bool InsertAtB(const ArrayElement& e, Int32 l, Int32 lNewElementCount = -1);
	Int32 GetElementCount() const
	{
		return m_lElements;
	}
	Bool IsEmpty() const
	{
		return m_lElements == 0;
	}
	Int32 GetMaxElementCount() const
	{
		return m_lMaxElements;
	}
	void ResetCounter(Bool bErase = false);
	void SetCounter(Int32 l)
	{
		m_lElements = l;
	}
	Bool RemoveAt(Int32 l, Bool bFillZeros);
	Bool RemoveAt(Int32 lPos, Int32 lCount, Bool bFillZeros);
	void SetData(void* pData)
	{
		m_pData = pData;
	}
	void* GetData() const
	{
		return m_pData;
	}
	virtual void Free();
	virtual void ExchangeContents(CDynArray<ArrayElement>* pDest);
	void Release();
	ArrayElement& operator [] (Int32 l) const;
	Bool CopyTo(CDynArray<ArrayElement>* pDest) const;
	void Flip();
	Bool IsEqual(const CDynArray<ArrayElement>* pCmp) const;
	Bool Read(HyperFile* pFile, Bool bFlush = true);
	Bool Write(HyperFile* pFile, Int32 lLevel) const;
	virtual Bool ReadElements(HyperFile* pFile, ArrayElement* pElement, Int32 lCount, Int32 lLevel);
	virtual Bool WriteElements(HyperFile* pFile, const ArrayElement* pElement, Int32 lCount) const;

	ArrayElement* m_pElements;
	Int32					m_lElements, m_lMaxElements;

	ArrayElement* m_pDefaultElement;
	void*					m_pData;
};

template <class ArrayElement> CDynArray<ArrayElement>::CDynArray()
{
	m_pElements = nullptr;
	m_lElements = 0;
	m_lMaxElements = 0;
	m_pDefaultElement = nullptr;
	m_pData = nullptr;
}

template <class ArrayElement> CDynArray<ArrayElement>::CDynArray(ArrayElement* pDefaultElement)
{
	m_pElements = nullptr;
	m_lElements = 0;
	m_lMaxElements = 0;
	m_pDefaultElement = pDefaultElement;
	m_pData = nullptr;
}

template <class ArrayElement> CDynArray<ArrayElement>::~CDynArray()
{
	Free();
}

template <class ArrayElement> void CDynArray<ArrayElement>::Free()
{
	m_lElements = 0;
	m_lMaxElements = 0;
	m_pDefaultElement = nullptr;
	DeleteMem(m_pElements);
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::Alloc(Int32 lExtraElements, Int32 lNewElementCount, Bool bClear)
{
	if (m_lMaxElements >= lExtraElements + m_lElements)
		return true;

	if (lNewElementCount < lExtraElements)
		lNewElementCount = lExtraElements;

	ArrayElement* pTmp;
	if (bClear)
	{
		pTmp = NewMemClear(ArrayElement, (lNewElementCount + m_lElements));
	}
	else
	{
		pTmp = NewMem(ArrayElement, lNewElementCount + m_lElements);
	}

	if (!pTmp)
		return false;
	CopyMemType(ArrayElement, m_pElements, pTmp, m_lMaxElements);

	DeleteMem(m_pElements);

	m_pElements = pTmp;
	m_lMaxElements = lNewElementCount + m_lElements;
	Int32 a;
	if (m_pDefaultElement)
	{
		for (a = m_lElements; a < m_lMaxElements; a++)
			m_pElements[a] = *m_pDefaultElement;
	}

	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::Init(const ArrayElement* pArray, Int32 lCount)
{
	Free();
	if (!SetMinSizeNoCopy(lCount, false))
		return false;
	CopyMemType(ArrayElement, pArray, m_pElements, lCount);
	m_lElements = lCount;
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::Init(const CDynArray<ArrayElement>& ar)
{
	Free();
	if (!SetMinSizeNoCopy(ar.m_lElements, false))
		return false;
	CopyMemType(ArrayElement, ar.m_pElements, m_pElements, ar.m_lElements);
	m_lElements = ar.m_lElements;
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::InitNoCopy(ArrayElement* pArray, Int32 lCount)
{
	Free();
	m_pElements = pArray;
	m_lElements = m_lMaxElements = lCount;
	return true;
}


template <class ArrayElement> Bool CDynArray<ArrayElement>::SetMinSize(Int32 lElements)
{
	if (m_lMaxElements >= lElements)
		return true;
	Int32 lBack = m_lElements;
	m_lElements = m_lMaxElements;
	if (!Alloc(lElements - m_lMaxElements + 1))
		return false;
	m_lElements = lBack;
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::SetMinSizeNoCopy(Int32 lElements, Bool bClear)
{
	if (!lElements)
		return true;
	if (m_lMaxElements >= lElements)
	{
		if (bClear)
			ClearMemType(ArrayElement, m_pElements, lElements);
		m_lElements = 0;
		return true;
	}
	m_lElements = m_lMaxElements = 0;

	DeleteMem(m_pElements);
	m_pElements = nullptr;

	ArrayElement* pTmp;
	if (bClear)
	{
		pTmp = NewMemClear(ArrayElement, lElements);
	}
	else
	{
		pTmp = NewMem(ArrayElement, lElements);
	}

	if (!pTmp)
		return false;
	m_pElements = pTmp;
	m_lMaxElements = lElements;

#ifdef MAXON_TARGET_DEBUG
	if (!bClear)
	{
		Int32	 i, s = m_lMaxElements * sizeof(ArrayElement);
		UChar* c = (UChar*)m_pElements;
		for (i = 0; i < s; i++)
			*c++ = 0xcd;
	}
#endif

	return true;
}

template <class ArrayElement> ArrayElement* CDynArray<ArrayElement>::GetElement(Int32 lIndex) const
{
	DebugAssert(!(lIndex < 0 || lIndex >= m_lElements));
	if (lIndex < 0 || lIndex >= m_lElements)
		return nullptr;
	return &(m_pElements[lIndex]);
}

template <class ArrayElement> void CDynArray<ArrayElement>::ResetCounter(Bool bErase)
{
	if (bErase)
		ClearMemType(ArrayElement, m_pElements, m_lElements);

	m_lElements = 0;
}


template <class ArrayElement> ArrayElement* CDynArray<ArrayElement>::GetNext(Int32 lNewElementCount, Bool bClear)
{
	if (m_lElements >= m_lMaxElements)
	{
		if (lNewElementCount <= 0)
		{
			// negative count means add -lNewElementCount% of the original size
			Int32 lAdd = LMax(20, (Int32)((-(Int64)lNewElementCount * (Int64)m_lElements) / 100));
			if (!Alloc(lAdd, -1, bClear))
			{
				CriticalStop();
				return nullptr;
			}
		}
		else
		{
			if (!Alloc(lNewElementCount, -1, bClear))
			{
				CriticalStop();
				return nullptr;
			}
		}
	}

	ArrayElement* e = &(m_pElements[m_lElements]);
	m_lElements++;
	return e;
}

template <class ArrayElement> ArrayElement* CDynArray<ArrayElement>::InsertAtA(Int32 l, Int32 lNewElementCount)
{
	return InsertMoreAt(l, 1, lNewElementCount);
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::InsertAtB(const ArrayElement& e, Int32 l, Int32 lNewElementCount)
{
	ArrayElement* pNew = InsertAtA(l, lNewElementCount);
	if (!pNew)
		return false;
	*pNew = e;
	return true;
}


template <class ArrayElement> ArrayElement* CDynArray<ArrayElement>::InsertMoreAt(Int32 lPos, Int32 lInsert, Int32 lNewElementCount)
{
	ArrayElement* e;
	if (lPos < 0)	// append
		lPos = m_lElements;

	// if we have enough space, just move the elements
	if (m_lElements + lInsert < m_lMaxElements)
	{
		memmove(&(m_pElements[lPos + lInsert]), &(m_pElements[lPos]), (m_lElements - lPos) * sizeof(ArrayElement));
	}
	else
	{
		Int32 lAdd = LMax(lInsert, lNewElementCount);
		lAdd = LMax(lAdd, 20);

		ArrayElement* pNew;
		pNew = NewMemClear(ArrayElement, (m_lMaxElements + lAdd));

		if (!pNew)
			return nullptr;
		CopyMemType(ArrayElement, m_pElements, pNew, lPos);
		CopyMemType(ArrayElement, &(m_pElements[lPos]), &(pNew[lPos + lInsert]), (m_lElements - lPos));
		if (m_pDefaultElement)
		{
			Int32 a;
			for (a = 0; a < lInsert; a++)
				pNew[a + lPos] = *m_pDefaultElement;
		}
		DeleteMem(m_pElements);

		m_pElements = pNew;
		m_lMaxElements += lAdd;
	}
	e = &(m_pElements[lPos]);

	m_lElements += lInsert;
	return e;
}

template <class ArrayElement> ArrayElement* CDynArray<ArrayElement>::InsertMoreAt(Int32 lPos, Int32 lCount, const ArrayElement* pElements)
{
	ArrayElement* pNew = InsertMoreAt(lPos, lCount, -1);
	if (!pNew)
		return false;
	CopyMemType(ArrayElement, pElements, pNew, lCount);
	return pNew;
}


template <class ArrayElement> Bool CDynArray<ArrayElement>::RemoveAt(Int32 l, Bool bFillZeros)
{
	if (l < 0 || l >= m_lElements)
		return false;
	memmove(&(m_pElements[l]), &(m_pElements[l + 1]), (m_lElements - l - 1) * sizeof(ArrayElement));
	if (bFillZeros)
		ClearMemType(ArrayElement, &(m_pElements[m_lElements - 1]), 1);
	m_lElements--;
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::RemoveAt(Int32 lPos, Int32 lCount, Bool bFillZeros)
{
	if (lPos < 0 || lPos >= m_lElements)
		return false;
	if (lPos + lCount >= m_lElements)
		lCount = m_lElements - lPos;
	memmove(&(m_pElements[lPos]), &(m_pElements[lPos + lCount]), (m_lElements - lPos - lCount) * sizeof(ArrayElement));
	if (bFillZeros)
		ClearMemType(ArrayElement, &(m_pElements[m_lElements - lCount]), lCount);
	m_lElements -= lCount;
	return true;
}


template <class ArrayElement> void CDynArray<ArrayElement>::ExchangeContents(CDynArray<ArrayElement>* pDest)
{
	Int32					l;
	ArrayElement* e;
	void*					v;

	e = m_pElements;
	m_pElements = pDest->m_pElements;
	pDest->m_pElements = e;
	e = m_pDefaultElement;
	m_pDefaultElement = pDest->m_pDefaultElement;
	pDest->m_pDefaultElement = e;
	l = m_lElements;
	m_lElements = pDest->m_lElements;
	pDest->m_lElements = l;
	l = m_lMaxElements;
	m_lMaxElements = pDest->m_lMaxElements;
	pDest->m_lMaxElements = l;
	v = m_pData;
	m_pData = pDest->m_pData;
	pDest->m_pData = v;
}

template <class ArrayElement> void CDynArray<ArrayElement>::Release()
{
	m_pElements = nullptr;
	m_lElements = 0;
	m_lMaxElements = 0;
}


template <class ArrayElement> Bool CDynArray<ArrayElement>::Append(const ArrayElement& e, Int32 lGetNextAdd, Bool bClear)
{
	if (m_lElements < m_lMaxElements)
	{
		m_pElements[m_lElements++] = e;
	}
	else
	{
		ArrayElement* pNew = GetNext(lGetNextAdd, bClear);
		if (!pNew)
			return false;
		*pNew = e;
	}
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::Append(const ArrayElement* e, Int32 lElements)
{
	if (!Alloc(lElements))
		return false;
	CopyMemType(ArrayElement, e, &(m_pElements[m_lElements]), lElements);
	m_lElements += lElements;
	return true;
}


template <class ArrayElement> ArrayElement& CDynArray<ArrayElement>::operator [] (Int32 l) const
{
	DebugAssert(l >= 0 && l < m_lElements);
	return m_pElements[l];
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::CopyTo(CDynArray<ArrayElement>* pDest) const
{
	pDest->Free();
	if (!pDest->SetMinSizeNoCopy(m_lElements))
		return false;
	pDest->SetCounter(m_lElements);
	CopyMemType(ArrayElement, m_pElements, pDest->m_pElements, m_lElements);
	return true;
}

template <class ArrayElement> void CDynArray<ArrayElement>::Flip()
{
	ArrayElement t;
	Int32				 a, max;
	max = m_lElements / 2;
	for (a = 0; a < max; a++)
	{
		t = m_pElements[a];
		m_pElements[a] = m_pElements[m_lElements - 1 - a];
		m_pElements[m_lElements - 1 - a] = t;
	}
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::IsEqual(const CDynArray<ArrayElement>* pCmp) const
{
	if (!pCmp)
		return false;
	if (m_lElements != pCmp->m_lElements)
		return false;
	Int32 a;
	for (a = 0; a < m_lElements; a++)
	{
		if (m_pElements[a] != pCmp->m_pElements[a])
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::Read(HyperFile* pFile, Bool bFlush)
{
	Int32 lElements, lLevel;
	if (bFlush)
		Free();
	if (!CDynArrayReadHeader(pFile, lElements, lLevel))
		return false;
	if (bFlush)
	{
		if (!SetMinSizeNoCopy(lElements))
			return false;
		m_lElements = 0;
	}
	else
	{
		if (!SetMinSizeNoCopy(m_lElements + lElements))
			return false;
	}
	if (!ReadElements(pFile, &(m_pElements[m_lElements]), lElements, lLevel))
		return false;
	m_lElements += lElements;
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::Write(HyperFile* pFile, Int32 lLevel) const
{
	Int32 lElements = GetElementCount();
	if (!CDynArrayWriteHeader(pFile, lElements, lLevel))
		return false;
	if (!WriteElements(pFile, m_pElements, lElements))
		return false;
	return true;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::ReadElements(HyperFile* pFile, ArrayElement* pElement, Int32 lCount, Int32 lLevel)
{
	// override
	CriticalStop();
	return false;
}

template <class ArrayElement> Bool CDynArray<ArrayElement>::WriteElements(HyperFile* pFile, const ArrayElement* pElement, Int32 lCount) const
{
	// override
	CriticalStop();
	return false;
}

template <class ArrayElement> class CDynObjectArray
{
public:
	typedef ArrayElement* ArrayElementPtr;

	CDynObjectArray();
	virtual ~CDynObjectArray();

	ArrayElement* GetNextObject();
	ArrayElement* InsertAt(Int32 l);

	ArrayElement* GetNextObjectType(Int32 lID);
	ArrayElement* InsertAtType(Int32 lID, Int32 l);

	Int32 GetObjectIndex(const ArrayElement* pObj) const;
	ArrayElement* GetObject(Int32 l) const;
	Int32 GetElementCount() const;
	Bool RemoveAt(Int32 l);
	Bool RemoveObjectPointer(const ArrayElement* pObj);
	const CDynArray<ArrayElementPtr>* GetArray() const
	{
		return &m_Objects;
	}
	CDynArray<ArrayElementPtr>* GetArray()
	{
		return &m_Objects;
	}
	ArrayElement* operator [] (Int32 l) const;
	Bool IsEmpty() const
	{
		return m_Objects.GetElementCount() == 0;
	}
	Bool IsEqual(const CDynObjectArray<ArrayElement>* pArray) const;

	Bool RemovePointer(Int32 lIndex);											// removes the object, doesn't free memory
	Bool InsertObject(ArrayElement* pObj, Int32 lIndex);	// inserts an existing object into the array at position lIndex
	Bool Init(const ArrayElement* const* ppObjects, Int32 lCount, AliasTrans* pTrans = nullptr, Bool bAppend = false);
	Bool CopyTo(CDynObjectArray<ArrayElement>* pDest, AliasTrans* pTrans, Bool bAppend = false) const;
	Bool Alloc(Int32 lElementCount);	// allocates memory to hold lElementCount elements more than currently, increases the counter

	virtual void Free();
	Bool Read(HyperFile* pFile, Bool bFlush = true);
	Bool Write(HyperFile* pFile, Int32 lLevel) const;
	Bool ReadCreateObject(HyperFile* pFile, Bool bFlush = true);

protected:
	virtual ArrayElement* AllocObject() const = 0;
	virtual void FreeObject(ArrayElement*& pObj) const = 0;
	virtual ArrayElement* AllocObjectType(Int32 lType) const;
	virtual ArrayElement* GetClone(const ArrayElement* pObj, AliasTrans* pTrans) const;
	CDynArray<ArrayElementPtr> m_Objects;

	virtual Bool ReadElement(BaseFile* pFile, ArrayElement* pElement, Int32 lLevel);
	virtual Bool WriteElement(BaseFile* pFile, const ArrayElement* pElement) const;
	virtual Bool ReadElementHF(HyperFile* pFile, ArrayElement* pElement, Int32 lLevel);
	virtual Bool WriteElementHF(HyperFile* pFile, const ArrayElement* pElement) const;
	virtual Bool ReadElementCreate(HyperFile* pFile, ArrayElement*& pElement, Int32 lLevel);
	virtual Bool TestEqual(const ArrayElement* pObjA, const ArrayElement* pObjB) const;
};

template <class ArrayElement> CDynObjectArray<ArrayElement>::CDynObjectArray()
{
}

template <class ArrayElement> CDynObjectArray<ArrayElement>::~CDynObjectArray()
{
	DebugAssert(m_Objects.GetElementCount() == 0);
	// don't call Free here, it will cause a pure virtual function call
	// call Free in the destructor of your derived class.
}


template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::GetNextObject()
{
	ArrayElement* pNew = AllocObject();
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.GetNext();
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}

	DebugAssert(!*ppNew);
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::GetNextObjectType(Int32 lID)
{
	ArrayElement* pNew = AllocObjectType(lID);
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.GetNext();
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}

	DebugAssert(!*ppNew);
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::AllocObjectType(Int32 lID) const
{
	// override!
	CriticalStop();
	return nullptr;
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::GetClone(const ArrayElement* pObj, AliasTrans* pTrans) const
{
	// override!
	CriticalStop();
	return nullptr;
}

template <class ArrayElement> Int32 CDynObjectArray<ArrayElement>::GetObjectIndex(const ArrayElement* pObj) const
{
	Int32 lElements = m_Objects.GetElementCount();
	ArrayElement* const* ppElements = m_Objects.GetArray();

	if (lElements == 0 || ppElements == 0)
		return -1;

	Int32 l = 0;
	for (l = 0; l < lElements; l++)
	{
		if (ppElements[l] == pObj)
			return l;
	}
	return -1;
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::GetObject(Int32 l) const
{
	if (l < 0 || l >= m_Objects.GetElementCount())
		return nullptr;

	ArrayElement** ppElement = m_Objects.GetElement(l);
	if (!ppElement)
		return nullptr;

	return *ppElement;
}

template <class ArrayElement> void CDynObjectArray<ArrayElement>::Free()
{
	Int32 l, lCount = m_Objects.GetElementCount();
	ArrayElement** ppElements = m_Objects.GetArray();
	for (l = 0; l < lCount; l++)
	{
		FreeObject(ppElements[l]);
	}
	m_Objects.Free();
}

template <class ArrayElement> Int32 CDynObjectArray<ArrayElement>::GetElementCount() const
{
	return m_Objects.GetElementCount();
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::RemoveAt(Int32 l)
{
	if (l < 0 || l >= m_Objects.GetElementCount())
		return false;
	ArrayElement** ppElement = m_Objects.GetElement(l);
	if (!ppElement)
		return false;
	if (!*ppElement)
		return false;
	ArrayElement* a = *ppElement;
	FreeObject(a);
	m_Objects.RemoveAt(l, true);
	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::RemoveObjectPointer(const ArrayElement* pObj)
{
	Int32 lIndex = GetObjectIndex(pObj);
	if (lIndex < 0)
		return false;
	return RemoveAt(lIndex);
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::InsertAt(Int32 l)
{
	ArrayElement* pNew = AllocObject();
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.InsertAtA(l);
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::InsertAtType(Int32 lType, Int32 l)
{
	ArrayElement* pNew = AllocObjectType(lType);
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.InsertAtA(l);
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynObjectArray<ArrayElement>::operator [] (Int32 l) const
{
	return m_Objects[l];
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::Read(HyperFile* pFile, Bool bFlush)
{
	Int32 lElements, lLevel;
	if (bFlush)
		m_Objects.Free();
	if (!CDynArrayReadHeader(pFile, lElements, lLevel))
		return false;
	if (bFlush)
	{
		if (!m_Objects.SetMinSizeNoCopy(lElements))
			return false;
	}

	for (Int32 a = 0; a < lElements; a++)
	{
		ArrayElement* pElement = AllocObject();
		if (!pElement)
			return false;
		if (!ReadElement(pFile, pElement, lLevel))
		{
			FreeObject(pElement);
			return false;
		}
		if (!m_Objects.Append(pElement))
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::ReadCreateObject(HyperFile* pFile, Bool bFlush)
{
	Int32 lElements, lLevel;
	if (bFlush)
		m_Objects.Free();
	if (!CDynArrayReadHeader(pFile, lElements, lLevel))
		return false;
	if (bFlush)
	{
		if (!m_Objects.SetMinSizeNoCopy(lElements))
			return false;
	}

	for (Int32 a = 0; a < lElements; a++)
	{
		ArrayElement* pElement = nullptr;
		if (!ReadElementCreate(pFile, pElement, lLevel))
		{
			FreeObject(pElement);
			return false;
		}
		if (!m_Objects.Append(pElement))
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::Write(HyperFile* pFile, Int32 lLevel) const
{
	Int32 lElements = m_Objects.GetElementCount();
	if (!CDynArrayWriteHeader(pFile, lElements, lLevel))
		return false;
	for (Int32 a = 0; a < lElements; a++)
	{
		if (!WriteElementHF(pFile, m_Objects[a]))
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::ReadElement(BaseFile* pFile, ArrayElement* pElement, Int32 lLevel)
{
	// override!
	CriticalStop();
	return false;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::WriteElement(BaseFile* pFile, const ArrayElement* pElement) const
{
	// override!
	CriticalStop();
	return false;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::ReadElementHF(HyperFile* pFile, ArrayElement* pElement, Int32 lLevel)
{
	// override!
	CriticalStop();
	return false;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::WriteElementHF(HyperFile* pFile, const ArrayElement* pElement) const
{
	// override!
	CriticalStop();
	return false;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::ReadElementCreate(HyperFile* pFile, ArrayElement*& pElement, Int32 lLevel)
{
	// override!
	CriticalStop();
	return false;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::TestEqual(const ArrayElement* pObjA, const ArrayElement* pObjB) const
{
	// override!
	CriticalStop();
	return false;
}


template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::RemovePointer(Int32 lIndex)
{
	return m_Objects.RemoveAt(lIndex, true);
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::InsertObject(ArrayElement* pObj, Int32 lIndex)
{
	ArrayElement** ppNew = m_Objects.InsertAtA(lIndex);
	if (!ppNew)
		return false;
	*ppNew = pObj;
	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::Init(const ArrayElement* const* ppObjects, Int32 lCount, AliasTrans* pTrans, Bool bAppend)
{
	if (!bAppend)
		Free();
	if (!ppObjects || !lCount)
		return true;
	m_Objects.SetMinSizeNoCopy(lCount);
	Int32 a;
	for (a = 0; a < lCount; a++)
	{
		ArrayElement* pClone = GetClone(ppObjects[a], pTrans);
		if (!pClone)
			return false;
		if (!m_Objects.Append(pClone))
		{
			FreeObject(pClone);
			return false;
		}
	}
	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::CopyTo(CDynObjectArray<ArrayElement>* pDest, AliasTrans* pTrans, Bool bAppend) const
{
	if (!bAppend)
		pDest->Free();
	Int32 a, lElements;
	lElements = m_Objects.GetElementCount();
	const ArrayElement* const* ppElements = m_Objects.GetArray();
	pDest->m_Objects.SetMinSizeNoCopy(lElements);
	for (a = 0; a < lElements; a++)
	{
		if (!ppElements[a])
			continue;
		ArrayElement* pClone = GetClone(ppElements[a], pTrans);
		if (!pClone)
			return false;
		if (!pDest->m_Objects.Append(pClone))
		{
			FreeObject(pClone);
			return false;
		}
	}

	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::IsEqual(const CDynObjectArray<ArrayElement>* pArray) const
{
	if (pArray->GetElementCount() != GetElementCount())
		return false;

	const ArrayElement* const* ppDest = pArray->GetArray()->GetArray(), *const* ppThis = GetArray()->GetArray();
	Int32 lElements = GetElementCount();
	for (Int32 a = 0; a < lElements; a++)
	{
		if (!ppDest[a] || !ppThis[a])
			return false;
		if (!TestEqual(ppThis[a], ppDest[a]))
			return false;
	}

	return true;
}

template <class ArrayElement> Bool CDynObjectArray<ArrayElement>::Alloc(Int32 lElementCount)	// allocates memory to hold lElementCount elements more than currently, increases the counter
{
	Int32 lOldCounter = m_Objects.GetElementCount();
	if (!m_Objects.Alloc(lElementCount))
		return false;

	Int32 lNewCounter = lOldCounter + lElementCount;
	Int32 i;
	ArrayElement** ppDest = m_Objects.GetArray();
	for (i = lOldCounter; i < lNewCounter; i++)
	{
		ppDest[i] = AllocObject();
		if (!ppDest[i])
			return false;
	}

	m_Objects.SetCounter(lNewCounter);

	return true;
}

/*****************************************************************************\
*   CDynamicObjectArray                                                       *
\*****************************************************************************/

template <class ArrayElement> class CDynamicObjectArray
{
public:
	typedef ArrayElement* ArrayElementPtr;

	CDynamicObjectArray();
	virtual ~CDynamicObjectArray();

	ArrayElement* GetNextObject();
	ArrayElement* InsertAt(Int32 l);

	ArrayElement* GetNextObjectType(Int32 lID);
	ArrayElement* InsertAtType(Int32 lID, Int32 l);

	Int32 GetObjectIndex(const ArrayElement* pObj) const;
	ArrayElement* GetObjectAt(Int32 l) const;
	Int32 GetElementCount() const;
	Bool RemoveAt(Int32 l);
	Bool RemoveObjectPointer(const ArrayElement* pObj);
	const CDynArray<ArrayElementPtr>* GetArray() const
	{
		return &m_Objects;
	}
	CDynArray<ArrayElementPtr>* GetArray()
	{
		return &m_Objects;
	}
	ArrayElement* operator [] (Int32 l) const;
	Bool IsEmpty() const
	{
		return m_Objects.GetElementCount() == 0;
	}
	Bool IsEqual(const CDynamicObjectArray<ArrayElement>* pArray) const;

	Bool RemovePointer(Int32 lIndex);											// removes the object, doesn't free memory
	Bool InsertObject(ArrayElement* pObj, Int32 lIndex);	// inserts an existing object into the array at position lIndex
	Bool Init(const ArrayElement* const* ppObjects, Int32 lCount, AliasTrans* pTrans = nullptr, Bool bAppend = false);
	virtual Bool CopyTo(CDynamicObjectArray<ArrayElement>* pDest, AliasTrans* pTrans, Bool bAppend = false) const;
	Bool Alloc(Int32 lElementCount);	// allocates memory to hold lElementCount elements more than currently, increases the counter

	virtual void Free();
	Bool Read(HyperFile* pFile, Bool bFlush = true);
	Bool Write(HyperFile* pFile, Int32 lLevel) const;
	Bool ReadCreateObject(HyperFile* pFile, Bool bFlush = true);

protected:
	virtual ArrayElement* AllocObject() const = 0;
	virtual void FreeObject(ArrayElement*& pObj) const = 0;
	virtual ArrayElement* AllocObjectType(Int32 lType) const;
	virtual ArrayElement* GetClone(const ArrayElement* pObj, AliasTrans* pTrans) const;
	CDynArray<ArrayElementPtr> m_Objects;

	virtual Bool ReadElement(BaseFile* pFile, ArrayElement* pElement, Int32 lLevel);
	virtual Bool WriteElement(BaseFile* pFile, const ArrayElement* pElement) const;
	virtual Bool ReadElementHF(HyperFile* pFile, ArrayElement* pElement, Int32 lLevel);
	virtual Bool WriteElementHF(HyperFile* pFile, const ArrayElement* pElement) const;
	virtual Bool ReadElementCreate(HyperFile* pFile, ArrayElement*& pElement, Int32 lLevel);
	virtual Bool TestEqual(const ArrayElement* pObjA, const ArrayElement* pObjB) const;
};

template <class ArrayElement> CDynamicObjectArray<ArrayElement>::CDynamicObjectArray()
{
}

template <class ArrayElement> CDynamicObjectArray<ArrayElement>::~CDynamicObjectArray()
{
	DebugAssert(m_Objects.GetElementCount() == 0);
	// don't call Free here, it will cause a pure virtual function call
	// call Free in the destructor of your derived class.
}


template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::GetNextObject()
{
	ArrayElement* pNew = AllocObject();
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.GetNext();
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}

	DebugAssert(!*ppNew);
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::GetNextObjectType(Int32 lID)
{
	ArrayElement* pNew = AllocObjectType(lID);
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.GetNext();
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}

	DebugAssert(!*ppNew);
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::AllocObjectType(Int32 lID) const
{
	// override!
	DebugAssert(false);
	return nullptr;
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::GetClone(const ArrayElement* pObj, AliasTrans* pTrans) const
{
	// override!
	DebugAssert(false);
	return nullptr;
}

template <class ArrayElement> Int32 CDynamicObjectArray<ArrayElement>::GetObjectIndex(const ArrayElement* pObj) const
{
	Int32 lElements = m_Objects.GetElementCount();
	ArrayElement* const* ppElements = m_Objects.GetArray();

	if (lElements == 0 || ppElements == 0)
		return -1;

	Int32 l = 0;
	for (l = 0; l < lElements; l++)
	{
		if (ppElements[l] == pObj)
			return l;
	}
	return -1;
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::GetObjectAt(Int32 l) const
{
	if (l < 0 || l >= m_Objects.GetElementCount())
		return nullptr;

	ArrayElement** ppElement = m_Objects.GetElement(l);
	if (!ppElement)
		return nullptr;

	return *ppElement;
}

template <class ArrayElement> void CDynamicObjectArray<ArrayElement>::Free()
{
	Int32 l, lCount = m_Objects.GetElementCount();
	ArrayElement** ppElements = m_Objects.GetArray();
	for (l = 0; l < lCount; l++)
	{
		FreeObject(ppElements[l]);
	}
	m_Objects.Free();
}

template <class ArrayElement> Int32 CDynamicObjectArray<ArrayElement>::GetElementCount() const
{
	return m_Objects.GetElementCount();
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::RemoveAt(Int32 l)
{
	if (l < 0 || l >= m_Objects.GetElementCount())
		return false;
	ArrayElement** ppElement = m_Objects.GetElement(l);
	if (!ppElement)
		return false;
	if (!*ppElement)
		return false;
	ArrayElement* a = *ppElement;
	FreeObject(a);
	m_Objects.RemoveAt(l, true);
	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::RemoveObjectPointer(const ArrayElement* pObj)
{
	Int32 lIndex = GetObjectIndex(pObj);
	if (lIndex < 0)
		return false;
	return RemoveAt(lIndex);
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::InsertAt(Int32 l)
{
	ArrayElement* pNew = AllocObject();
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.InsertAtA(l);
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::InsertAtType(Int32 lType, Int32 l)
{
	ArrayElement* pNew = AllocObjectType(lType);
	if (!pNew)
		return nullptr;

	ArrayElement** ppNew = m_Objects.InsertAtA(l);
	if (!ppNew)
	{
		FreeObject(pNew);
		return nullptr;
	}
	*ppNew = pNew;
	return *ppNew;
}

template <class ArrayElement> ArrayElement* CDynamicObjectArray<ArrayElement>::operator [] (Int32 l) const
{
	return m_Objects[l];
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::Read(HyperFile* pFile, Bool bFlush)
{
	Int32 lElements, lLevel;
	if (bFlush)
		m_Objects.Free();
	if (!CDynArrayReadHeader(pFile, lElements, lLevel))
		return false;
	if (bFlush)
	{
		if (!m_Objects.SetMinSizeNoCopy(lElements))
			return false;
	}

	for (Int32 a = 0; a < lElements; a++)
	{
		ArrayElement* pElement = AllocObject();
		if (!pElement)
			return false;
		if (!ReadElementHF(pFile, pElement, lLevel))
		{
			FreeObject(pElement);
			return false;
		}
		if (!m_Objects.Append(pElement))
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::ReadCreateObject(HyperFile* pFile, Bool bFlush)
{
	Int32 lElements, lLevel;
	if (bFlush)
		m_Objects.Free();
	if (!CDynArrayReadHeader(pFile, lElements, lLevel))
		return false;
	if (bFlush)
	{
		if (!m_Objects.SetMinSizeNoCopy(lElements))
			return false;
	}

	for (Int32 a = 0; a < lElements; a++)
	{
		ArrayElement* pElement = nullptr;
		if (!ReadElementCreate(pFile, pElement, lLevel))
		{
			FreeObject(pElement);
			return false;
		}
		if (!m_Objects.Append(pElement))
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::Write(HyperFile* pFile, Int32 lLevel) const
{
	Int32 lElements = m_Objects.GetElementCount();
	if (!CDynArrayWriteHeader(pFile, lElements, lLevel))
		return false;
	for (Int32 a = 0; a < lElements; a++)
	{
		if (!WriteElementHF(pFile, m_Objects[a]))
			return false;
	}
	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::ReadElement(BaseFile* pFile, ArrayElement* pElement, Int32 lLevel)
{
	// override!
	DebugAssert(false);
	return false;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::WriteElement(BaseFile* pFile, const ArrayElement* pElement) const
{
	// override!
	DebugAssert(false);
	return false;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::ReadElementHF(HyperFile* pFile, ArrayElement* pElement, Int32 lLevel)
{
	// override!
	DebugAssert(false);
	return false;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::WriteElementHF(HyperFile* pFile, const ArrayElement* pElement) const
{
	// override!
	DebugAssert(false);
	return false;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::ReadElementCreate(HyperFile* pFile, ArrayElement*& pElement, Int32 lLevel)
{
	// override!
	DebugAssert(false);
	return false;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::TestEqual(const ArrayElement* pObjA, const ArrayElement* pObjB) const
{
	// override!
	DebugAssert(false);
	return false;
}


template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::RemovePointer(Int32 lIndex)
{
	return m_Objects.RemoveAt(lIndex, true);
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::InsertObject(ArrayElement* pObj, Int32 lIndex)
{
	ArrayElement** ppNew = m_Objects.InsertAtA(lIndex);
	if (!ppNew)
		return false;
	*ppNew = pObj;
	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::Init(const ArrayElement* const* ppObjects, Int32 lCount, AliasTrans* pTrans, Bool bAppend)
{
	if (!bAppend)
		Free();
	if (!ppObjects || !lCount)
		return true;
	m_Objects.SetMinSizeNoCopy(lCount);
	Int32 a;
	for (a = 0; a < lCount; a++)
	{
		ArrayElement* pClone = GetClone(ppObjects[a], pTrans);
		if (!pClone)
			return false;
		if (!m_Objects.Append(pClone))
		{
			FreeObject(pClone);
			return false;
		}
	}
	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::CopyTo(CDynamicObjectArray<ArrayElement>* pDest, AliasTrans* pTrans, Bool bAppend) const
{
	if (!bAppend)
		pDest->Free();
	Int32 a, lElements;
	lElements = m_Objects.GetElementCount();
	const ArrayElement* const* ppElements = m_Objects.GetArray();
	pDest->m_Objects.SetMinSizeNoCopy(lElements);
	for (a = 0; a < lElements; a++)
	{
		if (!ppElements[a])
			continue;
		ArrayElement* pClone = GetClone(ppElements[a], pTrans);
		if (!pClone)
			return false;
		if (!pDest->m_Objects.Append(pClone))
		{
			FreeObject(pClone);
			return false;
		}
	}

	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::IsEqual(const CDynamicObjectArray<ArrayElement>* pArray) const
{
	if (pArray->GetElementCount() != GetElementCount())
		return false;

	const ArrayElement* const* ppDest = pArray->GetArray()->GetArray(), *const* ppThis = GetArray()->GetArray();
	Int32 lElements = GetElementCount();
	for (Int32 a = 0; a < lElements; a++)
	{
		if (!ppDest[a] || !ppThis[a])
			return false;
		if (!TestEqual(ppThis[a], ppDest[a]))
			return false;
	}

	return true;
}

template <class ArrayElement> Bool CDynamicObjectArray<ArrayElement>::Alloc(Int32 lElementCount)	// allocates memory to hold lElementCount elements more than currently, increases the counter
{
	Int32 lOldCounter = m_Objects.GetElementCount();
	if (!m_Objects.Alloc(lElementCount))
		return false;

	Int32 lNewCounter = lOldCounter + lElementCount;
	Int32 i;
	ArrayElement** ppDest = m_Objects.GetArray();
	for (i = lOldCounter; i < lNewCounter; i++)
	{
		ppDest[i] = AllocObject();
		if (!ppDest[i])
			return false;
	}

	m_Objects.SetCounter(lNewCounter);

	return true;
}

//////////////////////////////////////////////////////////////////////////

template <class TYPE> inline Int32 BinarySearch(const TYPE& v, TYPE* pData, Int32 lCount, Bool first = false)
{
	if (lCount == 0 || v < pData[0])
		return NOTOK;
	if (v > pData[lCount - 1])
		return NOTOK;

	Int32 l, u, m;

	l = 0;
	u = lCount - 1;

	while (l < u)
	{
		m = (l + u) >> 1;
		if (v >= pData[m])
			l = m + 1;
		else
			u = m - 1;
	}

	if (pData[l] == v)
	{
	}
	else if (l > 0 && pData[l - 1] == v)
	{
		l--;
	}
	else
	{
		return NOTOK;
	}

	if (first)
	{
		while (l > 0 && pData[l - 1] == v)
		{
			l--;
		}
	}

	return l;
}

template <class TYPE> inline void ShellSort(TYPE* pArray, Int32 lNumItems)
{
	Int32 i, j, h;
	TYPE	v;

	for (h = 1; h <= lNumItems / 9; h = 3 * h + 1)
	{
	}
	for (; h > 0; h /= 3)
	{
		for (i = h; i < lNumItems; i++)
		{
			v = pArray[i];
			j = i;
			while (j > h - 1 && pArray[j - h] > v)
			{
				pArray[j] = pArray[j - h];
				j -= h;
			}
			pArray[j] = v;
		}
	}
}
template <class TYPE, class LTYPE> inline void ShellSortLinked(TYPE* pArray, LTYPE* lArray, Int32 lNumItems)
{
	Int32 i, j, h;
	TYPE	v;
	LTYPE w;

	for (h = 1; h <= lNumItems / 9; h = 3 * h + 1)
	{
	}
	for (; h > 0; h /= 3)
	{
		for (i = h; i < lNumItems; i++)
		{
			v = pArray[i];
			w = lArray[i];

			j = i;
			while (j > h - 1 && pArray[j - h] > v)
			{
				pArray[j] = pArray[j - h];
				lArray[j] = lArray[j - h];

				j -= h;
			}
			pArray[j] = v;
			lArray[j] = w;
		}
	}
}
/*

  class DynamicArrayBase
  ----------------------

  DataTYPE used MUST be safe for memory copying and moving, including the default constructed data!

*/
template <class TYPE> inline void FillMem(TYPE* pData, Int32 lCount, TYPE* pFill)
{
	if (lCount > 0)
	{
#ifdef MAXON_TARGET_DEBUG
		FillMemType(TYPE, pData, lCount, 0xFF);
#endif
		CopyMemType(TYPE, pFill, pData, 1);

		lCount--;
		pFill	 = pData;
		pData += 1;

		Int32 lSize = 1;

		while (lCount > lSize)
		{
			CopyMemType(TYPE, pFill, pData, lSize);
			lCount -= lSize;
			pData	 += lSize;
			lSize	 += lSize;
		}

		if (lCount > 0)
			CopyMemType(TYPE, pFill, pData, lCount);
	}
}

template <class TYPE> inline void FillMem(TYPE* pData, Int32 lCount, const TYPE& pFill)
{
	FillMem(pData, lCount, (TYPE*)&pFill);
}

template <class TYPE, Int32 BLOCK> class DynamicArrayBase
{
protected:
	TYPE* m_pData;
	Int32	m_Cnt;				// total data size
	Int32	m_Allocated;	// actual allocated elements

	DynamicArrayBase()
	{
		Reset();
	}
	void Reset()
	{
		m_pData = nullptr;
		m_Cnt = 0;
		m_Allocated = 0;
	}

	Bool Add()
	{
		if (m_Allocated >= m_Cnt)
		{
			Int32 ncnt;
			TYPE* data = nullptr;

			if (m_Cnt == 0)
				ncnt = BLOCK;
			else
				ncnt = m_Cnt * 2;

			data = (TYPE*)MemAlloc(sizeof(TYPE) * ncnt);
			if (!data)
				return false;

			if (m_pData)
				CopyMem(m_pData, data, m_Allocated * sizeof(TYPE));
			DeleteMem(m_pData);

			m_pData = data;
			m_Cnt = ncnt;
		}

		TYPE def;
		CopyMem(&def, &m_pData[m_Allocated], sizeof(TYPE));

		m_Allocated++;

		return true;
	}

	void InitElements(Int32 cnt)
	{
		TYPE def;
		FillMem(&m_pData[m_Allocated], cnt - m_Allocated, &def);
		m_Allocated = cnt;
	}

	void FreeElements()
	{
		for (Int32 i = 0; i < m_Allocated; i++)
		{
			m_pData[i].~TYPE();
		}
		m_Allocated = 0;
	}

public:
	~DynamicArrayBase()
	{
		Free();
	}

	Int GetMemoryUse()
	{
		return sizeof(TYPE) * m_Cnt;
	}
	Int32 GetMaxCount()
	{
		return m_Cnt;
	}

	TYPE& operator[](int i)
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		return m_pData[i];
	}
	TYPE operator[](int i) const
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		return m_pData[i];
	}

	const DynamicArrayBase<TYPE, BLOCK>& operator = (const DynamicArrayBase<TYPE, BLOCK>& src)
	{
		((DynamicArrayBase*)&src)->CopyTo(this);
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////

	Bool Insert(Int32 i)
	{
		DebugAssert(i >= 0 && i <= m_Cnt);

		if (m_Allocated >= m_Cnt)
		{
			Int32 ncnt;
			TYPE* data = nullptr;

			if (m_Cnt == 0)
				ncnt = BLOCK;
			else
				ncnt = m_Cnt * 2;

			data = (TYPE*)MemAlloc(sizeof(TYPE) * ncnt);
			if (!data)
				return false;

			if (m_pData)
			{
				if (i > 0)
					CopyMem(m_pData, data, i * sizeof(TYPE));
				if (i < m_Allocated)
					CopyMem((Char*)m_pData + i * sizeof(TYPE), (Char*)data + (i + 1) * sizeof(TYPE), (m_Allocated - i) * sizeof(TYPE));
			}
			DeleteMem(m_pData);

			m_pData = data;
			m_Cnt = ncnt;
		}
		else
		{
			if (i < m_Allocated)
				memmove((Char*)m_pData + (i + 1) * sizeof(TYPE), (Char*)m_pData + i * sizeof(TYPE), (m_Allocated - i) * sizeof(TYPE));
		}

		TYPE def;
		CopyMem(&def, &m_pData[i], sizeof(TYPE));

		m_Allocated++;

		return true;
	}

	Bool RemoveLast()
	{
		if (m_Allocated > 0)
			return Remove(m_Allocated - 1);
		else
			return true;
	}

	Bool Remove(Int32 i)
	{
		DebugAssert(i >= 0 && i <= m_Cnt);

		m_pData[i].~TYPE();

		if (m_Allocated <= (m_Cnt >> 1) && m_Cnt > BLOCK)
		{
			Int32 ncnt = m_Cnt >> 1;
			TYPE* data = nullptr;

			data = (TYPE*)MemAlloc(sizeof(TYPE) * ncnt);
			if (!data)
				return false;

			if (m_pData)
			{
				if (i > 0)
					CopyMem(m_pData, data, i * sizeof(TYPE));
				if (i < (m_Allocated - 1))
					CopyMem((Char*)m_pData + (i + 1) * sizeof(TYPE), (Char*)data + i * sizeof(TYPE), (m_Allocated - i - 1) * sizeof(TYPE));
			}
			DeleteMem(m_pData);

			m_pData = data;
			m_Cnt = ncnt;
		}
		else
		{
			if (i < (m_Allocated - 1))
				memmove((Char*)m_pData + i * sizeof(TYPE), (Char*)m_pData + (i + 1) * sizeof(TYPE), (m_Allocated - i - 1) * sizeof(TYPE));
		}

		m_Allocated--;

		return true;
	}

	TYPE* GetLast()
	{
		if (m_Allocated == 0)
			return nullptr;
		return &m_pData[m_Allocated - 1];
	}
	TYPE* GetAt(Int32 i)
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		return &m_pData[i];
	}
	void Set(Int32 i, const TYPE& data)
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		m_pData[i] = data;
	}
	TYPE& Get(Int32 i)
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		return m_pData[i];
	}

	Int32 Find(const TYPE& data, Int32 l, Int32 u)
	{
		for (Int32 i = l; i <= u; i++)
		{
			if (m_pData[i] == data)
				return i;
		}
		return NOTOK;
	}

	Int32 Find(const TYPE& data)
	{
		for (Int32 i = 0; i < m_Allocated; i++)
		{
			if (m_pData[i] == data)
				return i;
		}
		return NOTOK;
	}

	Int32 FindSorted(const TYPE& data)
	{
		return BinarySearch(data, m_pData, m_Allocated, true);
	}

	Bool SetCount(Int32 cnt)
	{
		if (cnt > m_Cnt)
		{
			if (!Expand(cnt))
				return false;
		}

		if (cnt < m_Allocated)
		{
			for (Int32 i = cnt; i < m_Allocated; i++)
			{
				m_pData[i].~TYPE();
			}
			m_Allocated = cnt;
		}
		else if (cnt > m_Allocated)
		{
			InitElements(cnt);
		}

		return true;
	}
	Int32 GetCount() const
	{
		return m_Allocated;
	}
	Bool Content() const
	{
		return m_Allocated > 0;
	}
	Bool IsEmpty() const
	{
		return GetCount() == 0;
	}
	Bool Expand(Int32 maxcnt)
	{
		if (maxcnt > m_Cnt)
		{
			Int32 ncnt;
			TYPE* data = nullptr;

			ncnt = maxcnt;
			data = (TYPE*)MemAlloc(sizeof(TYPE) * ncnt);
			if (!data)
				return false;

			if (m_pData)
				CopyMem(m_pData, data, m_Allocated * sizeof(TYPE));
			DeleteMem(m_pData);

			m_pData = data;
			m_Cnt = ncnt;
		}

		return true;
	}

	Bool CopyTo(DynamicArrayBase<TYPE, BLOCK>* dst) const
	{
		if (dst == this)
			return true;

		if (!dst->SetCount(m_Allocated))
			return false;
		for (Int32 i = 0; i < m_Allocated; i++)
		{
			dst->m_pData[i] = m_pData[i];
		}

		return true;
	}

	Bool CopyFrom(TYPE* data, Int32 cnt)
	{
		if (!SetCount(cnt))
			return false;
		for (Int32 i = 0; i < m_Allocated; i++)
		{
			m_pData[i] = data[i];
		}

		return true;
	}
	Bool CopyFrom(const DynamicArrayBase<TYPE, BLOCK>* src)
	{
		return src->CopyTo(this);
	}

	Bool Init(Int32 cnt = NOTOK)
	{
		Free();
		if (cnt == NOTOK)
			cnt = BLOCK;
		return Expand(cnt);
	}
	void Free()
	{
		FreeElements();
		DeleteMem(m_pData);
		Reset();
	}

	void TransferTo(DynamicArrayBase<TYPE, BLOCK>& dst)
	{
		dst.Free();
		dst.m_pData = m_pData;
		dst.m_Cnt = m_Cnt;
		dst.m_Allocated = m_Allocated;
		Reset();
	}

	TYPE* GetArray()
	{
		return m_pData;
	}
	const TYPE* GetArray() const
	{
		return m_pData;
	}

	void Sort()
	{
		Int32 i, j, h;
		TYPE	v;

		for (h = 1; h <= m_Allocated / 9; h = 3 * h + 1)
		{
		}
		for (; h > 0; h /= 3)
		{
			for (i = h; i < m_Allocated; i++)
			{
				v = m_pData[i];
				j = i;
				while (j > h - 1 && (m_pData[j - h] > v))
				{
					m_pData[j] = m_pData[j - h];
					j -= h;
				}
				m_pData[j] = v;
			}
		}
	}

	TYPE* ToArray()
	{
		if (m_Allocated == 0)
			return nullptr;

		TYPE* data = (TYPE*)MemAlloc(sizeof(TYPE) * m_Allocated);
		if (!data)
			return nullptr;

		CopyMem(m_pData, data, sizeof(TYPE) * m_Allocated);

		return data;
	}
};

//////////////////////////////////////////////////////////////////////////

class SelectArray;
template <class TYPE> class SelectArrayExt;

template <class TYPE, Int32 BLOCK> class SortedArray : public DynamicArrayBase<TYPE, BLOCK>
{
	friend class SelectArray;
	friend class SelectArrayExt<TYPE>;

protected:
	Int32 SearchIndex(const TYPE& v)
	{
		if (GCC34T m_Allocated == 0 || v <= GCC34T m_pData[0])
			return 0;
		if (v >= GCC34T m_pData[GCC34T m_Allocated - 1])
			return GCC34T m_Allocated;

		Int32 l, u, m;

		l = 1;
		u = GCC34T m_Allocated - 2;

		while (l < u)
		{
			m = (l + u) >> 1;
			if (v >= GCC34T m_pData[m])
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == GCC34T m_pData[l - 1])
			l--;
		else if (v > GCC34T m_pData[l])
			l++;

		return l;
	}

	Int32 SearchEqualIndex(const TYPE& v)
	{
		if (GCC34T m_Allocated == 0 || v <= GCC34T m_pData[0])
			return 0;
		if (v > GCC34T m_pData[GCC34T m_Allocated - 1])
			return GCC34T m_Allocated;
		if (v == GCC34T m_pData[GCC34T m_Allocated - 1])
			return GCC34T m_Allocated - 1;

		Int32 l, u, m;

		l = 1;
		u = GCC34T m_Allocated - 2;

		while (l < u)
		{
			m = (l + u) >> 1;
			if (v >= GCC34T m_pData[m])
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == GCC34T m_pData[l - 1])
			l--;
		else if (v > GCC34T m_pData[l])
			l++;

		return l;
	}

public:
	SortedArray()
	{
	}

	const SortedArray<TYPE, BLOCK>& operator = (const SortedArray<TYPE, BLOCK>& src)
	{
		((SortedArray*)&src)->CopyTo(this);
		return *this;
	}

	Bool AddLast()
	{
		return DynamicArrayBase<TYPE, BLOCK>::Add();
	}

	Bool AddLast(const TYPE& v)
	{
		if (!DynamicArrayBase<TYPE, BLOCK>::Add())
			return false;
		this->Set(GCC34T m_Allocated - 1, v);
		return true;
	}

	Bool Add(const TYPE& data)
	{
		Int32 i = SearchIndex(data);
		if (!DynamicArrayBase<TYPE, BLOCK>::Insert(i))
			return false;
		this->Set(i, data);
		return true;
	}

	Bool AddUnique(const TYPE& v)
	{
		if (GCC34T m_Allocated > 0)
		{
			Int32 i = SearchIndex(v);
			if (i == GCC34T m_Allocated && v == GCC34T m_pData[i - 1])
				return true;
			if (i < GCC34T m_Allocated && v == GCC34T m_pData[i])
				return true;
			if (!DynamicArrayBase<TYPE, BLOCK>::Insert(i))
				return false;
			this->Set(i, v);
			return true;
		}
		else
		{
			return Add(v);
		}
	}

	TYPE* GetUnique(const TYPE& v, Bool& bExisted)
	{
		if (GCC34T m_Allocated > 0)
		{
			Int32 i = SearchIndex(v);
			if (i == GCC34T m_Allocated && v == GCC34T m_pData[i - 1])
			{
				bExisted = true;
				return &(GCC34T m_pData[i - 1]);
			}
			if (i < GCC34T m_Allocated && v == GCC34T m_pData[i])
			{
				bExisted = true;
				return &(GCC34T m_pData[i]);
			}
			if (!DynamicArrayBase<TYPE, BLOCK>::Insert(i))
				return nullptr;
			Set(i, v);
			bExisted = false;
			return &(GCC34T m_pData[i]);
		}
		else
		{
			bExisted = false;
			if (!Add(v))
				return nullptr;
			return &(GCC34T m_pData[0]);
		}
		bExisted = false;
		return nullptr;
	}

	Bool AddUnique(const TYPE& v, Bool& set)
	{
		if (GCC34T m_Allocated > 0)
		{
			Int32 i = SearchIndex(v);

			set = false;
			if (i == GCC34T m_Allocated && v == GCC34T m_pData[i - 1])
				return true;
			if (i < GCC34T m_Allocated && v == GCC34T m_pData[i])
				return true;
			if (!DynamicArrayBase<TYPE, BLOCK>::Insert(i))
				return false;
			Set(i, v);
			set = true;
			return true;
		}
		else
		{
			set = true;
			return Add(v);
		}
	}

	Bool RemoveValue(const TYPE& v)
	{
		Int32 i = Find(v);
		if (i == NOTOK)
			return true;
		return GCC34T Remove(i);
	}

	Bool RemoveValue(const TYPE& v, Bool& set)
	{
		Int32 i = Find(v);
		if (i == NOTOK)
		{
			set = false;
			return true;
		}
		set = true;
		return GCC34T Remove(i);
	}

	Int32 Find(const TYPE& v)
	{
		if (!GCC34T m_pData || GCC34T m_Allocated == 0)
			return NOTOK;

		if (v < GCC34T m_pData[0] || v > GCC34T m_pData[GCC34T m_Allocated - 1])
			return NOTOK;
		if (v == GCC34T m_pData[0])
			return 0;
		if (v == GCC34T m_pData[GCC34T m_Allocated - 1])
			return GCC34T m_Allocated - 1;

		Int32 i = SearchIndex(v);

		if (v == GCC34T m_pData[i])
			return i;
		return NOTOK;
	}

	Int32 FindValueIndex(const TYPE& v)
	{
		Int32 i = SearchIndex(v);
		if (GCC34T m_Allocated == 0)
			return 0;
		if (i == GCC34T m_Allocated && v == GCC34T m_pData[i - 1])
			i--;
		return i;
	}
};

//////////////////////////////////////////////////////////////////////////

// elements can be moved in memory so pointers are not valid and TYPE must be safe with this and a nullptr init

template <class TYPE, Int32 BLOCK> class DynamicArray : public DynamicArrayBase<TYPE, BLOCK>
{
public:
	DynamicArray()
	{
	}

	const DynamicArray<TYPE, BLOCK>& operator = (const DynamicArray<TYPE, BLOCK>& src)
	{
		((DynamicArray*)&src)->CopyTo(this);
		return *this;
	}

	Bool Add()
	{
		return DynamicArrayBase<TYPE, BLOCK>::Add();
	}
	Bool Add(const TYPE& v)
	{
		if (!DynamicArrayBase<TYPE, BLOCK>::Add())
			return false;
		this->Set(GCC34T m_Allocated - 1, v);
		return true;
	}
	TYPE* AddLast()
	{
		if (!DynamicArrayBase<TYPE, BLOCK>::Add())
			return nullptr;
		return DynamicArrayBase<TYPE, BLOCK>::GetLast();
	}
};

//////////////////////////////////////////////////////////////////////////

template <class TYPE, Int32 MINBLOCK, Int32 MAXBLOCK> class DynamicBlockArray
{
protected:
	struct DynamicDataBlock
	{
public:
		DynamicDataBlock()
		{
			data = nullptr;
			allocated = 0;
			block = MINBLOCK;
			adjustment = 0;
		}
		~DynamicDataBlock() { }

		TYPE* data;
		Int32 allocated;
		Int32 block;
		Int32 adjustment;
	};

	Int32 m_Allocated;
	DynamicArray<DynamicDataBlock, 128> m_Blocks;

	//	MemoryStack *m_pMemStack;

	//////////////////////////////////////////////////////////////////////////

	DynamicDataBlock* AllocBlock()
	{
		DynamicDataBlock* data;

		if (!m_Blocks.Add())
			return nullptr;
		data = m_Blocks.GetAt(m_Blocks.GetCount() - 1);

		data->data = (TYPE*)MemAlloc(sizeof(TYPE) * data->block);
		if (!data->data)
			return nullptr;

		return data;
	}

	TYPE* AddData()
	{
		DynamicDataBlock* data = m_Blocks.GetLast();
		if (!data)
		{
			data = AllocBlock();
			if (!data)
				return nullptr;
		}

		if (data->allocated == data->block)
		{
			if (data->block == MAXBLOCK)
			{
				data = AllocBlock();
				if (!data)
					return nullptr;
			}
			else
			{
				TYPE* ndata = (TYPE*)MemAlloc(sizeof(TYPE) * data->block * 2);
				if (!ndata)
					return nullptr;

				CopyMem(data->data, ndata, sizeof(TYPE) * data->block);

				MemFree((void*&)*data->data);
				data->data	 = ndata;
				data->block *= 2;
			}
		}

		data->allocated++;
		m_Allocated++;

		CorrectAdjustments(m_Blocks.GetCount() - 1);

		return data->data + data->allocated - 1;
	}

	//////////////////////////////////////////////////////////////////////////

public:
	//	void Attach(MemoryStack *pMem) { Free(); m_pMemStack=pMem; m_Blocks.Attach(pMem); }
	Int GetMemoryUse()
	{
		Int		used = 0;
		Int32 i;

		for (i = 0; i < m_Blocks.GetCount(); i++)
		{
			used += m_Blocks[i].block * sizeof(TYPE);
		}

		return m_Blocks.GetMemoryUse() + used;
	}
	Int GetMaxCount()
	{
		Int32 cnt = 0, i;
		for (i = 0; i < m_Blocks.GetCount(); i++)
		{
			cnt += m_Blocks[i].block;
		}
		return cnt;
	}

	DynamicDataBlock* InsertBlock(Int32 b)
	{
		DynamicDataBlock* data;

		if (!m_Blocks.Insert(b))
			return nullptr;
		data = m_Blocks.GetAt(b);

		data->data = (TYPE*)MemAlloc(sizeof(TYPE) * data->block);
		if (!data->data)
			return nullptr;

		return data;
	}

	void CorrectAdjustments(Int32 b)
	{
		Int32 a;

		if (b > 0)
			a = m_Blocks[b - 1].adjustment + (MAXBLOCK - m_Blocks[b - 1].allocated);
		else
			a = 0;

		for (Int32 i = b; i < m_Blocks.GetCount(); i++)
		{
			DebugAssert(m_Blocks[i].allocated > 0);

			m_Blocks[i].adjustment = a;
			a += (MAXBLOCK - m_Blocks[i].allocated);
		}
	}

	TYPE* InsertData(Int32& b, Int32& i)
	{
		if (b == m_Blocks.GetCount())
			return AddData();

		DynamicDataBlock* data = m_Blocks.GetAt(b);

		DebugAssert(data->allocated > 0);

		if (i >= MAXBLOCK)
		{
			DebugAssert(i == MAXBLOCK);

			b++;
			i = 0;
			data = InsertBlock(b);
			if (!data)
				return nullptr;

			data->allocated++;
			m_Allocated++;

			CorrectAdjustments(b);

			return data->data;
		}

		if (data->allocated == data->block)
		{
			if (data->block == MAXBLOCK)
			{
				DynamicDataBlock* ndata;

				if (b == (m_Blocks.GetCount() - 1) || m_Blocks[b + 1].allocated == MAXBLOCK)
				{
					if (b > 0 && m_Blocks[b - 1].allocated < MAXBLOCK)
					{
						ndata = m_Blocks.GetAt(b - 1);
						if (ndata->allocated == ndata->block)
						{
							TYPE* nblock = (TYPE*)MemAlloc(sizeof(TYPE) * ndata->block * 2);
							if (!nblock)
								return nullptr;

							CopyMem(ndata->data, nblock, sizeof(TYPE) * ndata->block);

							MemFree((void*&)*ndata->data);
							ndata->data		= nblock;
							ndata->block *= 2;
						}

						if (i > 0)
						{
							Int32 cblock = (ndata->block - ndata->allocated);
							if (cblock > i)
								cblock = i;

							CopyMem(data->data, ndata->data + ndata->allocated, cblock * sizeof(TYPE));
							ndata->allocated += cblock;

							if (i > cblock)
								memmove(data->data, data->data + cblock, (i - cblock) * sizeof(TYPE));
							if (i < data->allocated)
								memmove(data->data + i - cblock + 1, data->data + i, sizeof(TYPE) * (data->allocated - i));

							i -= cblock;
							data->allocated -= (cblock - 1);
							m_Allocated++;

							DebugAssert(data->allocated > 0);

							CorrectAdjustments(b - 1);

							return data->data + i;
						}
						else
						{
							i = ndata->allocated;
							ndata->allocated++;
							m_Allocated++;
							b--;

							CorrectAdjustments(b);

							return ndata->data + i;
						}
					}

					ndata = InsertBlock(b + 1);
					if (!ndata)
						return nullptr;
					data = m_Blocks.GetAt(b);
				}
				else
				{
					ndata = m_Blocks.GetAt(b + 1);
				}

				if (ndata->allocated == ndata->block)
				{
					TYPE* nblock = (TYPE*)MemAlloc(sizeof(TYPE) * ndata->block * 2);
					if (!nblock)
						return nullptr;

					CopyMem(ndata->data, nblock, sizeof(TYPE) * ndata->block);

					MemFree((void*&)*ndata->data);
					ndata->data		= nblock;
					ndata->block *= 2;
				}

				Int32 cblock = (ndata->block - ndata->allocated);
				if (cblock < 1)
					cblock = 1;
				else if (cblock > (MAXBLOCK - i))
					cblock = MAXBLOCK - i;

				if (ndata->allocated > 0)
					memmove(ndata->data + cblock, ndata->data, sizeof(TYPE) * ndata->allocated);
				CopyMem(data->data + MAXBLOCK - cblock, ndata->data, sizeof(TYPE) * cblock);

				data->allocated	 -= cblock;
				ndata->allocated += cblock;

				DebugAssert(data->allocated > 0);

				if (i < data->allocated)
					memmove(data->data + i + 1, data->data + i, sizeof(TYPE) * (data->allocated - i));
			}
			else
			{
				TYPE* ndata = (TYPE*)MemAlloc(sizeof(TYPE) * data->block * 2);
				if (!ndata)
					return nullptr;

				if (i > 0)
					CopyMem(data->data, ndata, sizeof(TYPE) * i);
				if (i < data->allocated)
					CopyMem(data->data + i, ndata + i + 1, sizeof(TYPE) * (data->allocated - i));

				MemFree((void*&)*data->data);
				data->data	 = ndata;
				data->block *= 2;
			}
		}
		else
		{
			if (i < data->allocated)
				memmove(data->data + i + 1, data->data + i, sizeof(TYPE) * (data->allocated - i));
		}

		data->allocated++;
		m_Allocated++;

		CorrectAdjustments(b);

		return data->data + i;
	}

	Int32 GetIndex(Int32& i)
	{
		Int32 b = i / MAXBLOCK, a = 0;
		if (b >= m_Blocks.GetCount())
			b--;

		DynamicDataBlock* data = m_Blocks.GetAt(b);

		i = i - (b * MAXBLOCK);
		do
		{
			DebugAssert(data->allocated > 0);

			while (i >= data->allocated)
			{
				i -= data->allocated;
				b++;
				a += (MAXBLOCK - data->allocated);
				data = m_Blocks.GetAt(b);

				DebugAssert(data->allocated > 0);
			}

			if (data->adjustment)
			{
				i = i - a + data->adjustment;
				a = data->adjustment;
			}
			else
			{
				break;
			}

		} while (i >= data->allocated);

		return b;
	}

	Int32 GetBlockIndex(Int32 b, Int32 i) const
	{
		if (b >= m_Blocks.GetCount())
			return m_Allocated;

		DebugAssert((b * MAXBLOCK - m_Blocks[b].adjustment + i) < m_Allocated);

		return b * MAXBLOCK - m_Blocks[b].adjustment + i;
	}

	Bool BlockInsert(Int32& b, Int32& i)
	{
		TYPE* data = InsertData(b, i), def;
		if (!data)
			return false;

		CopyMem(&def, data, sizeof(TYPE));

		return true;
	}

	void BlockSet(Int32 b, Int32 i, const TYPE& v)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);
		m_Blocks[b].data[i] = v;
	}

	TYPE* BlockGetAt(Int32 b, Int32 i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);
		return &m_Blocks[b].data[i];
	}

	TYPE& BlockGet(Int32 b, Int32 i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);
		return m_Blocks[b].data[i];
	}

	TYPE* BlockPrev(Int32 b, Int32 i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);

		if (i > 0)
		{
			return &m_Blocks[b].data[i - 1];
		}
		else
		{
			DebugAssert(b > 0);
			return &m_Blocks[b - 1].data[m_Blocks[b - 1].allocated - 1];
		}
	}

	TYPE* BlockNext(Int32 b, Int32 i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);

		if (i < (m_Blocks[b].allocated - 1))
			return &m_Blocks[b].data[i + 1];
		else
			return &m_Blocks[b + 1].data[0];
	}

	TYPE* BlockGetPrev(Int32& b, Int32& i)
	{
		if (i > 0)
		{
			i--;
		}
		else
		{
			b--;
			if (b < 0)
				return nullptr;
			i = m_Blocks[b].allocated - 1;
		}

		return &m_Blocks[b].data[i];
	}

	TYPE* BlockGetNext(Int32& b, Int32& i)
	{
		if (i < (m_Blocks[b].allocated - 1))
		{
			i++;
		}
		else
		{
			b++;
			i = 0;
		}

		if (b >= m_Blocks.GetCount() || i >= m_Blocks[b].allocated)
			return nullptr;

		return &m_Blocks[b].data[i];
	}

	void BlockNextIndex(Int32& b, Int32& i)
	{
		if (i < (m_Blocks[b].allocated - 1))
		{
			i++;
		}
		else
		{
			b++;
			i = 0;
		}
	}

	TYPE* GetAtInc(Int32& b, Int32& i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);

		TYPE* data = &m_Blocks[b].data[i];

		if (i < (m_Blocks[b].allocated - 1))
		{
			i++;
		}
		else
		{
			b++;
			i = 0;
		}

		return data;
	}

	Bool Remove(Int32& b, Int32& i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);

		m_Blocks[b].data[i].~TYPE();

		if (m_Blocks[b].allocated <= (m_Blocks[b].block >> 1) && m_Blocks[b].block > MINBLOCK)
		{
			Int32 ncnt = m_Blocks[b].block >> 1;
			TYPE* data = nullptr;

			data = (TYPE*)MemAlloc(sizeof(TYPE) * ncnt);
			if (!data)
				return false;

			if (i > 0)
				CopyMem(m_Blocks[b].data, data, i * sizeof(TYPE));
			if (i < (m_Blocks[b].allocated - 1))
				CopyMem((Char*)m_Blocks[b].data + (i + 1) * sizeof(TYPE), (Char*)data + i * sizeof(TYPE), (m_Blocks[b].allocated - i - 1) * sizeof(TYPE));

			MemFree((void*&)*m_Blocks[b].data);

			m_Blocks[b].data	= data;
			m_Blocks[b].block = ncnt;
		}
		else
		{
			if (i < (m_Blocks[b].allocated - 1))
				memmove((Char*)m_Blocks[b].data + i * sizeof(TYPE), (Char*)m_Blocks[b].data + (i + 1) * sizeof(TYPE), (m_Blocks[b].allocated - i - 1) * sizeof(TYPE));
		}

		m_Blocks[b].allocated--;
		m_Allocated--;

		if (m_Blocks[b].allocated == 0)
		{
			MemFree((void*&)*m_Blocks[b].data);
			if (!m_Blocks.Remove(b))
				return false;
			if (b < m_Blocks.GetCount())
				CorrectAdjustments(b);
			i = 0;
		}
		else
		{
			CorrectAdjustments(b);
			if (i >= m_Blocks[b].allocated)
			{
				b++;
				i = 0;
			}
		}

		return true;
	}

	Bool BlockRemove(Int32& b, Int32& i)
	{
		return Remove(b, i);
	}

	//////////////////////////////////////////////////////////////////////////

	DynamicBlockArray()
	{
		Reset();
	}
	~DynamicBlockArray()
	{
		Free();
	}

	TYPE& operator[](int i)
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		return Get(i);
	}
	TYPE operator[](int i) const
	{
		DebugAssert(i >= 0 && i < m_Allocated);
		return Get(i);
	}

	void Free()
	{
		for (Int32 i = 0; i < m_Blocks.GetCount(); i++)
		{
			for (Int32 l = 0; l < m_Blocks[i].allocated; l++)
			{
				m_Blocks[i].data[l].~TYPE();
			}
			DeleteMem(m_Blocks[i].data);
		}

		m_Blocks.Free();
		Reset();
	}

	void Reset()
	{
		m_Allocated = 0;
	}

	Int32 GetCount() const
	{
		return m_Allocated;
	}

	TYPE* GetAt(Int32 i)
	{
		Int32 b = GetIndex(i);
		return m_Blocks.GetAt(b)->data + i;
	}

	TYPE* GetAt(Int32 b, Int32 i)
	{
		DebugAssert(b < m_Blocks.GetCount());
		DebugAssert(i < m_Blocks[b].allocated);
		return &m_Blocks[b].data[i];
	}

	TYPE& Get(Int32 i)
	{
		Int32 b = GetIndex(i);
		return *(m_Blocks.GetAt(b)->data + i);
	}

	TYPE& GetLast()
	{
		Int32 b = m_Blocks.GetCount() - 1, i;
		i = m_Blocks[b].allocated - 1;
		return *(m_Blocks.GetAt(b)->data + i);
	}

	TYPE* GetAtLast()
	{
		Int32 b = m_Blocks.GetCount() - 1, i;
		i = m_Blocks[b].allocated - 1;
		return m_Blocks.GetAt(b)->data + i;
	}

	TYPE* GetAtLast(Int32& b, Int32& i)
	{
		b = m_Blocks.GetCount() - 1;
		i = m_Blocks[b].allocated - 1;
		return m_Blocks.GetAt(b)->data + i;
	}

	void Set(Int32 i, const TYPE& v)
	{
		Int32 b = GetIndex(i);
		*(m_Blocks.GetAt(b)->data + i) = v;
	}

	Bool Insert(Int32 i)
	{
		if (i == m_Allocated)
			return Add();

		Int32 b = GetIndex(i);
		TYPE* data = InsertData(b, i), def;
		if (!data)
			return false;

		CopyMem(&def, data, sizeof(TYPE));

		return true;
	}

	Bool Insert(Int32 i, const TYPE& v)
	{
		if (i == m_Allocated)
			return Add(v);

		Int32 b = GetIndex(i);
		TYPE* data = InsertData(b, i), def;
		if (!data)
			return false;

		CopyMem(&def, data, sizeof(TYPE));
		*data = v;

		return true;
	}

	Bool Add()
	{
		TYPE* data = AddData(), def;
		if (!data)
			return false;

		CopyMem(&def, data, sizeof(TYPE));

		return true;
	}

	Bool Add(const TYPE& v)
	{
		TYPE* data = AddData(), def;
		if (!data)
			return false;

		CopyMem(&def, data, sizeof(TYPE));
		*data = v;

		return true;
	}

	Bool Remove(Int32 i)
	{
		Int32 b = GetIndex(i);
		return Remove(b, i);
	}

	Bool CopyTo(DynamicBlockArray* dst)
	{
		if (dst == this)
			return true;

		dst->Free();
		if (!dst->m_Blocks.SetCount(m_Blocks.GetCount()))
			return false;

		Int32 b, i;

		for (b = 0; b < m_Blocks.GetCount(); b++)
		{
			dst->m_Blocks[b].data = (TYPE*)MemAlloc(m_Blocks[b].block * sizeof(TYPE));
			if (!dst->m_Blocks[b].data)
				goto Error2;

			TYPE def;

			FillMem(dst->m_Blocks[b].data, m_Blocks[b].allocated, &def);

			for (i = 0; i < m_Blocks[b].allocated; i++)
			{
				dst->m_Blocks[b].data[i] = m_Blocks[b].data[i];
			}

			dst->m_Blocks[b].adjustment = m_Blocks[b].adjustment;
			dst->m_Blocks[b].allocated	= m_Blocks[b].allocated;
			dst->m_Blocks[b].block = m_Blocks[b].block;
		}

		dst->m_Allocated = m_Allocated;

		return true;
Error2:
		dst->Free();
		return false;
	}

	TYPE* ToTable(Bool directcopy = true)
	{
		if (!m_Allocated)
			return nullptr;

		TYPE* table;

		if (directcopy)
		{
			table = NewMemClear(TYPE, m_Allocated);

			if (!table)
				return nullptr;

			Int32 b, o = 0;

			for (b = 0; b < m_Blocks.GetCount(); b++)
			{
				CopyMem(m_Blocks[b].data, table + o, sizeof(TYPE) * m_Blocks[b].allocated);
				o += m_Blocks[b].allocated;
			}
		}
		else
		{
			Int32 o = 0, b, i;

			table = NewMemClear(TYPE, m_Allocated);
			if (!table)
				return nullptr;

			for (b = 0; b < m_Blocks.GetCount(); b++)
			{
				for (i = 0; i < m_Blocks[b].allocated; i++)
				{
					table[o++] = m_Blocks[b].data[i];
				}
			}
		}

		return table;
	}

	void FromTable(TYPE* table, Bool directcopy = true)
	{
		if (!m_Allocated || !table)
			return;

		if (directcopy)
		{
			Int32 b, o = 0;

			for (b = 0; b < m_Blocks.GetCount(); b++)
			{
				CopyMem(table + o, m_Blocks[b].data, sizeof(TYPE) * m_Blocks[b].allocated);
				o += m_Blocks[b].allocated;
			}
		}
		else
		{
			Int32 o = 0, b, i;

			for (b = 0; b < m_Blocks.GetCount(); b++)
			{
				for (i = 0; i < m_Blocks[b].allocated; i++)
				{
					m_Blocks[b].data[i] = table[o++];
				}
			}
		}
	}

	void CopyTo(TYPE* table)
	{
		if (!m_Allocated || !table)
			return;

		Int32 b, o = 0;

		for (b = 0; b < m_Blocks.GetCount(); b++)
		{
			CopyMem(m_Blocks[b].data, table + o, sizeof(TYPE) * m_Blocks[b].allocated);
			o += m_Blocks[b].allocated;
		}
	}

	void CopyFrom(TYPE* table)
	{
		if (!m_Allocated || !table)
			return;

		Int32 b, o = 0;

		for (b = 0; b < m_Blocks.GetCount(); b++)
		{
			CopyMem(table + o, m_Blocks[b].data, sizeof(TYPE) * m_Blocks[b].allocated);
			o += m_Blocks[b].allocated;
		}
	}

	void Sort()
	{
		TYPE* table = ToTable();
		if (!table)
		{
			Int32 i, j, h;
			TYPE	v, t;

			for (h = 1; h <= m_Allocated / 9; h = 3 * h + 1)
			{
			}
			for (; h > 0; h /= 3)
			{
				for (i = h; i < m_Allocated; i++)
				{
					v = Get(i);
					j = i;
					while (j > h - 1 && (t = Get(j - h)) > v)
					{
						Set(j, t);
						j -= h;
					}
					Set(j, v);
				}
			}
		}
		else
		{
			ShellSort<TYPE>(table, m_Allocated);
			FromTable(table);

			DeleteMem(table);
		}
	}

	Bool SetCount(Int32 ncnt)
	{
		Int32 cnt = ncnt, i, l;

		for (i = 0; i < m_Blocks.GetCount(); i++)
		{
			if (cnt <= MAXBLOCK)
			{
				if (cnt <= m_Blocks[i].allocated)
				{
					for (l = cnt; l < m_Blocks[i].allocated; l++)
					{
						m_Blocks[i].data[l].~TYPE();
					}
				}
				else if (cnt <= m_Blocks[i].block)
				{
					TYPE def;
					FillMem(&m_Blocks[i].data[m_Blocks[i].allocated], cnt - m_Blocks[i].allocated, &def);
				}
				else
				{
					Int32 bsize = m_Blocks[i].block;
					while (bsize < cnt)
					{
						bsize *= 2;
					}

					TYPE* ndata = (TYPE*)MemAlloc(bsize * sizeof(TYPE));
					if (!ndata)
						return false;

					CopyMem(m_Blocks[i].data, ndata, sizeof(TYPE) * m_Blocks[i].allocated);
					MemFree((void*&)(*m_Blocks[i].data));

					m_Blocks[i].data	= ndata;
					m_Blocks[i].block = bsize;

					TYPE def;
					FillMem(&m_Blocks[i].data[m_Blocks[i].allocated], cnt - m_Blocks[i].allocated, &def);
				}

				m_Blocks[i].allocated = cnt;
				if (!m_Blocks.SetCount(i + 1))
					return false;
				cnt = 0;

				break;
			}
			else if (m_Blocks[i].allocated < MAXBLOCK)
			{
				if (m_Blocks[i].block < MAXBLOCK)
				{
					TYPE* ndata = (TYPE*)MemAlloc(MAXBLOCK * sizeof(TYPE));
					if (!ndata)
						return false;

					CopyMem(m_Blocks[i].data, ndata, sizeof(TYPE) * m_Blocks[i].allocated);
					MemFree((void*&)*m_Blocks[i].data);

					m_Blocks[i].data	= ndata;
					m_Blocks[i].block = MAXBLOCK;
				}

				TYPE def;
				FillMem(&m_Blocks[i].data[m_Blocks[i].allocated], MAXBLOCK - m_Blocks[i].allocated, &def);

				m_Blocks[i].allocated = MAXBLOCK;
			}

			cnt -= MAXBLOCK;
		}

		DynamicDataBlock* data;

		while (cnt > 0)
		{
			if (!m_Blocks.Add())
				return false;
			data = m_Blocks.GetAt(m_Blocks.GetCount() - 1);

			Int32 bsize = MINBLOCK;
			while (bsize < cnt && bsize < MAXBLOCK)
			{
				bsize *= 2;
			}

			data->data = (TYPE*)MemAlloc(sizeof(TYPE) * bsize);
			if (!data->data)
				return false;

			if (cnt > MAXBLOCK)
				data->allocated = MAXBLOCK;
			else
				data->allocated = cnt;

			data->block = bsize;

			TYPE def;
			FillMem(m_Blocks[i].data, m_Blocks[i].allocated, &def);

			cnt -= data->allocated;
		}

		CorrectAdjustments(0);

		m_Allocated = ncnt;

		return true;
	}
};

template <class TYPE, Int32 MINBLOCK, Int32 MAXBLOCK> class SortedBlockArray : public DynamicBlockArray<TYPE, MINBLOCK, MAXBLOCK>
{
	friend class SelectBlockArray;

protected:
	Int32 SearchIndex(const TYPE& v)
	{
		if (GCC34T m_Allocated == 0 || v <= GCC34T Get(0))
			return 0;
		if (v >= GCC34T Get(GCC34T m_Allocated - 1))
			return GCC34T m_Allocated;

		Int32 l, u, m;

		l = 1;
		u = GCC34T m_Allocated - 2;

		while (l < u)
		{
			m = (l + u) >> 1;

			if (v >= GCC34T Get(m))
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == GCC34T Get(l - 1))
			l--;
		else if (v > GCC34T Get(l))
			l++;

		return l;
	}

	Int32 SearchEqualIndex(const TYPE& v)
	{
		TYPE* d;

		if (GCC34T m_Allocated == 0 || v <= GCC34T Get(0))
			return 0;

		d = GetAt(GCC34T m_Allocated - 1);
		if (v > (*d))
			return GCC34T m_Allocated;
		if (v == (*d))
			return GCC34T m_Allocated - 1;

		Int32 l, u, m;

		l = 1;
		u = GCC34T m_Allocated - 2;

		while (l < u)
		{
			m = (l + u) >> 1;

			if (v >= GCC34T Get(m))
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == GCC34T Get(l - 1))
			l--;
		else if (v > GCC34T Get(l))
			l++;

		return l;
	}

	Int32 SearchBlockIndex(const TYPE& v, Int32& b, Int32& i)
	{
		if (GCC34T m_Allocated == 0 || v <= GCC34T m_Blocks[0].data[0])
		{
			b = i = 0;
			return 0;
		}

		b = GCC34T m_Blocks.GetCount() - 1;
		i = GCC34T m_Blocks[b].allocated - 1;
		if (v >= GCC34T m_Blocks[b].data[i])
		{
			i++;
			return GCC34T m_Allocated;
		}

		Int32 l, u, m, bl, bu, bm;

		bl = 0;
		bu = b;

		while (bl < bu)
		{
			bm = (bl + bu) >> 1;
			if (v >= GCC34T m_Blocks[bm].data[0])
				bl = bm + 1;
			else
				bu = bm - 1;
		}

		if (bl > 0 && v >= GCC34T m_Blocks[bl - 1].data[0] && v <= GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
			bl--;
		else if (v > GCC34T m_Blocks[bl].data[GCC34T m_Blocks[bl].allocated - 1])
			bl++;

		if (bl >= GCC34T m_Blocks.GetCount())
		{
			i = 0;
			b = bl;
			return GCC34T m_Allocated;
		}

		TYPE* data = GCC34T m_Blocks[bl].data;

		l = 1;
		u = GCC34T m_Blocks[bl].allocated - 2;

		if (v <= data[0])
		{
			b = bl;
			i = 0;
			return GCC34T GetBlockIndex(b, i);
		}
		else if (v >= data[u + 1])
		{
			b = bl;
			i = u + 1;
			return GCC34T GetBlockIndex(b, i);
		}

		while (l < u)
		{
			m = (l + u) >> 1;
			if (v >= data[m])
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == data[l - 1])
		{
			l--;
		}
		else if (bl > 0 && v == GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
		{
			bl--;
			l = GCC34T m_Blocks[bl].allocated - 1;
		}
		else if (v > data[l])
		{
			l++;
			if (l > GCC34T m_Blocks[bl].allocated)
			{
				l = 0;
				bl++;
			}
		}

		b = bl;
		i = l;

		return GCC34T GetBlockIndex(b, i);
	}

	Int32 SearchBlockEqualIndex(const TYPE& v, Int32& b, Int32& i)
	{
		if (GCC34T m_Allocated == 0 || v <= GCC34T m_Blocks[0].data[0])
		{
			b = i = 0;
			return 0;
		}

		b = GCC34T m_Blocks.GetCount() - 1;
		i = GCC34T m_Blocks[b].allocated - 1;
		if (v > GCC34T m_Blocks[b].data[i])
		{
			i++;
			return GCC34T m_Allocated;
		}
		if (v == GCC34T m_Blocks[b].data[i])
			return GCC34T m_Allocated - 1;

		Int32 l, u, m, bl, bu, bm;

		bl = 0;
		bu = b;

		while (bl < bu)
		{
			bm = (bl + bu) >> 1;
			if (v >= (*GCC34T m_Blocks[bm].data))
				bl = bm + 1;
			else
				bu = bm - 1;
		}

		if (bl > 0 && v >= GCC34T m_Blocks[bl - 1].data[0] && v <= GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
			bl--;
		else if (v > GCC34T m_Blocks[bl].data[GCC34T m_Blocks[bl].allocated - 1])
			bl++;

		if (bl >= GCC34T m_Blocks.GetCount())
		{
			i = 0;
			b = bl;
			return GCC34T m_Allocated;
		}

		TYPE* data = GCC34T m_Blocks[bl].data;

		l = 1;
		u = GCC34T m_Blocks[bl].allocated - 2;

		if (v <= data[0])
		{
			b = bl;
			i = 0;
			return GCC34T GetBlockIndex(b, i);
		}
		else if (v >= data[u + 1])
		{
			b = bl;
			i = u + 1;
			return GCC34T GetBlockIndex(b, i);
		}

		while (l < u)
		{
			m = (l + u) >> 1;
			if (v >= data[m])
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == data[l - 1])
		{
			l--;
		}
		else if (bl > 0 && v == GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
		{
			bl--;
			l = GCC34T m_Blocks[bl].allocated - 1;
		}
		else if (v > data[l])
		{
			l++;
			if (l > GCC34T m_Blocks[bl].allocated)
			{
				l = 0;
				bl++;
			}
		}

		b = bl;
		i = l;

		return GCC34T GetBlockIndex(b, i);
	}

	Bool SearchBlockIndexMatch(const TYPE& v, Int32& b, Int32& i) const
	{
		if (GCC34T m_Allocated == 0)
			return false;

		Int32 l, u, m, bl, bu, bm;

		bl = 0;
		bu = GCC34T m_Blocks.GetCount() - 1;

		while (bl < bu)
		{
			bm = (bl + bu) >> 1;
			if (v >= (*GCC34T m_Blocks[bm].data))
				bl = bm + 1;
			else
				bu = bm - 1;
		}

		if (bl > 0 && v >= GCC34T m_Blocks[bl - 1].data[0] && v <= GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
			bl--;
		else if (v > GCC34T m_Blocks[bl].data[GCC34T m_Blocks[bl].allocated - 1])
			return false;

		TYPE* data = GCC34T m_Blocks[bl].data;

		l = 0;
		u = GCC34T m_Blocks[bl].allocated - 1;

		if (v < data[0])
			return false;
		else if (v > data[u])
			return false;

		while (l < u)
		{
			m = (l + u) >> 1;
			if (v >= data[m])
				l = m + 1;
			else
				u = m - 1;
		}

		if (l > 0 && v == data[l - 1])
		{
			b = bl;
			i = l - 1;
			return true;
		}
		else if (bl > 0 && v == GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
		{
			b = bl - 1;
			i = GCC34T m_Blocks[bl].allocated - 1;
			return true;
		}
		else if (v == data[l])
		{
			b = bl;
			i = l;
			return true;
		}

		return false;
	}

	Bool SearchBlockIndexMatchFull(const TYPE& v, Int32& b, Int32& i) const
	{
		if (GCC34T m_Allocated == 0)
			return false;

		Int32 l, u, m, bl, bu, bm;

		bl = 0;
		bu = GCC34T m_Blocks.GetCount() - 1;
		l	 = 0;
		u	 = GCC34T m_Blocks[bu].allocated - 1;

		if (bl < bu)
		{
			while ((bl + 1) < bu)
			{
				bm = (bl + bu) >> 1;
				if (v >= (*GCC34T m_Blocks[bm].data))
					bl = bm;
				else
					bu = bm;
			}

			if (v >= GCC34T m_Blocks[bu].data[0])
				bl = bu;

			l = 0;
			u = GCC34T m_Blocks[bl].allocated - 1;
		}

		TYPE* data = GCC34T m_Blocks[bl].data;

		while (l < u)
		{
			m = (l + u) >> 1;
			if (v >= data[m])
				l = m + 1;
			else
				u = m - 1;
		}

		if (v == data[l])
		{
			b = bl;
			i = l;
			return true;
		}
		else if (l > 0 && v == data[l - 1])
		{
			b = bl;
			i = l - 1;
			return true;
		}
		else if (bl > 0 && v == GCC34T m_Blocks[bl - 1].data[GCC34T m_Blocks[bl - 1].allocated - 1])
		{
			b = bl - 1;
			i = GCC34T m_Blocks[bl].allocated - 1;
			return true;
		}

		return false;
	}

public:
	SortedBlockArray()
	{
	}

	Bool Add(const TYPE& data)
	{
		Int32 b, i;

		SearchBlockIndex(data, b, i);

		if (!DynamicBlockArray<TYPE, MINBLOCK, MAXBLOCK>::BlockInsert(b, i))
			return false;
		BlockSet(b, i, data);

		return true;
	}

	const SortedBlockArray<TYPE, MINBLOCK, MAXBLOCK>& operator = (const SortedBlockArray<TYPE, MINBLOCK, MAXBLOCK>& src)
	{
		((SortedBlockArray*)&src)->CopyTo(this);
		return *this;
	}

	Bool AddLast()
	{
		return DynamicBlockArray<TYPE, MINBLOCK, MAXBLOCK>::Add();
	}

	Bool AddLast(const TYPE& v)
	{
		if (!DynamicBlockArray<TYPE, MINBLOCK, MAXBLOCK>::Add())
			return false;
		this->Set(GCC34T m_Allocated - 1, v);
		return true;
	}

	Bool AddUnique(const TYPE& v)
	{
		if (GCC34T m_Allocated > 0)
		{
			Int32 b, i, l;

			l = SearchBlockIndex(v, b, i);
			if (i == GCC34T m_Allocated && GCC34T GetLast() == v)
				return true;
			if (i < GCC34T m_Allocated && GCC34T m_Blocks[b].data[i] == v)
				return true;

			if (!DynamicBlockArray<TYPE, MINBLOCK, MAXBLOCK>::BlockInsert(b, i))
				return false;
			BlockSet(b, i, v);

			return true;
		}
		else
		{
			return Add(v);
		}
	}

	Bool RemoveValue(const TYPE& v)
	{
		Int32 i = Find(v);
		if (i == NOTOK)
			return true;
		return GCC34T Remove(i);
	}

	Int32 Find(const TYPE& v)
	{
		Int32 b, i;
		if (!SearchBlockIndexMatchFull(v, b, i))
			return NOTOK;
		return GCC34T GetBlockIndex(b, i);
	}

	Bool Find(const TYPE& v, Int32& b, Int32& i)
	{
		return SearchBlockIndexMatchFull(v, b, i);
	}

	Int32 FindValueIndex(const TYPE& v)
	{
		if (GCC34T m_Allocated == 0)
			return 0;

		Int32 b, i;
		return SearchBlockEqualIndex(v, b, i);
	}

	Int32 FindValueIndex(const TYPE& v, Int32& b, Int32& i)
	{
		if (GCC34T m_Allocated == 0)
			return 0;
		return SearchBlockEqualIndex(v, b, i);
	}
};

//////////////////////////////////////////////////////////////////////////

#define SELECTBLOCKARRAY_MIN 32
#define SELECTBLOCKARRAY_MAX 512

class SelectBlockArray
{
	class SelectArrayData
	{
public:
		SelectArrayData()
		{
			m_Lower = 0;
			m_Upper = 0;
		}
		SelectArrayData(Int32 index)
		{
			m_Upper = m_Lower = index;
		}
		SelectArrayData(Int32 lower, Int32 upper)
		{
			m_Upper = upper;
			m_Lower = lower;
		}

		Bool operator <(const SelectArrayData& dst) const
		{
			return m_Lower < dst.m_Lower;
		}
		Bool operator >(const SelectArrayData& dst) const
		{
			return m_Lower > dst.m_Upper;
		}
		Bool operator <=(const SelectArrayData& dst) const
		{
			return m_Lower <= dst.m_Upper;
		}
		Bool operator >=(const SelectArrayData& dst) const
		{
			return m_Lower >= dst.m_Lower;
		}
		Bool operator ==(const SelectArrayData& dst) const
		{
			return m_Lower >= dst.m_Lower && m_Lower <= dst.m_Upper;
		}

		SelectArrayData& operator =(const SelectArrayData& src)
		{
			m_Lower = src.m_Lower;
			m_Upper = src.m_Upper;
			return *this;
		}

		Int32 m_Lower;
		Int32 m_Upper;
	};

private:
	SortedBlockArray<SelectArrayData, SELECTBLOCKARRAY_MIN, SELECTBLOCKARRAY_MAX> m_Data;
	Int32 m_SelectCount;

	Int32 m_LastBlockSegment;

	Int32 FindNum(Int32 num);
	Int32 FindBlockNum(Int32 num, Int32& b, Int32& i);

public:
	SelectBlockArray();
	~SelectBlockArray();

	Int GetMemoryUse();
	Int GetMaxCount();

	Int32 GetCount() const;
	Int32 GetSegments() const;
	Bool Select(Int32 num);
	Bool SelectAll(Int32 min, Int32 max);
	Bool Deselect(Int32 num);
	void DeselectAll();
	Bool Toggle(Int32 num);
	Bool ToggleAll(Int32 min, Int32 max);
	Bool GetFirstSelected(Int32* a);
	Bool GetBlockRange(Int32& seg, Int32* va, Int32* vb);
	Bool GetRange(Int32 seg, Int32* a, Int32* b);
	Bool IsSelected(Int32 num);
	Int32 GetSegment(Int32 b, Int32 i, Int32* l, Int32* u) const;
	Bool FindSegment(Int32 num);
	Bool FindBlockSegment(Int32 num, Int32& b, Int32& i) const;
	Bool FindSegment(Int32 num, Int32* segment);
	Bool CopyTo(SelectBlockArray* dst);
	Bool CopyTo(BaseSelect* bs);
	Bool CopyFrom(BaseSelect* bs);
};

class SelectBitArray
{
public:
	SelectBitArray();
	~SelectBitArray();

	Bool Select(Int32 i);
	Bool Deselect(Int32 i);
	Bool SelectAll(Int32 max);
	Bool SelectBlock(Int32 min, Int32 max);
	void DeselectAll();
	Bool IsSelected(Int32 i);
	Bool GetBlockRange(Int32& seg, Int32* va, Int32* vb);
	Int32 GetCount()
	{
		return m_sCount;
	}

	void SetGrowth(UInt32 g);

	void Free();
	Bool Read(HyperFile* hf);
	Bool Write(HyperFile* hf);
	Bool CopyTo(SelectBitArray* dst) const;

	Int32 GetMemoryUse()
	{
		return m_TotalElements * sizeof(UInt32);
	}

	Int32 GetUpper()
	{
		return m_Top;
	}

private:
	Bool SetElements(UInt32 e);
	void GetMaxElement();

	UInt32	m_gCount;
	UInt32	m_sCount;
	UInt32	m_TotalElements;
	UInt32* m_pBits;
	UInt32	m_TopElement;
	Int32		m_Top;
};

class BitArray
{
public:
	BitArray();
	~BitArray();

	Bool Init(Int32 count);
	void Select(Int32 i);
	void Deselect(Int32 i);
	Bool IsSelected(Int32 i);
	Int32 GetCount()
	{
		return m_Count;
	}
	void Free();
	Bool Read(HyperFile* hf);
	Bool Write(HyperFile* hf);
	Bool CopyTo(BitArray* dst) const;
	Int32 GetUsed()
	{
		return m_Used;
	}
	void DeselectAll();
	void SelectAll();
	void ToggleAll();

private:
	Int32		m_Used;
	Int32		m_Count;
	Int32		m_TotalElements;
	UInt32* m_pBits;
};

#pragma pack (pop)
}

#endif	// PRIVATE_GE_MTOOLS_H__
