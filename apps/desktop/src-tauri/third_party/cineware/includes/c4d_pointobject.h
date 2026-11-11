/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_POINTOBJECT_H__
#define C4D_POINTOBJECT_H__

#include "c4d_baseobject.h"
#include "c4d_baseselect.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a point object. Abstract; instantiate either PolygonObject or SplineObject.
//----------------------------------------------------------------------------------------
class PointObject : public BaseObject
{
	INSTANCEOF(PointObject, BaseObject)

	friend class BaseObject;

private:
	BaseSelect bs_p;
	BaseSelect hide_p;

	virtual Int32 GetDiskType() const;

protected:
	PointObject();

public:
	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object type for a PointObject.
	/// @note	Always returns @ref Opoint.
	/// @return												The object type for a PointObject i.e. @ref Opoint.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const
	{
		return Opoint;
	}

	/// @}

	/// @name Point
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the selected points.
	/// @return												The selected points. @theOwnsPointed{point object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetPointS();

	//----------------------------------------------------------------------------------------
	/// Gets the hidden points.
	/// @return												The hidden points. @theOwnsPointed{point object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetPointH();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the read-only point buffer.
	/// @return												A pointer to the read-only point buffer. @theOwnsPointed{point object,buffer}
	//----------------------------------------------------------------------------------------
	const Vector* GetPointR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable point buffer.
	/// @return												A pointer to the point buffer. @theOwnsPointed{point object,buffer}
	//----------------------------------------------------------------------------------------
	Vector* GetPointW();

	//----------------------------------------------------------------------------------------
	/// Gets the number of points.
	/// @return												The number of points.
	//----------------------------------------------------------------------------------------
	Int32 GetPointCount();

	//----------------------------------------------------------------------------------------
	/// Resizes the object's point buffer.
	/// @param[in] pcnt								The new number of points.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool ResizeObject(Int32 pcnt);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_POINTOBJECT_H__
