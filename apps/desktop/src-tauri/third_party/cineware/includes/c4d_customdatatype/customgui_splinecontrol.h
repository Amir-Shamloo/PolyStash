/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_SPLINECONTROL_H__
#define CUSTOMGUI_SPLINECONTROL_H__

#include "c4d_customdatatype.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

/// Spline data type ID.
#define CUSTOMDATATYPE_SPLINE 1009060

/// @addtogroup FLAG_KNOT
/// @ingroup group_enumeration
/// @{
#define FLAG_KNOT_T_BREAK						(1 << 4)	///< Breaks knot tangents.
#define FLAG_KNOT_LOCK_X						(1 << 5)	///< Locks knot's X movement.
#define FLAG_KNOT_LOCK_Y						(1 << 6)	///< Locks knot's Y movement.
#define FLAG_KNOT_T_LOCK_A					(1 << 7)	///< Locks knot's tangent angle.
#define FLAG_KNOT_T_LOCK_L					(1 << 8)	///< Locks knot's tangent length.
#define FLAG_KNOT_T_KEEPVISUALANGLE	(1 << 9)	///< Keeps visual angle between knot's tangents.
#define ADD_KNOT_ADAPT_TANGENTS			(1 << 16)	///< Tangents of new knots are set in such a way that the resulting spline will match the curvature of the original spline.
/// @}

/// Gets the ID of a CustomSplineKnot. Usage:
/// @code
/// Int32 id = SPLINE_KNOT_GET_ID(knot.lFlags);
/// @endcode
#define SPLINE_KNOT_GET_ID(flags) ((flags >> 16) & 0x0000ffff)

/// Sets the ID of a CustomSplineKnot. Usage:
/// @code
/// SPLINE_KNOT_SET_ID(knot.lFlags, id);
/// @endcode
#define SPLINE_KNOT_SET_ID(flags, flag_id) (flags = (flags & 65535) | ((flag_id & 0x0000ffff) << 16))

/// @addtogroup CustomSplineKnotInterpolation
/// @ingroup group_enumeration
/// @{
/// Spline knot interpolation types.
enum CustomSplineKnotInterpolation
{
	CustomSplineKnotInterpolationBezier,	///< Bezier interpolation.
	CustomSplineKnotInterpolationLinear,	///< Linear interpolation.
	CustomSplineKnotInterpolationCubic		///< Cubic interpolation.
};
/// @}

//----------------------------------------------------------------------------------------
/// Represents a knot in a SplineData.
//----------------------------------------------------------------------------------------
struct CustomSplineKnot
{
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	CustomSplineKnot()
	{
		bSelect	 = false;
		interpol = CustomSplineKnotInterpolationBezier;
	}

	Vector vPos;																	///< Knot position.
	Int32	 lFlagsSettings;												///< Knot flags: @ref FLAG_KNOT
																								///< @note	The flags must be accessed using |= and &=. Also contains the point ID. \n
																								///<				The knot ID must only be retrieved using the helpers SPLINE_KNOT_GET_ID() and SPLINE_KNOT_SET_ID().
	Bool													bSelect;				///< Internal select state. Do not change this.
	Vector												vTangentLeft,		///< The left tangent.
																vTangentRight;	///< The right tangent.
	CustomSplineKnotInterpolation interpol;				///< Spline knot's interpolation.

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Equality operator. Checks if two knots are equal.
	/// @param[in] k									A spline knot.
	/// @return												@trueIfOtherwiseFalse{the knots are equal}
	//----------------------------------------------------------------------------------------
	Bool operator == (CustomSplineKnot& k);

	//----------------------------------------------------------------------------------------
	/// Inequality operator. Checks if two knots are different.
	/// @param[in] k									A spline knot.
	/// @return												@trueIfOtherwiseFalse{the knots are different}
	//----------------------------------------------------------------------------------------
	Bool operator != (CustomSplineKnot& k);

	/// @}
};

