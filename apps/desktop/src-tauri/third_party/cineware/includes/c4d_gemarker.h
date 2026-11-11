/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_GEMARKER_H__
#define C4D_GEMARKER_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;

//----------------------------------------------------------------------------------------
/// A unique marker that identifies an object derived from one of the base lists.
//----------------------------------------------------------------------------------------
class GeMarker
{
private:
	UChar uniqueid[16];

public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	GeMarker();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{marker}
	/// @return												@allocReturn{marker}
	//----------------------------------------------------------------------------------------
	static GeMarker* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{markers}
	///
	/// @param[in,out] obj						@theToDestruct{marker}
	//----------------------------------------------------------------------------------------
	static void Free(GeMarker*& obj);

	//----------------------------------------------------------------------------------------
	/// Checks if the marker is empty.
	/// @return												@trueIfOtherwiseFalse{the marker has been set}
	//----------------------------------------------------------------------------------------
	Bool Content() const;

	//----------------------------------------------------------------------------------------
	/// Checks if two markers are equal.
	/// @param[in] m									The marker to compare with.
	/// @return												@trueIfOtherwiseFalse{if both markers are equal}
	//----------------------------------------------------------------------------------------
	Bool IsEqual(const GeMarker& m) const;

	//----------------------------------------------------------------------------------------
	/// Compares two markers. Basically it works like @em memcmp().
	/// @param[in] m									The marker to compare with.
	/// @return												@em 0 if the markers are identical and != @em 0 if different.
	//----------------------------------------------------------------------------------------
	Int32 Compare(const GeMarker& m) const;

	//----------------------------------------------------------------------------------------
	/// Sets a marker the same as another marker.
	/// @param[in] m									The new marker.
	//----------------------------------------------------------------------------------------
	void Set(const GeMarker& m);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf) const;

	//----------------------------------------------------------------------------------------
	/// Gets a memory pointer and the size of the memory block. Used for instance to create a checksum.
	/// @note The memory must not be freed.
	/// @param[in] data								The memory pointer.
	/// @param[in] size								The size of the memory block.
	//----------------------------------------------------------------------------------------
	void GetMemory(void*& data, Int32& size) const;

	//----------------------------------------------------------------------------------------
	/// Gets a new marker.
	/// @return												A new marker.
	//----------------------------------------------------------------------------------------
	friend GeMarker GeGetNewMarker();

	//----------------------------------------------------------------------------------------
	/// Equality operator.
	/// @param[in] m1									Left operand.
	/// @param[in] m2									Right operand.
	/// @return												@trueIfOtherwiseFalse{markers are equal}
	//----------------------------------------------------------------------------------------
	friend Bool operator == (const GeMarker& m1, const GeMarker& m2);

	//----------------------------------------------------------------------------------------
	/// Inequality operator.
	/// @param[in] m1									Left operand.
	/// @param[in] m2									Right operand.
	/// @return												@trueIfOtherwiseFalse{markers are not equal}
	//----------------------------------------------------------------------------------------
	friend Bool operator != (const GeMarker& m1, const GeMarker& m2);

	//----------------------------------------------------------------------------------------
	/// Inferior operator.
	/// @param[in] m1									Left operand.
	/// @param[in] m2									Right operand.
	/// @return												@trueIfOtherwiseFalse{Left operand marker is inferior to right operand marker}
	//----------------------------------------------------------------------------------------
	friend Bool operator < (const GeMarker& m1, const GeMarker& m2);

	//----------------------------------------------------------------------------------------
	/// Superior operator.
	/// @param[in] m1									Left operand.
	/// @param[in] m2									Right operand.
	/// @return												@trueIfOtherwiseFalse{Left operand marker is superior to right operand marker}
	//----------------------------------------------------------------------------------------
	friend Bool operator > (const GeMarker& m1, const GeMarker& m2);
};

//----------------------------------------------------------------------------------------
/// Gets a new marker.
/// @return												A new marker.
//----------------------------------------------------------------------------------------
GeMarker GeGetNewMarker();

#pragma pack (pop)
}

#endif	// C4D_GEMARKER_H__
