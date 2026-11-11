/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_OMSETTINGS_H__
#define C4D_OMSETTINGS_H__

#include "c4d_basesettings.h"

namespace cineware
{
#pragma pack (push, 8)

// Object Manager settings
class OMSettings : public BaseSettings
{
public:
	Bool sort_name;
	Bool show_layered;
	Bool show_flat;
	Bool show_vertical;

	static NodeData* Alloc() { return NewObj(OMSettings); }

	virtual Bool BS_Init();
	virtual void BS_Free() { }
	virtual Bool BS_CopyTo(BaseSettings* dest);
	virtual Bool BS_ReadfromContainer(BaseContainer* bc, BaseDocument* doc);
	virtual Bool BS_WriteToContainer(BaseContainer* bc, BaseDocument* doc);
};

#pragma pack (pop)
}

#endif	// C4D_OMSETTINGS_H__
