/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_PLA_H__
#define CUSTOMDATATYPE_PLA_H__

#include "c4d_customdatatype.h"

namespace cineware
{
#pragma pack (push, 8)

class PointTag;
class TangentTag;
class AliasTrans;
class HyperFile;

/// PLA data type ID.
#define CUSTOMDATATYPE_PLA		100004811
#define CUSTOMDATATYPE_MT_TAG	100004838

//----------------------------------------------------------------------------------------
/// PLA data type (@ref CUSTOMDATATYPE_PLA).
//----------------------------------------------------------------------------------------
class PLAData : public CustomDataType
{
public:
	//----------------------------------------------------------------------------------------
	/// Retrieves the variables of a PLA key.
	/// @param[out] ptag							Assigned a PointTag containing the points of the PLA key, or @formatConstant{nullptr} in case of an error.\n
	///																The tag contains the same number of points as the object, though this should be always checked.
	/// @param[out] htag							Assigned a TangentTag with the Hermite tangents of each point, or @formatConstant{nullptr} in case of an error.\n
	///																The tag contains the same number of tangents as the object, though this should be always checked.
	//----------------------------------------------------------------------------------------
	void GetVariableTags(PointTag*& ptag, TangentTag*& htag);

private:
	PLAData();
	~PLAData();
};

/// @markInternal
class PLADataTypeClass
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

#endif	// CUSTOMDATATYPE_PLA_H__
