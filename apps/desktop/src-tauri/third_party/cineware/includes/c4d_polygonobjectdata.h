/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_POLYGONOBJECTDATA_H__
#define C4D_POLYGONOBJECTDATA_H__

#include "c4d_pointobjectdata.h"

namespace cineware
{
#pragma pack (push, 8)

class NgonBase;

//----------------------------------------------------------------------------------------
/// Node data class for polygonal objects.
//----------------------------------------------------------------------------------------
class PolygonObjectData : public PointObjectData
{
	INSTANCEOF(PolygonObjectData, PointObjectData)

	friend class PolygonObject;

private:
	BaseSelect bs_v;
	BaseSelect hide_v;
	BaseSelect bs_e;
	BaseSelect hide_e;
	BaseSelect phong_e;
	NgonBase*	 m_NgonBase;

	virtual Int32 GetDiskType() const;
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	PolygonObjectData();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~PolygonObjectData();

	/// @}

	/// @name Resize
	/// @{

	//----------------------------------------------------------------------------------------
	/// Changes the number of points and polygons in the object.
	/// @param[in] pcnt								The new point count.
	/// @param[in] vcnt								The new polygon count.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool _ResizeObject(Int32 pcnt, Int32 vcnt);

	//----------------------------------------------------------------------------------------
	/// Changes the number of points, polygons and N-gons.
	/// @param[in] pcnt								The new point count.
	/// @param[in] vcnt								The new polygon count.
	/// @param[in] ncnt								The new N-gon count.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool _ResizeObject(Int32 pcnt, Int32 vcnt, Int32 ncnt);

	/// @}

	/// @name N-gon
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the NgonBase instance for the object.
	/// @return												The N-gon base. @theOwnsPointed{polygon object data,N-gon base}
	//----------------------------------------------------------------------------------------
	NgonBase* _GetNgonBase();

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the polygon object data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The type ID of the polygon object to read.
	/// @param[in] level							The disklevel of the object to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the polygon object data to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_POLYGONOBJECTDATA_H__
