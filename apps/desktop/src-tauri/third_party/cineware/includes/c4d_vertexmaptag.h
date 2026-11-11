/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_VERTEXMAPTAG_H__
#define C4D_VERTEXMAPTAG_H__

#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Vertex map tag.
//----------------------------------------------------------------------------------------
class VertexMapTag : public BaseTag
{
	INSTANCEOF(VertexMapTag, BaseTag)

private:
	Float32* vdata;
	Int32		 vCount;

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	VertexMapTag();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~VertexMapTag();

	/// @}

	/// @name Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the read-only vertex map array.
	/// @return												The read-only vertex map array. @theOwnsPointed{tag,vertex map array}
	//----------------------------------------------------------------------------------------
	const Float32* GetDataAddressR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable vertex map array.
	/// @return												The writable vertex map array. @theOwnsPointed{tag,vertex map array}
	//----------------------------------------------------------------------------------------
	Float32* GetDataAddressW();

	//----------------------------------------------------------------------------------------
	/// Gets the number of data elements in the variable tag.
	/// @return												The number of data elements.
	//----------------------------------------------------------------------------------------
	Int32 GetDataCount();

	//----------------------------------------------------------------------------------------
	/// Resizes the vertex map data array.
	/// @param[in] newcnt							The new array size.
	/// @return												@trueIfOtherwiseFalse{the vertex map array was resized successfully}
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 newcnt);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_VERTEXMAPTAG_H__
