/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SPLINEOBJECT_H__
#define C4D_SPLINEOBJECT_H__

#include "c4d_pointobject.h"

namespace cineware
{
#pragma pack (push, 8)

struct Tangent;

//----------------------------------------------------------------------------------------
/// Spline segment data.
//----------------------------------------------------------------------------------------
struct Segment
{
	Int32	cnt;		///< The number of points in the segment.
	Bool	closed;	///< @formatConstant{true} if the segment is closed.
};

//----------------------------------------------------------------------------------------
/// Represents @C4D's spline object.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class SplineObject : public PointObject
{
	INSTANCEOF(SplineObject, PointObject)

	friend class BaseObject;

private:
	virtual Int32 GetDiskType() const;
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

protected:
	SplineObject();

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{spline object}
	/// @param[in] pcnt								The point count.
	/// @param[in] type								The spline type.
	/// @return												@allocReturn{spline object}
	//----------------------------------------------------------------------------------------
	static SplineObject* Alloc(Int32 pcnt, SPLINETYPE type);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{spline objects}
	/// @param[in,out] bl							@theToDestruct{spline object}
	//----------------------------------------------------------------------------------------
	static void Free(SplineObject*& bl);

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object type for a SplineObject.
	/// @note	Always returns @ref Ospline.
	/// @return												The object type for a SplineObject i.e. @ref Ospline.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const
	{
		return Ospline;
	}

	/// @}

	/// @name Interpolation Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the spline interpolation type.
	/// @return												The spline interpolation type: @ref SPLINEOBJECT_INTERPOLATION.
	//----------------------------------------------------------------------------------------
	Int32 GetInterpolationType();

	//----------------------------------------------------------------------------------------
	/// Sets the spline interpolation type.
	/// @param[in] i									The spline interpolation type to set: @ref SPLINEOBJECT_INTERPOLATION.
	//----------------------------------------------------------------------------------------
	void SetInterpolationType(Int32 i);

	/// @}

	/// @name Spline Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the spline type.
	/// @return												The spline type.
	//----------------------------------------------------------------------------------------
	SPLINETYPE GetSplineType();

	//----------------------------------------------------------------------------------------
	/// Sets the spline type.
	/// @param[in] t									The spline type to set.
	//----------------------------------------------------------------------------------------
	void SetSplineType(SPLINETYPE t);

	/// @}

	/// @name Closed
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if the spline is closed.
	/// @return												@trueIfOtherwiseFalse{the spline is closed}
	//----------------------------------------------------------------------------------------
	Bool GetIsClosed();

	//----------------------------------------------------------------------------------------
	/// Sets if a spline is closed.
	/// @param[in] b									@formatConstant{true} for closed splines, @formatConstant{false} for open splines.
	//----------------------------------------------------------------------------------------
	void SetIsClosed(Bool b);

	/// @}

	/// @name Resize
	/// @{

	//----------------------------------------------------------------------------------------
	/// Changes the number of segments and points.
	/// @param[in] pcnt								The new point count.
	/// @param[in] scnt								The new segment count.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool ResizeObject(Int32 pcnt, Int32 scnt);

	/// @}

	/// @name Segment
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the read-only segments buffer.
	/// @return												A pointer to the read-only segments buffer. @theOwnsPointed{spline object,buffer}
	//----------------------------------------------------------------------------------------
	const Segment* GetSegmentR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable segments buffer.
	/// @return												A pointer to the writable segments buffer. @theOwnsPointed{spline object,buffer}
	//----------------------------------------------------------------------------------------
	Segment* GetSegmentW();

	//----------------------------------------------------------------------------------------
	/// Gets the number of segments.
	/// @return												The number of segments.
	//----------------------------------------------------------------------------------------
	Int32 GetSegmentCount();

	/// @}

	/// @name Tangent
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the read-only tangents buffer.
	/// @return												A pointer to the read-only tangents buffer. @theOwnsPointed{spline object,buffer}
	//----------------------------------------------------------------------------------------
	const Tangent* GetTangentR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable tangents buffer.
	/// @return												A pointer to the writable tangents buffer. @theOwnsPointed{spline object,buffer}
	//----------------------------------------------------------------------------------------
	Tangent* GetTangentW();

	//----------------------------------------------------------------------------------------
	/// Gets the number of tangents.
	/// @return												The number of tangents.
	//----------------------------------------------------------------------------------------
	Int32 GetTangentCount();

	/// @}

	/// @name Execute
	/// @{

	//----------------------------------------------------------------------------------------
	/// Overload this for self defined spline objects.
	/// @return												@trueIfOtherwiseFalse{the spline object type is supported by the external application}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_SPLINEOBJECT_H__
