/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_REALARRAY_H__
#define CUSTOMDATATYPE_REALARRAY_H__

#include "c4d_customdatatype.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

/// @markPrivate
#define REAL_ARRAY_ID 180000104

/// @markPrivate
class RealArray : public CustomDataType
{
public:
	CDynArray<Float> values;

	static RealArray* Alloc();
	static void Free(RealArray*& data);
};

/// @markInternal
class RealArrayClass
{
public:
	static Int32 GetId();

	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
};

#pragma pack (pop)
}

#endif	// CUSTOMDATATYPE_REALARRAY_H__
