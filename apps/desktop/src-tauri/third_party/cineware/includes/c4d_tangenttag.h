/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_TANGENTTAG_H__
#define C4D_TANGENTTAG_H__

#include "ge_math.h"
#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a tangent with its left and right interpolation parts.
//----------------------------------------------------------------------------------------
struct Tangent
{
	Vector vl,	///< Left part that defines the tangent.
				 vr;	///< Right part that defines the tangent.
};

//----------------------------------------------------------------------------------------
/// Tangent tag. Holds tangent data.
//----------------------------------------------------------------------------------------
class TangentTag : public BaseTag
{
	INSTANCEOF(TangentTag, BaseTag)

	Tangent*	m_pTranslatedTangents;
	Int32			m_count;

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	TangentTag();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~TangentTag();

	/// @}

	/// @name Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the size of the tangent data array.
	/// @return												The size of the tangent array.
	//----------------------------------------------------------------------------------------
	Int32 GetDataCount();

	//----------------------------------------------------------------------------------------
	/// Sets the size of the tangent data array.
	/// @param[in] count							The new array size.
	/// @return												@trueIfOtherwiseFalse{the size could be set}
	//----------------------------------------------------------------------------------------
	Bool SetDataCount(Int32 count);

	//----------------------------------------------------------------------------------------
	/// Resizes the tangent data array.
	/// @param[in] newcnt							The new array size.
	/// @return												@trueIfOtherwiseFalse{the tangent array was resized successfully}
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 newcnt);

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the readable tangent array.
	/// @return												The pointer to the readable tangent array. @theOwnsPointed{tag, tangent array}
	//----------------------------------------------------------------------------------------
	const Tangent* GetDataR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable tangent array.
	/// @return												The pointer to the writable tangent array. @theOwnsPointed{tag, tangent array}
	//----------------------------------------------------------------------------------------
	Tangent* GetDataW();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_TANGENTTAG_H__
