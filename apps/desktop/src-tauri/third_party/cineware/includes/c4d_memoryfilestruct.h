/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_MEMORYFILESTRUCT_H__
#define C4D_MEMORYFILESTRUCT_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

class MemoryFileWriteStruct;

/// @addtogroup MEMMODE
/// @ingroup group_enumeration
/// @{
/// @markPrivate Used internally by MemoryFileStruct.
#define MEMMODE_READ		1
#define MEMMODE_WRITE		2
#define MEMMODE_FREEMEM	4
/// @}

struct MemoryTile
{
	UChar* t_mem;
	Int		 t_size;
};

class MemoryFileWriteStruct
{
private:
	Int32				currenttile_index;
	Int					currenttile_position;
	Int					currenttile_size;

	MemoryTile*	tiles;
	Int32				tile_buffer;

	UChar*			singleblock;

	void Free();

public:
	MemoryFileWriteStruct();
	~MemoryFileWriteStruct();

	Int32 GetBlockCount();
	Bool GetBlock(Int32 index, void*& mem, Int& size);
	Bool GetSingleBlock(void*& mem, Int& size);

	Bool WriteBytes(const void* dst, Int cnt);
};

//----------------------------------------------------------------------------------------
/// This class is used with Filename::SetMemoryWriteMode() to be able to make a BaseFile write to a memory buffer instead of a file.
//----------------------------------------------------------------------------------------
class MemoryFileStruct
{
	friend class BaseFile;
	friend class Filename;
	friend class HyperFile;

private:
	Int32									 t_memmode;	///< @markPrivate @markInternal
	Int										 t_size;		///< @markPrivate @markInternal
	void*									 t_mem;			///< @markPrivate @markInternal
	MemoryFileWriteStruct* t_write;		///< @markPrivate @markInternal

public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	MemoryFileStruct();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~MemoryFileStruct();

	// only for writing

	//----------------------------------------------------------------------------------------
	/// Gets the number of memory blocks.
	/// @return												The number of memory blocks.
	//----------------------------------------------------------------------------------------
	Int32 GetBlockCount();

	//----------------------------------------------------------------------------------------
	/// Gets the memory block specified by @formatParam{index}.
	/// @param[in] index							The memory block index. Must be @em 0 <= @formatParam{index} < GetBlockCount().
	/// @param[in] mem								Assigned the pointer to the memory block.
	/// @param[in] size								Assigned the size of the memory block.
	/// @return												@trueIfOtherwiseFalse{the memory block was successfully accessed}
	//----------------------------------------------------------------------------------------
	Bool GetBlock(Int32 index, void*& mem, Int& size);

	//----------------------------------------------------------------------------------------
	/// Gets a single memory block.
	/// @param[in] mem								Assigned the pointer to the single memory block.
	/// @param[in] size								Assigned the size of the single memory block.
	/// @return												@trueIfOtherwiseFalse{the single memory block was successfully accessed}
	//----------------------------------------------------------------------------------------
	Bool GetSingleBlock(void*& mem, Int& size);

	//----------------------------------------------------------------------------------------
	/// Writes a number of bytes to the structure.
	/// @param[in] src								Pointer to the buffer to write. @callerOwnsPointed{buffer}
	/// @param[in] cnt								The number of bytes to write. Must not exceed the size of the buffer.
	/// @return												@trueIfOtherwiseFalse{the bytes were successfully written}
	//----------------------------------------------------------------------------------------
	Bool WriteBytes(const void* src, Int cnt);

	//----------------------------------------------------------------------------------------
	/// Frees internal data.
	//----------------------------------------------------------------------------------------
	void Free();
};

#pragma pack (pop)
}

#endif	// C4D_MEMORYFILESTRUCT_H__
