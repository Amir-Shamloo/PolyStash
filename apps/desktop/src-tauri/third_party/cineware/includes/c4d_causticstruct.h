/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CAUSTICSTRUCT_H__
#define C4D_CAUSTICSTRUCT_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class CausticStruct
{
public:
	CausticStruct();

	Bool	cs_enable, vcs_enable;
	Float	cs_energy, vcs_energy, innerradius, outerradius;
	Int32	cs_photons, vcs_photons, falloff;
};

#pragma pack (pop)
}

#endif	// C4D_CAUSTICSTRUCT_H__
