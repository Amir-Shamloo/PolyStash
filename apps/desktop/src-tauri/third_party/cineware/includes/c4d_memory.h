/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_MEMORY_H__
#define C4D_MEMORY_H__

#ifdef MAXON_TARGET_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4986)	// std::set_new_handler: exception specification does not match previous declaration
#endif

#include <new>

#if defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS)
using namespace std;
#elif !defined MAXON_TARGET_WINDOWS
	#include <stddef.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup group_c4d_memory_memoryallocation Memory Allocation
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
void* MemAllocNC(Int size);

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
void* MemAlloc(Int size);

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
void* MemRealloc(void* orimem, Int size);

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
void MemFree(void*& mem);

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
inline void _MemFree(void** mem)
{
	MemFree(*mem);
}

//----------------------------------------------------------------------------------------
/// Allocates raw memory and clear it.
/// @warning	No constructors are called!
/// @param[in] t									Type of item (e.g. Char, Float).
/// @param[in] x									Number of items. THREADSAFE.
/// @return												Pointer to the memory block, or @formatConstant{nullptr} if allocation failed. @callerOwnsPointed{memory}
//----------------------------------------------------------------------------------------
#define NewMemClear(t, x) (t*)MemAlloc(sizeof(t) * (x))

//----------------------------------------------------------------------------------------
/// Allocates raw memory.
/// @warning	No constructors are called!
/// @param[in] t									Type of item (e.g. Char, Float).
/// @param[in] x									Number of items. THREADSAFE.
/// @return												Pointer to the memory block, or @formatConstant{nullptr} if allocation failed. @callerOwnsPointed{memory}
//----------------------------------------------------------------------------------------
#define NewMem(t, x) (t*)MemAllocNC(sizeof(t) * (x))

//----------------------------------------------------------------------------------------
/// Resizes raw memory.
/// @warning	No constructors are called!
/// @param[in] t									Type of item (e.g. Char, Float).
/// @param[in] p									Pointer to the previous memory block. Can be @formatConstant{nullptr}. @callerOwnsPointed{memory}
/// @param[in] s									New number of items. New size of the memory block is @formatParam{s} * sizeof(@formatParam{t})) THREADSAFE.
/// @return												Pointer to the memory block, or @formatConstant{nullptr} if resize failed. @callerOwnsPointed{memory}
//----------------------------------------------------------------------------------------
#define ReallocMemType(t, p, s) (t*)MemRealloc((p), sizeof(t) * (s))

//----------------------------------------------------------------------------------------
/// Frees raw memory block.
/// @warning	No destructors are called!
//----------------------------------------------------------------------------------------
#define DeleteMem(x) _MemFree((void**) &(x))

//----------------------------------------------------------------------------------------
/// Copies a block of memory.
/// @warning	Parameter order is the opposite of memcpy()!
/// @param[in] s									Pointer to the source block of memory. @callerOwnsPointed{memory}
/// @param[in] d									Pointer to the destination block of memory. @callerOwnsPointed{memory}
/// @param[in] size								Size in bytes of the block of memory to copy.
//----------------------------------------------------------------------------------------
inline void CopyMem(const void* s, void* d, Int size)
{
	if (s && d && size)
		memcpy(d, s, size);
}

//----------------------------------------------------------------------------------------
/// Clears a block of memory.
/// @param[in] d									Pointer to the block of memory to clear. @callerOwnsPointed{memory}
/// @param[in] size								Size in bytes of the block of memory to clear.
/// @param[in] value							Value to clear the memory block with.
//----------------------------------------------------------------------------------------
inline void ClearMem(void* d, Int size, Int32 value = 0)
{
	memset(d, value & 0xFF, size);
}

//----------------------------------------------------------------------------------------
/// Compares two blocks of memory.
/// @note	Implementation call memcmp(@formatParam{s}, @formatParam{d}, @formatParam{size}).
/// @param[in] s									First block of memory. @callerOwnsPointed{memory}
/// @param[in] d									Second block of memory. @callerOwnsPointed{memory}
/// @param[in] size								Number of bytes to compare.
/// @return												@em 0 if the contents of both memory blocks are equal.\n
///																A value greater than @em 0 to indicate that the first byte that does not match in both memory blocks has a greater value in @formatParam{s} than in @formatParam{d} as if evaluated as unsigned char values.\n
///																A value less than @em 0 to indicate that the first byte that does not match in both memory blocks has a less value in @formatParam{s} than in @formatParam{d} as if evaluated as unsigned char values.
///																@note	Same return value as @c memcmp().
//----------------------------------------------------------------------------------------
inline Int CompareMem(const void* s, const void* d, Int size)
{
	return memcmp(s, d, size);
}

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
template <class T, class U> inline void ClearMemTypeTemplate(T* data_ptr, Int size, const U* check_type)
{
#ifdef MAXON_TARGET_DEBUG
	if (check_type != data_ptr)	// this will cause a compile time error if the types are different
		return;
#endif

	memset(data_ptr, 0, size);
}

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
template <class T, class U> inline void FillMemTypeTemplate(T* data_ptr, Int size, const U* check_type, Int32 value)
{
#ifdef MAXON_TARGET_DEBUG
	if (check_type != data_ptr)	// this will cause a compile time error if the types are different
		return;
#endif

	memset(data_ptr, value, size);
}

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
template <class T, class U> inline void CopyMemTypeTemplate(const T* src_ptr, T* dst_ptr, Int size, const U* check_type)
{
#ifdef MAXON_TARGET_DEBUG
	if (check_type != dst_ptr)	// this will cause a compile time error if the types are different
		return;
#endif

	if (src_ptr && dst_ptr && size)
		memcpy(dst_ptr, src_ptr, size);
}

//----------------------------------------------------------------------------------------
/// Clears a block of memory of the specified type.
/// @param[in] t									Data type (e.g. Char, Float).
/// @param[in] d									Pointer to the block of memory to clear. @callerOwnsPointed{memory}
/// @param[in] x									Size of the memory block to clear.
//----------------------------------------------------------------------------------------
#define ClearMemType(t, d, x) ClearMemTypeTemplate(d, sizeof(t) * (x), ((t*) d))

//----------------------------------------------------------------------------------------
/// Fills a block of memory of the specified type.
/// @param[in] t									Data type (e.g. Char, Float).
/// @param[in] d									Pointer to the block of memory to fill. @callerOwnsPointed{memory}
/// @param[in] x									Size of the memory block to fill.
/// @param[in] v									Value to fill the memory block with.
//----------------------------------------------------------------------------------------
#define FillMemType(t, d, x, v) FillMemTypeTemplate(d, sizeof(t) * (x), ((t*) d), v)

//----------------------------------------------------------------------------------------
/// Copies a block of memory of the specified type.
/// @param[in] t									Data type (e.g. Char, Float).
/// @param[in] s									Pointer to the source block of memory. @callerOwnsPointed{memory}
/// @param[in] d									Pointer to the destination block of memory. @callerOwnsPointed{memory}
/// @param[in] x									Number of elements to copy.
//----------------------------------------------------------------------------------------
#define CopyMemType(t, s, d, x) CopyMemTypeTemplate(s, d, sizeof(t) * (x), ((t*) d))

/// @}

#pragma pack (pop)
}	// end namespace cineware

#ifdef MAXON_TARGET_WINDOWS
	#pragma warning(pop)
#endif

#endif	// C4D_MEMORY_H__
