/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_VNSTRUCT_H__
#define C4D_VNSTRUCT_H__

#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class VNStruct
{
public:
	VNStruct();

	Int32	 noise, octaves;
	Float	 rscale, velocity, amplitude, windvelocity, offset;
	Vector wind, vscale;
	Bool	 local;
};

#pragma pack (pop)
}

#endif	// C4D_VNSTRUCT_H__
