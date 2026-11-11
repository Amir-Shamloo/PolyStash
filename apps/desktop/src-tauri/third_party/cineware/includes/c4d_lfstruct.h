/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_LFSTRUCT_H__
#define C4D_LFSTRUCT_H__

#include "c4d_customdatatype/customgui_lensglow.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct Lens
{
	Float	 pos, size;
	Vector col;
	Int32	 type;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
#define MAXLINSANZ 40
class LFStruct
{
public:
	LFStruct();
	void LensFlareSet(Int32 reflnum);

	Float					 glowsv, reflsv, glowhell, reflhell, scale, rotation, reference;
	Bool					 lightparam, fadebehind, fadenear, fadeoffscreen, scaleglow, scalereflexes;
	Int32					 lensanz, lensglow, lensrefl;

	Lens					 lens[MAXLINSANZ];
	LensGlowStruct lgs;
};

#pragma pack (pop)
}

#endif	// C4D_LFSTRUCT_H__