enum
{
	SPLINE_TYPE_SOFT = 0,	///< Soft.
	SPLINE_TYPE_EASEIN,		///< Ease in.
	SPLINE_TYPE_EASEOUT,	///< Ease out.
	SPLINE_TYPE_EASEEASE,	///< Ease ease.
	SPLINE_TYPE_ZERO,			///< Zero angle/length.
	SPLINE_TYPE_UNIFY			///< Unify.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/// Spline data type (@ref CUSTOMDATATYPE_SPLINE).
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class SplineData : public CustomDataType
{
public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{spline data}
	/// @return												@allocReturn{spline data}
	//----------------------------------------------------------------------------------------
	static SplineData* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{spline data}
	/// @param[in,out] pData					@theToDestruct{spline data}
	//----------------------------------------------------------------------------------------
	static void Free(SplineData*& pData);

	/// @}

	/// @name Knot
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of knots.
	/// @return												The knot count.
	//----------------------------------------------------------------------------------------
	Int32 GetKnotCount() const;

	//----------------------------------------------------------------------------------------
	/// Gets the spline knot at position @formatParam{l}.
	/// @param[in] l									The position index. @em 0 <= @formatParam{l} < GetKnotCount()
	/// @return												The spline knot.
	//----------------------------------------------------------------------------------------
	CustomSplineKnot* GetKnot(Int32 l);

	//----------------------------------------------------------------------------------------
	/// Sets the spline knot at position @formatParam{l}.
	/// @param[in] l									The knot index. @em 0 <= @formatParam{l} < GetKnotCount()
	/// @param[in] knot								The spline knot.
	/// @return												@trueIfOtherwiseFalse{the spline knot was successfully set}
	//----------------------------------------------------------------------------------------
	Bool SetKnot(Int32 l, const CustomSplineKnot& knot);

	//----------------------------------------------------------------------------------------
	/// Inserts a knot.
	/// @param[in] x									The X coordinate.
	/// @param[in] y									The Y coordinate.
	/// @param[in] lFlagsSettings			The knot flags: @ref FLAG_KNOT.
	/// @return												The index of the inserted knot.
	//----------------------------------------------------------------------------------------
	Int32 InsertKnot(Float x, Float y, Int32 lFlagsSettings = 0);

	//----------------------------------------------------------------------------------------
	/// Deletes a knot.
	/// @param[in] a									The knot index to delete. @em 0 <= @formatParam{a} < GetKnotCount()
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool DeleteKnot(Int32 a);

	//----------------------------------------------------------------------------------------
	/// Sorts the knots.
	/// @param[out] lActiveKnot				Assigned the new index of the active knot.
	//----------------------------------------------------------------------------------------
	void SortKnots(Int32& lActiveKnot);

	//----------------------------------------------------------------------------------------
	/// Gets the number of selected knots.
	/// @return												The selected knot count.
	//----------------------------------------------------------------------------------------
	Int32 GetSelectCount() const;

	//Vector GetPoint(Float r) const;

	//----------------------------------------------------------------------------------------
	/// Deletes all points.
	//----------------------------------------------------------------------------------------
	void DeleteAllPoints();

	/// @}

	/// @name Make Spline
	/// @{

	//----------------------------------------------------------------------------------------
	/// Makes an uninitialized spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakePointBuffer(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes a linear spline with @formatParam{lPoints} points. The spline knots' is linear.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeLinearSplineLinear(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes a linear spline with @formatParam{lPoints} points. The spline knots' interpolation is Bezier.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeLinearSplineBezier(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes a square spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeSquareSpline(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes a cubic spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeCubicSpline(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes a root spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeRootSpline(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes an inverse spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeInversSpline(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes a sinus spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeSinSpline(Int32 lPoints = -1);

	//----------------------------------------------------------------------------------------
	/// Makes an absolute cosinus spline with @formatParam{lPoints} points.
	/// @param[in] lPoints						The number of points, or @em -1 to get the default value.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool MakeAbsCosSpline(Int32 lPoints = -1);

	//Bool MakeUserSpline(const String &str, Int32 lPoints = -1);

	/// @}

	/// @name Spline Knots Manipulation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Selects all points.
	//----------------------------------------------------------------------------------------
	void SelectAll();

	//----------------------------------------------------------------------------------------
	/// Flips the spline.
	//----------------------------------------------------------------------------------------
	void Flip();

	//----------------------------------------------------------------------------------------
	/// Mirrors the spline.
	//----------------------------------------------------------------------------------------
	void Mirror();

	//----------------------------------------------------------------------------------------
	/// Makes all Y coordinates less than or equal to @em 1.
	//----------------------------------------------------------------------------------------
	void Maximum();

	//----------------------------------------------------------------------------------------
	/// Makes all Y coordinates greater than or equal to @em 0.
	//----------------------------------------------------------------------------------------
	void Minimum();

	/// @}

	/// @name Set Type/Zero
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the interpolation type of the selected knots.
	/// @param[in] id									The interpolation type: ::CustomSplineKnotInterpolation.
	/// @param[in] bAll								@formatConstant{true} to set all knots, @formatConstant{false} to set selected knots only.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetType(Int32 id, Bool bAll = false);

	//----------------------------------------------------------------------------------------
	/// Sets the tangents of the selected knots to zero.
	/// @param[in] bY									@formatConstant{true} to set the tangents' Y coordinate to zero, @formatConstant{false} to set the X coordinate.
	/// @param[in] bAll								@formatConstant{true} to set all tangents, @formatConstant{false} to set the tangents of selected knots only.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetZero(Bool bY, Bool bAll = false);

	/// @}

	/// @name Range
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the range of the spline.
	/// @param[out] xmin							The minimum X value.
	/// @param[out] xmax							The maximum X value.
	/// @param[out] xsteps						The X steps value.
	/// @param[out] ymin							The minimum Y value.
	/// @param[out] ymax							The maximum Y value.
	/// @param[out] ysteps						The Y steps value.
	/// @return												@trueIfOtherwiseFalse{the range of the spline data was successfully retrieved}
	//----------------------------------------------------------------------------------------
	Bool GetRange(Float* xmin, Float* xmax, Float* xsteps, Float* ymin, Float* ymax, Float* ysteps) const;

	//----------------------------------------------------------------------------------------
	/// Sets the range of the spline.
	/// @param[in] xmin								The minimum X value.
	/// @param[in] xmax								The maximum X value.
	/// @param[in] xsteps							The X steps value.
	/// @param[in] ymin								The minimum Y value.
	/// @param[in] ymax								The maximum Y value.
	/// @param[in] ysteps							The Y steps value.
	//----------------------------------------------------------------------------------------
	void SetRange(Float xmin, Float xmax, Float xsteps, Float ymin, Float ymax, Float ysteps);

	//----------------------------------------------------------------------------------------
	/// Adapts the internal [@em 0, @em 1] range of the old spline GUI to the range set by [@formatParam{xmin}, @formatParam{xmax}, @formatParam{ymin}, @formatParam{ymax}].
	/// @param[in] xmin								The minimum X value.
	/// @param[in] xmax								The maximum X value.
	/// @param[in] xsteps							The X steps value.
	/// @param[in] ymin								The minimum Y value.
	/// @param[in] ymax								The maximum Y value.
	/// @param[in] ysteps							The Y steps value.
	//----------------------------------------------------------------------------------------
	void AdaptRange(Float xmin, Float xmax, Float xsteps, Float ymin, Float ymax, Float ysteps);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Copies the SplineData values into the destination spline data @formatParam{pDest}.
	/// @param[in] pDest							The destination spline data. @callerOwnsPointed{spline data}
	/// @return												@trueIfOtherwiseFalse{the spline data was successfully copied to @formatParam{pDest}}
	//----------------------------------------------------------------------------------------
	Bool CopyTo(SplineData* pDest);

	/// @}

private:
	SplineData();
	~SplineData();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// @markInternal
class SplineDataTypeClass
{
public:
	static Int32 GetId();

	static CustomDataType*	AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool ReadData (CustomDataType* d, HyperFile* hf, Int32 level);

	static Bool GetParameter(const CustomDataType* data, const DescID& id, GeData& t_data, Int32& flags);
	static Bool SetDParameter(CustomDataType* data, const DescID& id, const GeData& t_data, Int32& flags);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack (pop)
}

#endif	// CUSTOMGUI_SPLINECONTROL_H__
