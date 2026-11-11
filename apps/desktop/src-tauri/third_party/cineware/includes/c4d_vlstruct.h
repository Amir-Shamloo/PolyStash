/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_VLSTRUCT_H__
#define C4D_VLSTRUCT_H__

#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class VLStruct
{
public:
	VLStruct();

	Bool	 customcol, falloff, edgefalloff, colfalloff, additive, anglescale;
	Float	 strength, edgestrength, jitter;
	Float	 innerdistance, outerdistance;
	Vector outerdist_rel;
	Float	 brightness, sampledist, dust;
};

#pragma pack (pop)
}

#endif	// C4D_VLSTRUCT_H__
