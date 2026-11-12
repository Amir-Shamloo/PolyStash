/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DEFAULTALLOCATOR_H__
#define C4D_DEFAULTALLOCATOR_H__

#include <stdlib.h>
#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

// Can't include c4d_memory.h because this would create a circular dependency, therefore use the following helpers
void* _DefaultAllocNC(Int size);
void* _DefaultAlloc(Int size);
void*	_DefaultReallocNC(void* data, Int size);
void _DefaultFree(void** data_ptr);

//----------------------------------------------------------------------------------------
/// Default implementation of an allocator.
/// An allocator is used by arrays, lists and other data structurs to allocate and release
/// memory. By default this implementation of an allocator is used.
///
/// There might be rare cases when you need a special memory alignment, a different resize
/// strategy or have to use a special memory area (stack, shared memory, ...). This can be
/// done by writing a custom allocator and specifying it as parameter upon array construction.
/// A custom allocator must implement the ComputeArraySize(), Alloc(), AllocClear(), Realloc()
/// and Free() methods, but it doesn't (and usually shouldn't) inherit from DefaultAllocator.
/// The allocator methods don't have to be static if your allocator requires member
/// variables, but the DefaultAllocator doesn't and therefore uses static methods for
/// better performance.
///
/// Please note that an allocator is copied upon array construction - it would be
/// a bad idea if your custom allocator object would consist of more than a few variables.
///
/// THREADSAFE
//----------------------------------------------------------------------------------------
class DefaultAllocator
{
public:
	//----------------------------------------------------------------------------------------
	/// Computes the new size for a growing array.
	/// @param[in] current_size				Current number of elements.
	/// @param[in] increment					Number of elements to be added (>= 1)
	/// @param[in] min_chunk_size			The minimum number of elements upon array creation.
	/// THREADSAFE.
	/// @return												New number of elements.
	//----------------------------------------------------------------------------------------
	static Int ComputeArraySize(Int current_size, Int increment, Int min_chunk_size)
	{
		Int new_size	= (current_size + increment) * 3 / 2;	// increase required size by 50 % for future allocations
		Int remainder = new_size % min_chunk_size;
		if (remainder || new_size == 0)
			new_size += min_chunk_size - remainder;

		return new_size;
	}

	//----------------------------------------------------------------------------------------
	/// Allocates a memory block.
	/// The memory is not cleared, it may contain a certain byte pattern in debug mode.
	/// @param[in] s									Block size in bytes (values < 0 will return nullptr)
	/// THREADSAFE.
	/// @return												Memory block address or nullptr.
	//----------------------------------------------------------------------------------------
	static inline void* Alloc(Int32 s)
	{
		return _DefaultAllocNC((Int)s);
	}

	//----------------------------------------------------------------------------------------
	/// Allocates a memory block.
	/// The memory is not cleared, it may contain a certain byte pattern in debug mode.
	/// @param[in] s									Block size in bytes (values < 0 will return nullptr)
	/// THREADSAFE.
	/// @return												Memory block address or nullptr.
	//----------------------------------------------------------------------------------------
	static inline void* Alloc(Int64 s)
	{
		//		if (s != Int(s)) { DebugStop(); return nullptr; }
		return _DefaultAllocNC((Int)s);
	}

	//----------------------------------------------------------------------------------------
	/// Allocates a memory block and clears it.
	/// @param[in] s									Block size in bytes (values < 0 will return nullptr)
	/// THREADSAFE.
	/// @return												Memory block address or nullptr.
	//----------------------------------------------------------------------------------------
	static inline void* AllocClear(Int32 s)
	{
		return _DefaultAlloc((Int)s);
	}

	//----------------------------------------------------------------------------------------
	/// Allocates a memory block and clears it.
	/// @param[in] s									Block size in bytes (values < 0 will return nullptr)
	/// THREADSAFE.
	/// @return												Memory block address or nullptr.
	//----------------------------------------------------------------------------------------
	static inline void* AllocClear(Int64 s)
	{
		//		if (s != Int(s)) { DebugStop(); return nullptr; }
		return _DefaultAlloc((Int)s);
	}

	//----------------------------------------------------------------------------------------
	/// Resizes a memory block.
	/// The additional memory is not cleared, it may contain a certain byte pattern in debug mode.
	/// @param[in] p									Current memory block (can be nullptr)
	/// @param[in] n									New block size in bytes (values < 0 will return nullptr)
	/// THREADSAFE.
	/// @return												Memory block address or nullptr if resize is not possible (p is still valid in this case)
	//----------------------------------------------------------------------------------------
	static inline void* Realloc(void* p, Int n)
	{
		return _DefaultReallocNC(p, (Int)n);
	}

	//----------------------------------------------------------------------------------------
	/// Frees a memory block.
	/// @param[in,out] p							Memory block address (can be nullptr, will be nullptr after return)
	/// THREADSAFE
	//----------------------------------------------------------------------------------------
	template <class T> static inline void Free(T*& p)
	{
		void*	tmp = (void*) p;
		_DefaultFree(&tmp);
		p = nullptr;
	}
};

//----------------------------------------------------------------------------------------
/// Deletes an object. This calls the destructor and frees memory afterwards.
/// Normally you should use DeleteObj(). This variant should be used only in the rare case
/// when the object pointer is a constant so that it can't be set to nullptr.
/// @param[in] o									Object pointer (can be nullptr)
/// THREADSAFE
//----------------------------------------------------------------------------------------
template <typename T> inline void DeleteConstPtrObj(T* o)
{
	if (o)
	{
		o->~T();
		DefaultAllocator::Free(o);
	}
}

#pragma pack (pop)
}

#endif	// C4D_DEFAULTALLOCATOR_H__
