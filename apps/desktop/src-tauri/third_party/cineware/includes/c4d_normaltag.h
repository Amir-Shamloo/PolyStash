/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_NORMALTAG_H__
#define C4D_NORMALTAG_H__

#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

/// Handle/pointer to normal data for NormalTag.
typedef void* NormalHandle;

/// Const handle/pointer to normal data for NormalTag.
typedef const void* ConstNormalHandle;

//----------------------------------------------------------------------------------------
/// Represents a polygon vertex normals.
//----------------------------------------------------------------------------------------
struct NormalStruct
{
	Vector a,	///< Vertex normal for the first point in the polygon.
				 b,	///< Vertex normal for the first point in the polygon.
				 c,	///< Vertex normal for the first point in the polygon.
				 d;	///< Vertex normal for the first point in the polygon.
};

//----------------------------------------------------------------------------------------
/// Normal tag. Holds normal data.
//----------------------------------------------------------------------------------------
class NormalTag : public BaseTag
{
	INSTANCEOF(NormalTag, BaseTag)

	friend class PolygonObject;

private:
	UInt16*	udata;
	Int32		nCount;

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	NormalTag();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~NormalTag();

	/// @}

	/// @name Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the size of the normal data array. Same as the number of polygons.
	/// @return												The size of the normal array.
	//----------------------------------------------------------------------------------------
	Int32 GetDataCount();

	//----------------------------------------------------------------------------------------
	/// Resizes the normal data array.
	/// @param[in] newcnt							The new array size.
	/// @return												@trueIfOtherwiseFalse{the normal array was resized successfully}
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 newcnt);

	//----------------------------------------------------------------------------------------
	/// Gets a handle to the read-only Normal data.
	/// @return												The handle to the read-only Normal data. @theOwnsPointed{tag,Normal data}
	//----------------------------------------------------------------------------------------
	ConstNormalHandle GetDataAddressR();

	//----------------------------------------------------------------------------------------
	/// Gets a handle to the writable Normal data.
	/// @return												The handle to the writable Normal data. @theOwnsPointed{tag,Normal data}
	//----------------------------------------------------------------------------------------
	NormalHandle GetDataAddressW();

	//----------------------------------------------------------------------------------------
	/// Gets the point normals for the polygon at index @formatParam{i}.
	/// @param[in] dataPtr						The handle to the normal data. @callerOwnsPointed{normals}
	/// @param[in] i									The index of the polygon to get the point normals for.
	/// @param[out] res								Assigned the point normals for the polygon. @callerOwnsPointed{normal struct}
	//----------------------------------------------------------------------------------------
	static void Get(ConstNormalHandle dataPtr, Int32 i, NormalStruct& res);

	//----------------------------------------------------------------------------------------
	/// Sets the point normals for the polygon at index @formatParam{i}.
	/// @param[in] dataPtr						The handle to the normal data. @callerOwnsPointed{normals}
	/// @param[in] i									The polygon index to set the point normals for.
	/// @param[out] s									The new point normals.
	//----------------------------------------------------------------------------------------
	static void Set(NormalHandle dataPtr, Int32 i, const NormalStruct& s);

	//----------------------------------------------------------------------------------------
	/// Copies point normals.
	/// @param[in] destPtr						The handle to the destination point normal data.
	/// @param[in] dstIndex						The destination polygon index in the destination data @formatParam{destptr}.
	/// @param[in] srcPtr							The handle to the source point normal data.
	/// @param[in] srcIndex						The source polygon index in the source data @formatParam{srcptr}.
	//----------------------------------------------------------------------------------------
	static void Copy(NormalHandle destPtr, Int32 dstIndex, ConstNormalHandle srcPtr, Int32 srcIndex);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

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

#endif	// C4D_NORMALTAG_H__
