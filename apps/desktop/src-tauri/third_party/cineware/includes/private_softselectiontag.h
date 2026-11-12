/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_SOFTSELECTIONTAG_H__
#define PRIVATE_SOFTSELECTIONTAG_H__

#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
/// Represents the SoftSelection tag for (temporary) soft selections.
//----------------------------------------------------------------------------------------
class SoftSelectionTag : public BaseTag
{
	INSTANCEOF(SoftSelectionTag, BaseTag)

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	SoftSelectionTag();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~SoftSelectionTag();

	/// @}

public:
	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the read-only soft selection array.
	/// @return												The read-only soft selection array. @theOwnsPointed{tag,soft selection array}
	//----------------------------------------------------------------------------------------
	const Float32* GetDataAddressR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable soft selection array.
	/// @return												The writable soft selection array. @theOwnsPointed{tag,soft selection array}
	//----------------------------------------------------------------------------------------
	Float32* GetDataAddressW();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Int32 GetDataCount();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 newCount);

private:
	Float32* sData;
	Int32		 sCount;

	/// @name Private
	/// @{

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// PRIVATE_SOFTSELECTIONTAG_H__
