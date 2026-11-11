/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_POINTOBJECTDATA_H__
#define C4D_POINTOBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_baseselect.h"

namespace cineware
{
#pragma pack (push, 8)

class PrivateChunk;
class HyperFile;
class AliasTrans;

//----------------------------------------------------------------------------------------
/// Node data class for point objects (polygon meshes, splines etc.).
//----------------------------------------------------------------------------------------
class PointObjectData : public NodeData
{
	INSTANCEOF(PointObjectData, NodeData)

	friend class PointObject;

private:
	BaseSelect bs_p;
	BaseSelect hide_p;

protected:
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);
	virtual Int32 GetDiskType() const;
	Bool _ResizeObject(Int32 pcnt);

public:
	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the point object data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The type ID of the point object to read.
	/// @param[in] level							The disklevel of the object to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the point object data to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_POINTOBJECTDATA_H__
