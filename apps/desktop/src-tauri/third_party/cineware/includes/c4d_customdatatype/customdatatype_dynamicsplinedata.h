/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_DYNAMICSPLINEDATA_H__
#define CUSTOMDATATYPE_DYNAMICSPLINEDATA_H__

#include "c4d_customdatatype.h"

namespace cineware
{
#pragma pack (push, 8)

class PointTag;
class TangentTag;
class SegmentTag;
class AliasTrans;
class HyperFile;

/// Dynamic spline data type ID.
#define CUSTOMDATATYPE_DYNAMICSPLINEDATA 1029123

//----------------------------------------------------------------------------------------
/// Dynamic spline data type (@ref CUSTOMDATATYPE_DYNAMICSPLINEDATA).
//----------------------------------------------------------------------------------------
class DynamicSplineData : public CustomDataType
{
public:
	//----------------------------------------------------------------------------------------
	/// Retrieves the variables of a dynamic spline data.
	/// @param[out] ptag							Assigned a PointTag containing the points of the dynamic spline, or @formatConstant{nullptr} in case of an error.\n
	///																The tag contains the same number of points as the object, though this should be always checked.
	/// @param[out] htag							Assigned a TangentTag with the Hermite tangents of each point, or @formatConstant{nullptr} in case of an error.\n
	///																The tag contains the same number of tangents as the object, though this should be always checked.
	/// @param[out] t_stag						Assigned a SegmentTag with the segments of the dynamic spline, or @formatConstant{nullptr} in case of an error.\n
	///																The tag contains the same number of segments as the object, though this should always be checked.
	//----------------------------------------------------------------------------------------
	void GetVariableTags(PointTag*& ptag, TangentTag*& htag, SegmentTag*& t_stag);

private:
	DynamicSplineData();
	~DynamicSplineData();
};

/// @markInternal
class DynamicSplineDataTypeClass
{
public:
	static Int32 GetId();

	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);
};

#pragma pack (pop)
}

#endif	// CUSTOMDATATYPE_DYNAMICSPLINEDATA_H__
