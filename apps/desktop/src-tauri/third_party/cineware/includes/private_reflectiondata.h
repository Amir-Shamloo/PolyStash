/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef REFLECTIONDATA_H__
#define REFLECTIONDATA_H__

#include "c4d_file.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

static const Int32 REFLECTION_DISK_LEVEL			= 11;
static const Int32 REFLECTION_DATA_OFFSET			= 10000;
static const Int32 REFLECTION_DATA_STEP_SIZE	= 20;

/// @markInternal
class XRLayersDataTypeClass
{
public:
	static Int32						GetId();

	static CustomDataType*	AllocData();
	static void							FreeData(CustomDataType *pData);

	static Bool							CopyData(const CustomDataType *pSrc, CustomDataType *pDest, AliasTrans *pAliasTrans);
	static Int32						Compare(const CustomDataType *pData1, const CustomDataType *pData2);

	static Bool							WriteData(const CustomDataType *pData, HyperFile *pFile);
	static Bool							ReadData(CustomDataType *pData, HyperFile *pFile, Int32 nLevel);
};

#pragma pack (pop)
}

#endif	// REFLECTIONDATA_H__
